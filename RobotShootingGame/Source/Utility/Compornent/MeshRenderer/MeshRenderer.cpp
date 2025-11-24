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

	m_pDescriptorHeap = new DescriptorHeap();
	m_pMaterialHandles.clear();
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		auto& mesh = meshes[i];
		auto handle = m_pDescriptorHeap->Register(mesh.DiffuseMap->Resource(), mesh.DiffuseMap->ViewDesc());
		assert(handle);	// nullptrチェック
		m_pMaterialHandles.push_back(handle);
	}

	// 定数バッファの生成
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_pCB[i] = new ConstantBuffer(sizeof(CB::WVP));
		assert(m_pCB[i]);	// nullptrチェック
		CB::WVP* ptr = m_pCB[i]->GetPtr<CB::WVP>();
		ptr->WorldMat = m_Owner->GetTransform().GetWorldMatrixFloat4x4(false);
		ptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->GetViewMatrixFloat4x4(false);
		ptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->GetProjectionMatrixFloat4x4(false);
	}

	// ルートシグネチャの生成
	m_pRootSignature = new RootSignature();
	assert(m_pRootSignature);	// nullptrチェック
	m_pRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_ALL); // 定数バッファ
	m_pRootSignature->AddDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, D3D12_SHADER_VISIBILITY_PIXEL); // テクスチャ
	m_pRootSignature->AddStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR); // スタティックサンプラー
	m_pRootSignature->Create();

	// パイプラインステートの生成
	m_pPipelineState = new PipelineState();
	assert(m_pPipelineState);	// nullptrチェック
	m_pPipelineState->SetInputLayout(Vertex::Mesh::InputLayout);
	m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
	m_pPipelineState->SetVS(L"Assets/Shader/SimpleVS.cso");
	m_pPipelineState->SetPS(L"Assets/Shader/SimplePS.cso");
	m_pPipelineState->Create();
}

void MeshRenderer::Update()
{
}

void MeshRenderer::Draw()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();
	CB::WVP* ptr = m_pCB[currentIndex]->GetPtr<CB::WVP>();

	ptr->WorldMat = m_Owner->GetTransform().GetWorldMatrixFloat4x4(false);
	ptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->GetViewMatrixFloat4x4(false);
	ptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->GetProjectionMatrixFloat4x4(false);

	auto commandList = Engine::GetInstance().GetCommandList();				// コマンドリストを取得
	auto materialHeap = m_pDescriptorHeap->GetHeap();						// ディスクリプタヒープを取得

	std::vector<Model::Mesh> meshes = m_pModel->GetMeshes();

	for (size_t i = 0; i < meshes.size(); ++i)
	{
		auto vbView = m_pVertexBuffers[i]->GetView();		// 頂点バッファビューを取得
		auto ibView = m_pIndexBuffers[i]->GetView();		// インデックスバッファビューを取得
		auto materialHandle = m_pMaterialHandles[i];		// マテリアルのディスクリプタハンドルを取得

		commandList->SetGraphicsRootSignature(m_pRootSignature->Get());			// ルートシグネチャを設定
		commandList->SetPipelineState(m_pPipelineState->Get());					// パイプラインステートを設定
		commandList->SetGraphicsRootConstantBufferView(0, m_pCB[currentIndex]->GetAddress()); // 定数バッファを設定

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// プリミティブトポロジーを設定
		commandList->IASetVertexBuffers(0, 1, &vbView);								// 頂点バッファを設定
		commandList->IASetIndexBuffer(&ibView);										// インデックスバッファを設定

		commandList->SetDescriptorHeaps(1, &materialHeap);								// ディスクリプタヒープを設定
		commandList->SetGraphicsRootDescriptorTable(1, materialHandle->HandleGPU);	// ディスクリプタテーブルを設定

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
		delete m_pCB[i];
		m_pCB[i] = nullptr;
	}

	delete m_pDescriptorHeap;
	m_pDescriptorHeap = nullptr;

	m_pMaterialHandles.clear();

	delete m_pRootSignature;
	m_pRootSignature = nullptr;

	delete m_pPipelineState;
	m_pPipelineState = nullptr;

	m_pModel = nullptr;
}
