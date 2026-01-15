/*+===================================================================
	File: SpriteRenderer.cpp
	Summary: SpriteRendererクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/14 18:31 初回作成
			26/01/15 18:10 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SpriteRenderer.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"
#include "Utility/CameraManager/CameraManager.hpp"
#include "Game/Actor/Actor.hpp"
#include "System/Render/Render.hpp"

void SpriteRenderer::Init(bool is2D)
{
	m_Is2D = is2D;

	Vertex::Sprite vertices[4]{};

	vertices[0].Position = { -0.5f, 0.5f, 0.0f };
	vertices[1].Position = { 0.5f, 0.5f, 0.0f };
	vertices[2].Position = { 0.5f, -0.5f, 0.0f };
	vertices[3].Position = { -0.5f, -0.5f, 0.0f };

	vertices[0].UV = { 0.0f, 0.0f };
	vertices[1].UV = { 1.0f, 0.0f };
	vertices[2].UV = { 1.0f, 1.0f };
	vertices[3].UV = { 0.0f, 1.0f };

	// 頂点バッファの生成
	auto vertexSize = std::size(vertices) * sizeof(Vertex::Sprite);
	auto vertexStride = sizeof(Vertex::Sprite);
	m_pVB = new VertexBuffer(vertexSize, vertexStride, vertices);
	assert(m_pVB);	// nullptrチェック

	uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };

	// インデックスバッファの生成
	auto indexSize = std::size(indices) * sizeof(uint32_t);
	m_pIB = new IndexBuffer(indexSize, indices);
	assert(m_pIB);	// nullptrチェック
	m_IndexCount = static_cast<int>(std::size(indices));

	auto material = m_Owner->GetMaterial();

	// 定数バッファの生成
	m_pWVPCBs = material->SetCBAtRegister(0, sizeof(CB::WVP)); // レジスタ番号0にWVP用定数バッファを設定
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		assert(m_pWVPCBs[i]);	// nullptrチェック
		CB::WVP* WVPptr = m_pWVPCBs[i]->GetPtr<CB::WVP>();
		WVPptr->WorldMat = m_Owner->GetTransform().GetWorldMatrixFloat4x4(false);

		// 2D描画の場合はビュー行列と射影行列を2D用に設定
		if (m_Is2D)
		{
			WVPptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->Get2DViewMatrixFloat4x4(false);
			WVPptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->Get2DProjectionMatrixFloat4x4(true);
		}
		else
		{
			WVPptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->Get3DViewMatrixFloat4x4(false);
			WVPptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->Get3DProjectionMatrixFloat4x4(false);
		}
	}
}

void SpriteRenderer::Init(size_t cell, float spacing)
{
	std::vector<Vertex::Sprite> vertices;
	size_t vertexPerSide = cell + 1;
	vertices.resize(vertexPerSide * vertexPerSide);

	for (size_t y = 0; y < vertexPerSide; ++y)
	{
		for (size_t x = 0; x < vertexPerSide; ++x)
		{
			size_t index = y * vertexPerSide + x;
			float posX = static_cast<float>(x - cell / 2) * spacing;
			float posY = static_cast<float>(y - cell / 2) * spacing;

			vertices[index].Position = { posX, posY, 0.0f };
			vertices[index].UV = { static_cast<float>(x) / cell, static_cast<float>(y) / cell };
		}
	}

	// 頂点バッファの生成
	auto vertexSize = vertices.size() * sizeof(Vertex::Sprite);
	auto vertexStride = sizeof(Vertex::Sprite);
	m_pVB = new VertexBuffer(vertexSize, vertexStride, vertices.data());
	assert(m_pVB);	// nullptrチェック

	std::vector<uint32_t> indices;
	size_t indexCount = vertexPerSide * 2 * cell + (cell - 1) * 2;
	indices.resize(indexCount);

	size_t i = 0;
	for (size_t y = 0; y < cell; ++y)
	{
		for (size_t x = 0; x < vertexPerSide; ++x)
		{
			indices[i++] = static_cast<uint32_t>(y * vertexPerSide + x);
			indices[i++] = static_cast<uint32_t>((y + 1) * vertexPerSide + x);
		}

		if (y != cell - 1)
		{
			// ディジェクションインデックスの追加
			indices[i++] = static_cast<uint32_t>((y + 1) * vertexPerSide + (vertexPerSide - 1));
			indices[i++] = static_cast<uint32_t>((y + 1) * vertexPerSide);
		}
	}

	// インデックスバッファの生成
	auto indexSize = indices.size() * sizeof(uint32_t);
	m_pIB = new IndexBuffer(indexSize, indices.data());
	assert(m_pIB);	// nullptrチェック
	m_IndexCount = static_cast<int>(indices.size());

	auto material = m_Owner->GetMaterial();

	// 定数バッファの生成
	m_pWVPCBs = material->SetCBAtRegister(0, sizeof(CB::WVP)); // レジスタ番号0にWVP用定数バッファを設定
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		assert(m_pWVPCBs[i]);	// nullptrチェック
		CB::WVP* WVPptr = m_pWVPCBs[i]->GetPtr<CB::WVP>();
		WVPptr->WorldMat = m_Owner->GetTransform().GetWorldMatrixFloat4x4(false);
		if (m_Is2D)
		{
			WVPptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->Get2DViewMatrixFloat4x4(false);
			WVPptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->Get2DProjectionMatrixFloat4x4(true);
		}
		else
		{
			WVPptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->Get3DViewMatrixFloat4x4(false);
			WVPptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->Get3DProjectionMatrixFloat4x4(false);
		}
	}
}

void SpriteRenderer::Update()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();
	auto material = m_Owner->GetMaterial();

	// 定数バッファの更新
	CB::WVP* ptr = m_pWVPCBs[currentIndex]->GetPtr<CB::WVP>();
	ptr->WorldMat = m_Owner->GetTransform().GetWorldMatrixFloat4x4(false);
	
	// 2D描画の場合はビュー行列と射影行列の更新をスキップ
	if (m_Is2D) return;
	ptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->Get3DViewMatrixFloat4x4(false);
	ptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->Get3DProjectionMatrixFloat4x4(false);
}

void SpriteRenderer::Draw()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();		// 現在のバックバッファのインデックスを取得

	// レンダーアイテムの設定
	Render::RenderItem item{};
	item.pMaterial = m_Owner->GetMaterial();
	item.type = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	item.pVBs.push_back(m_pVB);
	item.pIBs.push_back(m_pIB);
	if (!m_Owner->GetMaterial()->IsOpaque())
		item.sortKey = CameraManager::GetInstance().CalculateDistanceToMainCamera(m_Owner->GetTransform().GetWorldMatrixFloat4x4(false));
	item.indexCounts.push_back(static_cast<UINT>(m_IndexCount));
	item.meshSize = 1;

	// レンダーキューにレンダーアイテムを登録
	Render::GetInstance().EnqueueRenderItem(item);
}

void SpriteRenderer::Uninit()
{
	if (m_pVB)
	{
		delete m_pVB;
		m_pVB = nullptr;
	}

	if (m_pIB)
	{
		delete m_pIB;
		m_pIB = nullptr;
	}

	m_pWVPCBs.fill(nullptr);
}
