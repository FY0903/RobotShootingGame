/*+===================================================================
	File: SpriteRenderer.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/14 18:31:18 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SpriteRenderer.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"
#include "Utility/CameraManager/CameraManager.hpp"
#include "Game/Actor/Actor.hpp"
#include "System/Render/Render.hpp"

void SpriteRenderer::Init()
{
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
	m_pVertexBuffer = new VertexBuffer(vertexSize, vertexStride, vertices);
	assert(m_pVertexBuffer);	// nullptrチェック

	uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };

	// インデックスバッファの生成
	auto indexSize = std::size(indices) * sizeof(uint32_t);
	m_pIndexBuffer = new IndexBuffer(indexSize, indices);
	assert(m_pIndexBuffer);	// nullptrチェック

	// 定数バッファの生成
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_pWVPCB[i] = new ConstantBuffer(sizeof(CB::WVP));
		assert(m_pWVPCB[i]);	// nullptrチェック
		CB::WVP* ptr = m_pWVPCB[i]->GetPtr<CB::WVP>();
		ptr->WorldMat = m_Owner->GetTransform().GetWorldMatrixFloat4x4(false);
		ptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->Get3DViewMatrixFloat4x4(false);
		ptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->Get3DProjectionMatrixFloat4x4(false);
	}
}

void SpriteRenderer::Update()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();
	CB::WVP* ptr = m_pWVPCB[currentIndex]->GetPtr<CB::WVP>();

	ptr->WorldMat = m_Owner->GetTransform().GetWorldMatrixFloat4x4(false);
	ptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->Get3DViewMatrixFloat4x4(false);
	ptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->Get3DProjectionMatrixFloat4x4(false);
}

void SpriteRenderer::Draw()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();		// 現在のバックバッファのインデックスを取得

	// レンダーアイテムの設定
	Render::RenderItem item{};
	item.pMaterial = m_Owner->GetMaterial();
	item.pWVPCB = m_pWVPCB[currentIndex];
	item.type = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	item.pVertexBuffers.push_back(m_pVertexBuffer);
	item.pIndexBuffers.push_back(m_pIndexBuffer);
	if (!m_Owner->GetMaterial()->IsOpaque())
		item.sortKey = CameraManager::GetInstance().CalculateDistanceToMainCamera(m_Owner->GetTransform().GetWorldMatrixFloat4x4(false));
	item.indexCounts.push_back(6);
	item.MeshSize = 1;

	// レンダーキューにレンダーアイテムを登録
	Render::GetInstance().EnqueueRenderItem(item);
}

void SpriteRenderer::Uninit()
{
	if (m_pVertexBuffer)
	{
		delete m_pVertexBuffer;
		m_pVertexBuffer = nullptr;
	}

	if (m_pIndexBuffer)
	{
		delete m_pIndexBuffer;
		m_pIndexBuffer = nullptr;
	}

	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		if (m_pWVPCB[i])
		{
			delete m_pWVPCB[i];
			m_pWVPCB[i] = nullptr;
		}
	}
}
