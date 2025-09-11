/*+===================================================================
	File: Object.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/04 10:24:12 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Object.hpp"
#include "../../System/Window/Window.hpp"

Object::Object(Camera& camera)
	: m_Camera(camera)
{
	// 頂点バッファの生成
	Vertex::Mesh vertices[4 * 6]{};

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

	// 頂点バッファの生成
	auto vertexSize = std::size(vertices) * sizeof(Vertex::Mesh);
	auto vertexStride = sizeof(Vertex::Mesh);
	m_pVertexBuffer = new VertexBuffer(vertexSize, vertexStride, vertices);
	assert(m_pVertexBuffer);	// nullptrチェック

	uint32_t indices[] =
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

	// インデックスバッファの生成
	auto indexSize = std::size(indices) * sizeof(uint32_t);
	m_pIndexBuffer = new IndexBuffer(indexSize, indices);
	assert(m_pIndexBuffer);	// nullptrチェック

	Mesh mesh{};
	mesh.Vertices = std::vector<Vertex::Mesh>(std::begin(vertices), std::end(vertices));
	mesh.Indices = std::vector<uint32_t>(std::begin(indices), std::end(indices));
	m_Meshes.push_back(mesh);

	// 定数バッファの生成
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_pTransformBuffer[i] = new ConstantBuffer(sizeof(Transform));
		assert(m_pTransformBuffer[i]);	// nullptrチェック

		Transform* ptr = m_pTransformBuffer[i]->GetPtr<Transform>();
		ptr->World = DirectX::XMMatrixIdentity();
		ptr->View = m_Camera.GetViewMatrix();
		ptr->Proj = m_Camera.GetProjectionMatrix();
	}

	// ルートシグネチャの生成
	m_pRootSignature = new RootSignature();
	assert(m_pRootSignature);	// nullptrチェック
	m_pRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_ALL); // 定数バッファ
	m_pRootSignature->Create();

	// パイプラインステートの生成
	m_pPipelineState = new PipelineState();
	assert(m_pPipelineState);	// nullptrチェック
	m_pPipelineState->SetInputLayout(Vertex::Mesh::InputLayout);
	m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
	m_pPipelineState->SetVS(L"Assets/Shader/SimpleVS.cso");
	m_pPipelineState->SetPS(L"Assets/Shader/DebugPS.cso");
	m_pPipelineState->Create();
}

Object::~Object()
{
	delete m_pVertexBuffer;
	m_pVertexBuffer = nullptr;

	delete m_pIndexBuffer;
	m_pIndexBuffer = nullptr;

	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		delete m_pTransformBuffer[i];
		m_pTransformBuffer[i] = nullptr;
	}

	delete m_pRootSignature;
	m_pRootSignature = nullptr;

	delete m_pPipelineState;
	m_pPipelineState = nullptr;
}

void Object::Update()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得
	Transform* ptr = m_pTransformBuffer[currentIndex]->GetPtr<Transform>();	// 定数バッファのポインタを取得
#if 0

	static float angle = 0.0f;
	angle += 0.01f;
	
	if (angle > DirectX::XM_2PI)
	{
		angle -= DirectX::XM_2PI;
	}

	ptr->World = DirectX::XMMatrixRotationY(angle);
#endif

	ptr->View = m_Camera.GetViewMatrix();
	ptr->Proj = m_Camera.GetProjectionMatrix();
}

void Object::Draw()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得
	auto commandList = Engine::GetInstance().GetCommandList();				// コマンドリストを取得

	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
		auto vbView = m_pVertexBuffer->GetView();	// 頂点バッファビューを取得
		auto ibView = m_pIndexBuffer->GetView();	// インデックスバッファビューを取得

		commandList->SetGraphicsRootSignature(m_pRootSignature->Get());			// ルートシグネチャを設定
		commandList->SetPipelineState(m_pPipelineState->Get());					// パイプラインステートを設定
		commandList->SetGraphicsRootConstantBufferView(0, m_pTransformBuffer[currentIndex]->GetAddress());	// 定数バッファを設定

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// プリミティブトポロジーを設定
		commandList->IASetVertexBuffers(0, 1, &vbView);								// 頂点バッファを設定
		commandList->IASetIndexBuffer(&ibView);										// インデックスバッファを設定

		commandList->DrawIndexedInstanced(static_cast<UINT>(m_Meshes[i].Indices.size()), 1, 0, 0, 0);	// 描画
	}
}
