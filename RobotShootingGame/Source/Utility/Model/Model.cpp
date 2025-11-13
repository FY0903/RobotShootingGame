/*+===================================================================
	File: Model.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/08 9:11:08 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Model.hpp"
#include "Game/Camera/Camera.hpp"

Model::Model(std::vector<Mesh> Meshes, Camera& Camera)
	: m_Meshes(Meshes), m_Camera(Camera)
{
	// メッシュの数だけ頂点バッファを用意する
	m_pVertexBuffers.reserve(m_Meshes.size());
	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
		auto size = sizeof(Vertex::Mesh) * m_Meshes[i].Vertices.size();
		auto stride = sizeof(Vertex::Mesh);
		auto vertices = m_Meshes[i].Vertices.data();
		auto pVB = new VertexBuffer(size, stride, vertices);
		assert(pVB);	// nullptrチェック

		m_pVertexBuffers.push_back(pVB);
	}

	// メッシュの数だけインデックスバッファを用意する
	m_pIndexBuffers.reserve(m_Meshes.size());
	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
		auto size = sizeof(uint32_t) * m_Meshes[i].Indices.size();
		auto indices = m_Meshes[i].Indices.data();
		auto pIB = new IndexBuffer(size, indices);
		assert(pIB);	// nullptrチェック

		m_pIndexBuffers.push_back(pIB);
	}

	// マテリアル用のディスクリプタヒープを生成
	m_pDescriptorHeap = new DescriptorHeap();
	m_pMaterialHandles.clear();
	m_pTextures.resize(m_Meshes.size());
	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
		m_pTextures[i] = new Texture();
		assert(m_pTextures[i]);	// nullptrチェック

		m_pTextures[i]->Load(m_Meshes[i].DiffuseMap);
		auto handle = m_pDescriptorHeap->Register(m_pTextures[i]->Resource(), m_pTextures[i]->ViewDesc());
		m_pMaterialHandles.push_back(handle);
	}

	// 定数バッファの生成
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_pConstantBuffer[i] = new ConstantBuffer(sizeof(Transform));
		assert(m_pConstantBuffer[i]);	// nullptrチェック

		Transform* ptr = m_pConstantBuffer[i]->GetPtr<Transform>();
		ptr->World = DirectX::XMMatrixIdentity();
		ptr->View = m_Camera.GetViewMatrix();
		ptr->Proj = m_Camera.GetProjectionMatrix();
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

Model::~Model()
{
	// 頂点バッファの解放
	for (auto vb : m_pVertexBuffers)
	{
		delete vb;
	}
	m_pVertexBuffers.clear();

	// インデックスバッファの解放
	for (auto ib : m_pIndexBuffers)
	{
		delete ib;
	}
	m_pIndexBuffers.clear();

	// テクスチャの解放
	for (auto tex : m_pTextures)
	{
		delete tex;
	}
	m_pTextures.clear();

	// 定数バッファの解放
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		delete m_pConstantBuffer[i];
		m_pConstantBuffer[i] = nullptr;
	}

	// ルートシグネチャの解放
	if (m_pRootSignature)
	{
		delete m_pRootSignature;
		m_pRootSignature = nullptr;
	}

	// パイプラインステートの解放
	if (m_pPipelineState)
	{
		delete m_pPipelineState;
		m_pPipelineState = nullptr;
	}

	// ディスクリプタヒープの解放
	if (m_pDescriptorHeap)
	{
		delete m_pDescriptorHeap;
		m_pDescriptorHeap = nullptr;
	}

	// ディスクリプタハンドルの解放
	m_pMaterialHandles.clear();
	m_pMaterialHandles.shrink_to_fit();
}

void Model::Update()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得
	Transform* ptr = m_pConstantBuffer[currentIndex]->GetPtr<Transform>();	// 定数バッファのポインタを取得

	ptr->View = m_Camera.GetViewMatrix();
	ptr->Proj = m_Camera.GetProjectionMatrix();
}

void Model::Draw()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得
	auto commandList = Engine::GetInstance().GetCommandList();				// コマンドリストを取得
	auto materialHeap = m_pDescriptorHeap->GetHeap();						// ディスクリプタヒープを取得

	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
		auto vbView = m_pVertexBuffers[i]->GetView();	// 頂点バッファビューを取得
		auto ibView = m_pIndexBuffers[i]->GetView();	// インデックスバッファビューを取得

		commandList->SetGraphicsRootSignature(m_pRootSignature->Get());			// ルートシグネチャを設定
		commandList->SetPipelineState(m_pPipelineState->Get());					// パイプラインステートを設定
		commandList->SetGraphicsRootConstantBufferView(0, m_pConstantBuffer[currentIndex]->GetAddress());	// 定数バッファを設定

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// プリミティブトポロジーを設定
		commandList->IASetVertexBuffers(0, 1, &vbView);								// 頂点バッファを設定
		commandList->IASetIndexBuffer(&ibView);										// インデックスバッファを設定

		commandList->SetDescriptorHeaps(1, &materialHeap);									// ディスクリプタヒープを設定
		commandList->SetGraphicsRootDescriptorTable(1, m_pMaterialHandles[i]->HandleGPU);	// ディスクリプタテーブルを設定

		commandList->DrawIndexedInstanced(static_cast<UINT>(m_Meshes[i].Indices.size()), 1, 0, 0, 0);	// 描画
	}
}
