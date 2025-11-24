/*+===================================================================
	File: Line.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/22 19:06:18 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Line.hpp"
#include "Game/Actor/Actor.hpp"
#include "Utility/CameraManager/CameraManager.hpp"

void Line::Update()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();
	CB::WVP* ptr = m_pCB[currentIndex]->GetPtr<CB::WVP>();

	ptr->WorldMat = m_Owner->GetTransform().GetWorldMatrixFloat4x4(false);
	ptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->GetViewMatrixFloat4x4(false);
	ptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->GetProjectionMatrixFloat4x4(false);
}

void Line::Draw()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();
	auto commandList = Engine::GetInstance().GetCommandList();

	auto vbView = m_pVertexBuffer->GetView();

	commandList->SetGraphicsRootSignature(m_pRootSignature->Get());
	commandList->SetPipelineState(m_pPipelineState->Get());
	commandList->SetGraphicsRootConstantBufferView(0, m_pCB[currentIndex]->GetAddress());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	commandList->IASetVertexBuffers(0, 1, &vbView);

	commandList->DrawInstanced(static_cast<UINT>(m_Lines.size()), 1, 0, 0);
}

void Line::Uninit()
{
	delete m_pVertexBuffer;
	m_pVertexBuffer = nullptr;

	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		delete m_pCB[i];
		m_pCB[i] = nullptr;
	}

	delete m_pRootSignature;
	m_pRootSignature = nullptr;

	delete m_pPipelineState;
	m_pPipelineState = nullptr;
}

void Line::AddPoint(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT4& color)
{
	m_Lines.push_back({ start, color });
	m_Lines.push_back({ end, color });
}

void Line::Create()
{
	auto vertexSize = sizeof(Vertex::Line) * m_Lines.size();
	auto vertexStride = sizeof(Vertex::Line);
	m_pVertexBuffer = new VertexBuffer(vertexSize, vertexStride, m_Lines.data());

	// 定数バッファの生成
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_pCB[i] = new ConstantBuffer(sizeof(CB::WVP));
		assert(m_pCB[i]);	// nullptrチェック
		CB::WVP* ptr = m_pCB[i]->GetPtr<CB::WVP>();
		ptr->WorldMat = m_Owner->GetTransform().GetWorldMatrixFloat4x4();
		ptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->GetViewMatrixFloat4x4();
		ptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->GetProjectionMatrixFloat4x4();
	}
	
	// ルートシグネチャの生成
	m_pRootSignature = new RootSignature();
	assert(m_pRootSignature);	// nullptrチェック
	m_pRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
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

void Line::Clear()
{
	m_Lines.clear();
}
