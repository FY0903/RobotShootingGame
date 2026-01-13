/*+===================================================================
	File: Sepia.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2026/01/13 16:41:42 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Sepia.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"

void Sepia::Init()
{
	CreateRootSignature();
	CreatePSO();
}

void Sepia::CreateRootSignature()
{
	m_pSnapshotRootSignature = new RootSignature();
	m_pSnapshotRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
	m_pSnapshotRootSignature->AddDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, D3D12_SHADER_VISIBILITY_PIXEL); // SRV
	m_pSnapshotRootSignature->AddStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR); // スタティックサンプラー
	m_pSnapshotRootSignature->Create();
}

void Sepia::CreatePSO()
{
	m_pSnapshotPSO = new PipelineState();
	m_pSnapshotPSO->SetInputLayout(Vertex::Sprite::InputLayout);
	m_pSnapshotPSO->SetRootSignature(m_pSnapshotRootSignature->Get());
	m_pSnapshotPSO->SetVS(L"Assets/Shader/SpriteVS.cso");
	m_pSnapshotPSO->SetPS(L"Assets/Shader/SepiaPS.cso");
	m_pSnapshotPSO->SetDSVFormat(DXGI_FORMAT_UNKNOWN);
	m_pSnapshotPSO->Create();
}
