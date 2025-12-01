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
#include <SimpleMath.h>
#include "Game/Actor/Actor.hpp"
#include "Utility/CameraManager/CameraManager.hpp"
#include "Utility/Compornent/SkeletalAnimator/SkeletalAnimator.hpp"

void MeshRenderer::Init(Model* pModel)
{
	m_pModel = pModel;
	std::vector<Model::Mesh> meshes = m_pModel->GetMeshes();

	m_pVertexBuffers.reserve(meshes.size());
	m_pIndexBuffers.reserve(meshes.size());
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		auto& mesh = meshes[i];

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
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		auto& mesh = meshes[i];
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

		m_pBoneMatrixCB[i] = new ConstantBuffer(sizeof(DirectX::XMMATRIX) * m_pModel->GetBones().size());
		assert(m_pBoneMatrixCB[i]); // nullptrチェック
		for (size_t j = 0; j < m_pModel->GetBones().size(); ++j)
		{
			CB::BoneMatrix* bonePtr = m_pBoneMatrixCB[i]->GetPtr<CB::BoneMatrix>();
			bonePtr->BoneMat[j] = DirectX::XMMatrixIdentity();
		}
	}
}

void MeshRenderer::Update()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();

	CB::WVP* ptr = m_pWVPCB[currentIndex]->GetPtr<CB::WVP>();
	ptr->WorldMat = m_Owner->GetTransform().GetWorldMatrixFloat4x4(false);
	ptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->GetViewMatrixFloat4x4(false);
	ptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->GetProjectionMatrixFloat4x4(false);

	if (!m_Owner->GetComponent<SkeletalAnimator>()) return;

	CB::BoneMatrix* bonePtr = m_pBoneMatrixCB[currentIndex]->GetPtr<CB::BoneMatrix>();
	std::copy_n(m_pModel->GetBoneMatCB().data(), m_pModel->GetBoneMatCB().size(), bonePtr->BoneMat);
}

void MeshRenderer::Draw()
{
	auto material = m_Owner->GetMaterial();

	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();
	auto commandList = Engine::GetInstance().GetCommandList();				// コマンドリストを取得
	auto materialHeap = material->GetDescriptorHeap()->GetHeap();			// ディスクリプタヒープを取得

	std::vector<Model::Mesh> meshes = m_pModel->GetMeshes();

	for (size_t i = 0; i < meshes.size(); ++i)
	{
		auto vbView = m_pVertexBuffers[i]->GetView();			// 頂点バッファビューを取得
		auto ibView = m_pIndexBuffers[i]->GetView();			// インデックスバッファビューを取得
		auto materialHandle = material->GetDescriptorHandle(i);	// マテリアルのディスクリプタハンドルを取得

		commandList->SetGraphicsRootSignature(material->GetRootSignature()->Get());		// ルートシグネチャを設定
		commandList->SetPipelineState(material->GetPipelineState()->Get());				// パイプラインステートを設定
		commandList->SetGraphicsRootConstantBufferView(0, m_pWVPCB[currentIndex]->GetAddress());		// 定数バッファを設定
		commandList->SetGraphicsRootConstantBufferView(1, m_pBoneMatrixCB[currentIndex]->GetAddress()); // ボーン行列用定数バッファを設定

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// プリミティブトポロジーを設定
		commandList->IASetVertexBuffers(0, 1, &vbView);								// 頂点バッファを設定
		commandList->IASetIndexBuffer(&ibView);										// インデックスバッファを設定

		commandList->SetDescriptorHeaps(1, &materialHeap);								// ディスクリプタヒープを設定
		commandList->SetGraphicsRootDescriptorTable(2, materialHandle->HandleGPU);	// ディスクリプタテーブルを設定

		commandList->DrawIndexedInstanced(static_cast<UINT>(meshes[i].Indices.size()), 1, 0, 0, 0);	// 描画
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
