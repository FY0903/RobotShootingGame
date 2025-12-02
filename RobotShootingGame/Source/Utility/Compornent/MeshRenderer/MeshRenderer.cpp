/*+===================================================================
	File: MeshRenderer.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/14 17:22:58 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "MeshRenderer.hpp"
#include "Game/Actor/Actor.hpp"
#include "Utility/CameraManager/CameraManager.hpp"
#include "Utility/Compornent/SkeletalAnimator/SkeletalAnimator.hpp"

void MeshRenderer::Init(Model* pModel)
{
	m_pModel = pModel;

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

		// -Z面
		vertices[4].Position = { 1.0f, 1.0f, -1.0f };
		vertices[5].Position = { -1.0f, 1.0f, -1.0f };
		vertices[6].Position = { -1.0f, -1.0f, -1.0f };
		vertices[7].Position = { 1.0f, -1.0f, -1.0f };

		vertices[4].UV = { 0.0f, 0.0f };
		vertices[5].UV = { 1.0f, 0.0f };
		vertices[6].UV = { 1.0f, 1.0f };
		vertices[7].UV = { 0.0f, 1.0f };

		// +X面
		vertices[8].Position = { 1.0f, 1.0f, 1.0f };
		vertices[9].Position = { 1.0f, 1.0f, -1.0f };
		vertices[10].Position = { 1.0f, -1.0f, -1.0f };
		vertices[11].Position = { 1.0f, -1.0f, 1.0f };

		vertices[8].UV = { 0.0f, 0.0f };
		vertices[9].UV = { 1.0f, 0.0f };
		vertices[10].UV = { 1.0f, 1.0f };
		vertices[11].UV = { 0.0f, 1.0f };

		// -X面
		vertices[12].Position = { -1.0f, 1.0f, -1.0f };
		vertices[13].Position = { -1.0f, 1.0f, 1.0f };
		vertices[14].Position = { -1.0f, -1.0f, 1.0f };
		vertices[15].Position = { -1.0f, -1.0f, -1.0f };

		vertices[12].UV = { 0.0f, 0.0f };
		vertices[13].UV = { 1.0f, 0.0f };
		vertices[14].UV = { 1.0f, 1.0f };
		vertices[15].UV = { 0.0f, 1.0f };

		// +Y面
		vertices[16].Position = { -1.0f, 1.0f, -1.0f };
		vertices[17].Position = { 1.0f, 1.0f, -1.0f };
		vertices[18].Position = { 1.0f, 1.0f, 1.0f };
		vertices[19].Position = { -1.0f, 1.0f, 1.0f };

		vertices[16].UV = { 0.0f, 0.0f };
		vertices[17].UV = { 1.0f, 0.0f };
		vertices[18].UV = { 1.0f, 1.0f };
		vertices[19].UV = { 0.0f, 1.0f };

		// -Y面
		vertices[20].Position = { -1.0f, -1.0f, 1.0f };
		vertices[21].Position = { 1.0f, -1.0f, 1.0f };
		vertices[22].Position = { 1.0f, -1.0f, -1.0f };
		vertices[23].Position = { -1.0f, -1.0f, -1.0f };

		vertices[20].UV = { 0.0f, 0.0f };
		vertices[21].UV = { 1.0f, 0.0f };
		vertices[22].UV = { 1.0f, 1.0f };
		vertices[23].UV = { 0.0f, 1.0f };

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

	CB::WVP* ptr = m_pWVPCB[currentIndex]->GetPtr<CB::WVP>();
	ptr->WorldMat = m_Owner->GetTransform().GetWorldMatrixFloat4x4(false);
	ptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->GetViewMatrixFloat4x4(false);
	ptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->GetProjectionMatrixFloat4x4(false);

	if (!m_Owner->GetComponent<SkeletalAnimator>() || !m_pModel) return;

	CB::BoneMatrix* bonePtr = m_pBoneMatrixCB[currentIndex]->GetPtr<CB::BoneMatrix>();
	std::copy_n(m_pModel->GetBoneMatCB().data(), m_pModel->GetBoneMatCB().size(), bonePtr->BoneMat);
}

void MeshRenderer::Draw()
{
	auto material = m_Owner->GetMaterial();
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();
	auto commandList = Engine::GetInstance().GetCommandList();				// コマンドリストを取得

	commandList->SetGraphicsRootSignature(material->GetRootSignature()->Get());					// ルートシグネチャを設定
	commandList->SetPipelineState(material->GetPipelineState()->Get());							// パイプラインステートを設定
	commandList->SetGraphicsRootConstantBufferView(0, m_pWVPCB[currentIndex]->GetAddress());	// 定数バッファを設定
	
	// ボーン行列用定数バッファとディスクリプタヒープの設定
	if (m_pModel)
	{
		auto materialHeap = material->GetDescriptorHeap()->GetHeap();	// ディスクリプタヒープを取得
		commandList->SetGraphicsRootConstantBufferView(1, m_pBoneMatrixCB[currentIndex]->GetAddress()); // ボーン行列用定数バッファを設定
		commandList->SetDescriptorHeaps(1, &materialHeap);														// ディスクリプタヒープを設定
	}
	
	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
		auto vbView = m_pVertexBuffers[i]->GetView();	// 頂点バッファビューを取得
		auto ibView = m_pIndexBuffers[i]->GetView();	// インデックスバッファビューを取得

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// プリミティブトポロジーを設定
		commandList->IASetVertexBuffers(0, 1, &vbView);								// 頂点バッファを設定
		commandList->IASetIndexBuffer(&ibView);										// インデックスバッファを設定

		if (m_pModel)
		{
			auto materialHandle = material->GetDescriptorHandle(i);	// マテリアルのディスクリプタハンドルを取得
			commandList->SetGraphicsRootDescriptorTable(2, materialHandle->HandleGPU);	// ディスクリプタテーブルを設定
		}

		commandList->DrawIndexedInstanced(static_cast<UINT>(m_Meshes[i].Indices.size()), 1, 0, 0, 0);	// 描画
	}
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

	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		delete m_pWVPCB[i];
		m_pWVPCB[i] = nullptr;
	}

	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		delete m_pBoneMatrixCB[i];
		m_pBoneMatrixCB[i] = nullptr;
	}

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

	// 定数バッファの生成
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_pWVPCB[i] = new ConstantBuffer(sizeof(CB::WVP));
		assert(m_pWVPCB[i]);	// nullptrチェック
		CB::WVP* WVPptr = m_pWVPCB[i]->GetPtr<CB::WVP>();
		WVPptr->WorldMat = m_Owner->GetTransform().GetWorldMatrixFloat4x4(false);
		WVPptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->GetViewMatrixFloat4x4(false);
		WVPptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->GetProjectionMatrixFloat4x4(false);

		if (!m_pModel) continue;

		m_pBoneMatrixCB[i] = new ConstantBuffer(sizeof(DirectX::XMMATRIX) * m_pModel->GetBones().size());
		assert(m_pBoneMatrixCB[i]); // nullptrチェック
		for (size_t j = 0; j < m_pModel->GetBones().size(); ++j)
		{
			CB::BoneMatrix* bonePtr = m_pBoneMatrixCB[i]->GetPtr<CB::BoneMatrix>();
			bonePtr->BoneMat[j] = DirectX::XMMatrixIdentity();
		}
	}
}
