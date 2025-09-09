/*+===================================================================
	File: Line.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/08 19:15:43 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Line.hpp"
#include "../../Game/Camera/Camera.hpp"

Line::Line(Camera& camera)
	: m_Camera(camera)
{
}

Line::~Line()
{
	delete m_pVertexBuffer;
	m_pVertexBuffer = nullptr;

	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		delete m_pConstantBuffer[i];
		m_pConstantBuffer[i] = nullptr;
	}

	delete m_pRootSignature;
	m_pRootSignature = nullptr;

	delete m_pPipelineState;
	m_pPipelineState = nullptr;
}

void Line::Update()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得
	Transform* ptr = m_pConstantBuffer[currentIndex]->GetPtr<Transform>();	// 定数バッファのポインタを取得

	ptr->View = m_Camera.GetViewMatrix();
	ptr->Proj = m_Camera.GetProjectionMatrix();
}

void Line::Draw()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得
	auto commandList = Engine::GetInstance().GetCommandList();				// コマンドリストを取得

	auto vbView = m_pVertexBuffer->GetView();	// 頂点バッファビューを取得
	
	commandList->SetGraphicsRootSignature(m_pRootSignature->Get());			// ルートシグネチャを設定
	commandList->SetPipelineState(m_pPipelineState->Get());					// パイプラインステートを設定
	commandList->SetGraphicsRootConstantBufferView(0, m_pConstantBuffer[currentIndex]->GetAddress());	// 定数バッファを設定
	
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);	// プリミティブトポロジーを設定
	commandList->IASetVertexBuffers(0, 1, &vbView);							// 頂点バッファを設定
	
	commandList->DrawInstanced(static_cast<UINT>(m_Lines.size()), 1, 0, 0);	// 描画
}

void Line::AddPoint(DirectX::SimpleMath::Vector3 start, DirectX::SimpleMath::Vector3 end, DirectX::SimpleMath::Vector4 color)
{
	// 線の始点と終点を追加
	m_Lines.push_back({ start, color });
	m_Lines.push_back({ end, color });
}

void Line::Create()
{
	// 頂点バッファの生成
	auto vertexSize = std::size(m_Lines) * sizeof(Vertex::Line);
	auto vertexStride = sizeof(Vertex::Line);
	m_pVertexBuffer = new VertexBuffer(vertexSize, vertexStride, m_Lines.data());

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
	m_pRootSignature->Create();

	// パイプラインステートの生成
	m_pPipelineState = new PipelineState();
	assert(m_pPipelineState);	// nullptrチェック
	m_pPipelineState->SetInputLayout(Vertex::Line::InputLayout);
	m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
	m_pPipelineState->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
	m_pPipelineState->SetVS(L"Assets/Shader/LineVS.cso");
	m_pPipelineState->SetPS(L"Assets/Shader/LinePS.cso");
	m_pPipelineState->Create();
}
