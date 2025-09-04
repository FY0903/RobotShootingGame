/*+===================================================================
	File: Scene.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/02 14:54:10 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Scene.hpp"
#include "../../System/Window/Window.hpp"
#include "../../Utility/Texture2D/Texture2D.hpp"
#include <iterator>

void Scene::Init()
{
	ImportSettings inportSetting =
	{
		L"Assets/Model/spot/spot.fbx",
		m_Meshes,
		false,
		true,
	};

	if (!ModelLoader::GetInstance().Load(inportSetting)) return;

#if 0
	Vertex vertices[4]{};

	vertices[0].Position = { -1.0f, 1.0f, 0.0f };
	vertices[0].Color = { 0.0f, 0.0f, 1.0f, 1.0f };

	vertices[1].Position = { 1.0f, 1.0f, 0.0f };
	vertices[1].Color = { 0.0f, 1.0f, 0.0f, 1.0f };

	vertices[2].Position = { 1.0f, -1.0f, 0.0f };
	vertices[2].Color = { 1.0f, 0.0f, 0.0f, 1.0f };

	vertices[3].Position = { -1.0f, -1.0f, 0.0f };
	vertices[3].Color = { 1.0f, 0.0f, 1.0f, 1.0f };

	// 頂点バッファの生成
	auto vertexSize = std::size(vertices) * sizeof(Vertex);
	auto vertexStride = sizeof(Vertex);
	m_pVertexBuffer = new VertexBuffer(vertexSize, vertexStride, vertices);
	assert(m_pVertexBuffer);	// nullptrチェック

	uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };

	// インデックスバッファの生成
	auto indexSize = std::size(indices) * sizeof(uint32_t);
	m_pIndexBuffer = new IndexBuffer(indexSize, indices);
	assert(m_pIndexBuffer);	// nullptrチェック

	Mesh mesh{};
	mesh.Vertices = std::vector<Vertex>(std::begin(vertices), std::end(vertices));
	mesh.Indices = std::vector<uint32_t>(std::begin(indices), std::end(indices));
	m_Meshes.clear();			// 既存のメッシュデータをクリア
	m_Meshes.shrink_to_fit();	// メモリを解放
	m_Meshes.push_back(mesh);	// メッシュデータを追加

#endif

	// メッシュの数だけ頂点バッファを用意する
	m_pVertexBuffers.reserve(m_Meshes.size());
	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
		auto size = sizeof(Vertex) * m_Meshes[i].Vertices.size();
		auto stride = sizeof(Vertex);
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

	DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(0.0f, 5.0f, 5.0f, 0.0f);
	DirectX::XMVECTOR targetPos = DirectX::XMVectorZero();
	DirectX::XMVECTOR upVec = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	float fov = DirectX::XMConvertToRadians(45.0f);
	float aspect = static_cast<float>(Window::GetInstance().GetWidth()) / static_cast<float>(Window::GetInstance().GetHeight());

	// 定数バッファの生成
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_pConstantBuffer[i] = new ConstantBuffer(sizeof(Transform));
		assert(m_pConstantBuffer[i]);	// nullptrチェック

		Transform* ptr = m_pConstantBuffer[i]->GetPtr<Transform>();
		ptr->World = DirectX::XMMatrixIdentity();
		ptr->View = DirectX::XMMatrixLookAtLH(eyePos, targetPos, upVec);
		ptr->Proj = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, 0.1f, 1000.0f);
	}

	// マテリアル用のディスクリプタヒープを生成
	m_pDescriptorHeap = new DescriptorHeap();
	m_pMaterialHandles.clear();
	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
		auto texPath = m_Meshes[i].DiffuseMap;
		auto mainTex = Texture2D::Get(texPath);
		auto handle = m_pDescriptorHeap->Register(mainTex);
		m_pMaterialHandles.push_back(handle);
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
	m_pPipelineState->SetInputLayout(Vertex::InputLayout);
	m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
	m_pPipelineState->SetVS(L"Assets/Shader/SimpleVS.cso");
	m_pPipelineState->SetPS(L"Assets/Shader/SimplePS.cso");
	m_pPipelineState->Create();

	// Cameraの生成
	m_pCamera = new Camera(eyePos, targetPos, upVec, fov, aspect);

	// Objectの生成
	m_pObject = new Object(*m_pCamera);
}

void Scene::Update()
{
#if 0
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex(); // 現在のバックバッファのインデックスを取得
	Transform* ptr = m_pConstantBuffer[currentIndex]->GetPtr<Transform>();
	static float angle = 0.0f;
	angle += 0.01f;
	if (angle > DirectX::XM_2PI)
	{
		angle -= DirectX::XM_2PI;
	}
	ptr->World = DirectX::XMMatrixRotationY(angle);
#endif
	// Cameraの更新
	m_pCamera->Update();

	// Objectの更新
	m_pObject->Update();
}

void Scene::Draw()
{
#if 0
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
#endif
	// Objectの描画
	m_pObject->Draw();
}

void Scene::UnInit()
{
	delete m_pObject;
	m_pObject = nullptr;

	delete m_pCamera;
	m_pCamera = nullptr;

	for (auto& vb : m_pVertexBuffers)
	{
		delete vb;
		vb = nullptr;
	}

	for (auto& ib : m_pIndexBuffers)
	{
		delete ib;
		ib = nullptr;
	}

	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		delete m_pConstantBuffer[i];
		m_pConstantBuffer[i] = nullptr;
	}

	delete m_pRootSignature;
	m_pRootSignature = nullptr;

	delete m_pPipelineState;
	m_pPipelineState = nullptr;

	m_Meshes.clear();
	m_Meshes.shrink_to_fit();

	delete m_pDescriptorHeap;
	m_pDescriptorHeap = nullptr;

	m_pMaterialHandles.clear();
	m_pMaterialHandles.shrink_to_fit();
}
