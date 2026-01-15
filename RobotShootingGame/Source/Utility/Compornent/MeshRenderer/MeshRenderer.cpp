/*+===================================================================
	File: MeshRenderer.cpp
	Summary: MeshRendererクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/14 17:22 初回作成
			26/01/15 18:02 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "MeshRenderer.hpp"
#include "Game/Actor/Actor.hpp"
#include "Utility/CameraManager/CameraManager.hpp"
#include "Utility/Compornent/SkeletalAnimator/SkeletalAnimator.hpp"
#include "System/Render/Render.hpp"

void MeshRenderer::Init(Model* pModel)
{
	m_pModel = pModel;

	// モデルが指定されている場合はそのメッシュで初期化
	if (m_pModel)
	{
		Init(m_pModel->GetMeshes());
	}
	else
	{
		std::vector<Model::Mesh> meshes{};
		meshes.resize(1);

		std::vector<Vertex::Mesh> vertices{};
		vertices.resize(4 * 6);

		// +Z面
		vertices[0].Position = { -1.0f, 1.0f, 1.0f };
		vertices[1].Position = { 1.0f, 1.0f, 1.0f };
		vertices[2].Position = { 1.0f, -1.0f, 1.0f };
		vertices[3].Position = { -1.0f, -1.0f, 1.0f };

		vertices[0].UV = { 0.0f, 0.0f };
		vertices[1].UV = { 1.0f, 0.0f };
		vertices[2].UV = { 1.0f, 1.0f };
		vertices[3].UV = { 0.0f, 1.0f };

		vertices[0].Normal = { 0.0f, 0.0f, 1.0f };
		vertices[1].Normal = { 0.0f, 0.0f, 1.0f };
		vertices[2].Normal = { 0.0f, 0.0f, 1.0f };
		vertices[3].Normal = { 0.0f, 0.0f, 1.0f };

		vertices[0].Tangent = { 1.0f, 0.0f, 0.0f };
		vertices[1].Tangent = { 1.0f, 0.0f, 0.0f };
		vertices[2].Tangent = { 1.0f, 0.0f, 0.0f };
		vertices[3].Tangent = { 1.0f, 0.0f, 0.0f };

		// -Z面
		vertices[4].Position = { 1.0f, 1.0f, -1.0f };
		vertices[5].Position = { -1.0f, 1.0f, -1.0f };
		vertices[6].Position = { -1.0f, -1.0f, -1.0f };
		vertices[7].Position = { 1.0f, -1.0f, -1.0f };

		vertices[4].UV = { 0.0f, 0.0f };
		vertices[5].UV = { 1.0f, 0.0f };
		vertices[6].UV = { 1.0f, 1.0f };
		vertices[7].UV = { 0.0f, 1.0f };

		vertices[4].Normal = { 0.0f, 0.0f, -1.0f };
		vertices[5].Normal = { 0.0f, 0.0f, -1.0f };
		vertices[6].Normal = { 0.0f, 0.0f, -1.0f };
		vertices[7].Normal = { 0.0f, 0.0f, -1.0f };

		vertices[4].Tangent = { -1.0f, 0.0f, 0.0f };
		vertices[5].Tangent = { -1.0f, 0.0f, 0.0f };
		vertices[6].Tangent = { -1.0f, 0.0f, 0.0f };
		vertices[7].Tangent = { -1.0f, 0.0f, 0.0f };

		// +X面
		vertices[8].Position = { 1.0f, 1.0f, 1.0f };
		vertices[9].Position = { 1.0f, 1.0f, -1.0f };
		vertices[10].Position = { 1.0f, -1.0f, -1.0f };
		vertices[11].Position = { 1.0f, -1.0f, 1.0f };

		vertices[8].UV = { 0.0f, 0.0f };
		vertices[9].UV = { 1.0f, 0.0f };
		vertices[10].UV = { 1.0f, 1.0f };
		vertices[11].UV = { 0.0f, 1.0f };

		vertices[8].Normal = { 1.0f, 0.0f, 0.0f };
		vertices[9].Normal = { 1.0f, 0.0f, 0.0f };
		vertices[10].Normal = { 1.0f, 0.0f, 0.0f };
		vertices[11].Normal = { 1.0f, 0.0f, 0.0f };

		vertices[8].Tangent = { 0.0f, 0.0f, -1.0f };
		vertices[9].Tangent = { 0.0f, 0.0f, -1.0f };
		vertices[10].Tangent = { 0.0f, 0.0f, -1.0f };
		vertices[11].Tangent = { 0.0f, 0.0f, -1.0f };

		// -X面
		vertices[12].Position = { -1.0f, 1.0f, -1.0f };
		vertices[13].Position = { -1.0f, 1.0f, 1.0f };
		vertices[14].Position = { -1.0f, -1.0f, 1.0f };
		vertices[15].Position = { -1.0f, -1.0f, -1.0f };

		vertices[12].UV = { 0.0f, 0.0f };
		vertices[13].UV = { 1.0f, 0.0f };
		vertices[14].UV = { 1.0f, 1.0f };
		vertices[15].UV = { 0.0f, 1.0f };

		vertices[12].Normal = { -1.0f, 0.0f, 0.0f };
		vertices[13].Normal = { -1.0f, 0.0f, 0.0f };
		vertices[14].Normal = { -1.0f, 0.0f, 0.0f };
		vertices[15].Normal = { -1.0f, 0.0f, 0.0f };

		vertices[12].Tangent = { 0.0f, 0.0f, 1.0f };
		vertices[13].Tangent = { 0.0f, 0.0f, 1.0f };
		vertices[14].Tangent = { 0.0f, 0.0f, 1.0f };
		vertices[15].Tangent = { 0.0f, 0.0f, 1.0f };

		// +Y面
		vertices[16].Position = { -1.0f, 1.0f, -1.0f };
		vertices[17].Position = { 1.0f, 1.0f, -1.0f };
		vertices[18].Position = { 1.0f, 1.0f, 1.0f };
		vertices[19].Position = { -1.0f, 1.0f, 1.0f };

		vertices[16].UV = { 0.0f, 0.0f };
		vertices[17].UV = { 1.0f, 0.0f };
		vertices[18].UV = { 1.0f, 1.0f };
		vertices[19].UV = { 0.0f, 1.0f };

		vertices[16].Normal = { 0.0f, 1.0f, 0.0f };
		vertices[17].Normal = { 0.0f, 1.0f, 0.0f };
		vertices[18].Normal = { 0.0f, 1.0f, 0.0f };
		vertices[19].Normal = { 0.0f, 1.0f, 0.0f };

		vertices[16].Tangent = { 1.0f, 0.0f, 0.0f };
		vertices[17].Tangent = { 1.0f, 0.0f, 0.0f };
		vertices[18].Tangent = { 1.0f, 0.0f, 0.0f };
		vertices[19].Tangent = { 1.0f, 0.0f, 0.0f };

		// -Y面
		vertices[20].Position = { -1.0f, -1.0f, 1.0f };
		vertices[21].Position = { 1.0f, -1.0f, 1.0f };
		vertices[22].Position = { 1.0f, -1.0f, -1.0f };
		vertices[23].Position = { -1.0f, -1.0f, -1.0f };

		vertices[20].UV = { 0.0f, 0.0f };
		vertices[21].UV = { 1.0f, 0.0f };
		vertices[22].UV = { 1.0f, 1.0f };
		vertices[23].UV = { 0.0f, 1.0f };

		vertices[20].Normal = { 0.0f, -1.0f, 0.0f };
		vertices[21].Normal = { 0.0f, -1.0f, 0.0f };
		vertices[22].Normal = { 0.0f, -1.0f, 0.0f };
		vertices[23].Normal = { 0.0f, -1.0f, 0.0f };

		vertices[20].Tangent = { 1.0f, 0.0f, 0.0f };
		vertices[21].Tangent = { 1.0f, 0.0f, 0.0f };
		vertices[22].Tangent = { 1.0f, 0.0f, 0.0f };
		vertices[23].Tangent = { 1.0f, 0.0f, 0.0f };

		// 頂点カラーの設定
		for (auto& vertex : vertices)
		{
			vertex.Color = { 1.0f, 1.0f, 1.0f, 1.0f };	// 白色
		}

		std::vector<uint32_t> indices =
		{
			// +Z面
			0, 1, 2,
			0, 2, 3,
			// -Z面
			4, 5, 6,
			4, 6, 7,
			// +X面
			8, 9, 10,
			8, 10, 11,
			// -X面
			12, 13, 14,
			12, 14, 15,
			// +Y面
			16, 17, 18,
			16, 18, 19,
			// -Y面
			20, 21, 22,
			20, 22, 23,
		};

		meshes[0].Vertices = vertices;
		meshes[0].Indices = indices;
		Init(meshes);
	}
}

void MeshRenderer::Update()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();
	auto material = m_Owner->GetMaterial();

	// WVP行列の更新
	CB::WVP* ptr = m_pWVPCBs[currentIndex]->GetPtr<CB::WVP>();
	ptr->WorldMat = m_Owner->GetTransform().GetWorldMatrixFloat4x4(false);
	ptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->Get3DViewMatrixFloat4x4(false);
	ptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->Get3DProjectionMatrixFloat4x4(false);

	// ボーン行列の更新
	if (!m_Owner->GetComponent<SkeletalAnimator>() || !m_pModel) return;

	CB::BoneMatrix* bonePtr = m_pBoneCBs[currentIndex]->GetPtr<CB::BoneMatrix>();
	std::copy_n(m_pModel->GetBoneMatCB().data(), m_pModel->GetBoneMatCB().size(), bonePtr->BoneMat);
}

void MeshRenderer::Draw()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();
	
	// レンダーアイテムの設定
	Render::RenderItem item{};
	item.pMaterial = m_Owner->GetMaterial();
	item.type = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	if (!m_Owner->GetMaterial()->IsOpaque())
		item.sortKey = CameraManager::GetInstance().CalculateDistanceToMainCamera(m_Owner->GetTransform().GetWorldMatrixFloat4x4(false));

	// 頂点バッファとインデックスバッファの設定
	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
		item.pVBs.push_back(m_pVertexBuffers[i]);
		item.pIBs.push_back(m_pIndexBuffers[i]);
		item.indexCounts.push_back(static_cast<UINT>(m_Meshes[i].Indices.size()));
	}
	item.meshSize = m_Meshes.size();	// メッシュ数を設定

	// レンダーキューに登録
	Render::GetInstance().EnqueueRenderItem(item);
}

void MeshRenderer::Uninit()
{
	for (auto& pVB : m_pVertexBuffers)
	{
		delete pVB;
	}

	for (auto& pIB : m_pIndexBuffers)
	{
		delete pIB;
	}

	m_pWVPCBs.fill(nullptr);
	m_pBoneCBs.fill(nullptr);

	m_pModel = nullptr;
}

void MeshRenderer::Init(std::vector<Model::Mesh> meshes)
{
	m_Meshes = std::move(meshes);

	m_pVertexBuffers.reserve(m_Meshes.size());
	m_pIndexBuffers.reserve(m_Meshes.size());
	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
		auto& mesh = m_Meshes[i];

		// 頂点バッファの生成
		auto vertexSize = mesh.Vertices.size() * sizeof(Vertex::Mesh);
		auto vertexStride = sizeof(Vertex::Mesh);
		auto pVertexBuffer = new VertexBuffer(vertexSize, vertexStride, mesh.Vertices.data());
		assert(pVertexBuffer);	// nullptrチェック
		m_pVertexBuffers.push_back(pVertexBuffer);

		// インデックスバッファの生成
		auto indexSize = mesh.Indices.size() * sizeof(uint32_t);
		auto pIndexBuffer = new IndexBuffer(indexSize, mesh.Indices.data());
		assert(pIndexBuffer);	// nullptrチェック
		m_pIndexBuffers.push_back(pIndexBuffer);
	}

	// マテリアルの設定
	auto material = m_Owner->GetMaterial();
	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
		auto& mesh = m_Meshes[i];
		material->SetTexture(mesh.DiffuseMap);
	}
	material->SetIsOpaque(true);	// モデルは不透明とする

	// 定数バッファの生成
	m_pWVPCBs = material->SetCBAtRegister(0, sizeof(CB::WVP)); // レジスタ番号0にWVP用定数バッファを設定
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		assert(m_pWVPCBs[i]);	// nullptrチェック
		CB::WVP* WVPptr = m_pWVPCBs[i]->GetPtr<CB::WVP>();
		WVPptr->WorldMat = m_Owner->GetTransform().GetWorldMatrixFloat4x4(false);
		WVPptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->Get3DViewMatrixFloat4x4(false);
		WVPptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->Get3DProjectionMatrixFloat4x4(false);
	}

	// ボーン行列用定数バッファの生成
	if (m_pModel)
	{
		m_pBoneCBs = material->SetCBAtRegister(1, sizeof(CB::BoneMatrix)); // レジスタ番号1にボーン行列用定数バッファを設定

		for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
		{
			assert(m_pBoneCBs[i]);	// nullptrチェック
			CB::BoneMatrix* bonePtr = m_pBoneCBs[i]->GetPtr<CB::BoneMatrix>();
			for (size_t j = 0; j < m_pModel->GetBones().size(); ++j)
			{
				bonePtr->BoneMat[j] = DirectX::XMMatrixIdentity();
			}
		}
	}
}
