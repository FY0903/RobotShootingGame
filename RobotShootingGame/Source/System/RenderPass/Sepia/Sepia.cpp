/*+===================================================================
	File: Sepia.cpp
	Summary: Sepiaレンダーパスの実装
	Author: AT13C192 23 藤原佑埜
	Date: 2026/01/13 16:41 初回作成
			   01/15 10:18 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Sepia.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"

void Sepia::Init()
{
	// ルートシグネチャ生成
	CreateRootSignature();

	// パイプラインステート生成
	CreatePSO();
}

void Sepia::CreateRootSignature()
{
	m_pRootSignature = new RootSignature();
	m_pRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
	m_pRootSignature->AddDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, D3D12_SHADER_VISIBILITY_PIXEL); // SRV
	m_pRootSignature->AddStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR); // スタティックサンプラー
	m_pRootSignature->Create();
}

void Sepia::CreatePSO()
{
	m_pPSO = new PipelineState();
	m_pPSO->SetInputLayout(Vertex::Sprite::InputLayout);
	m_pPSO->SetRootSignature(m_pRootSignature->Get());
	m_pPSO->SetVS(L"Assets/Shader/SpriteVS.cso");
	m_pPSO->SetPS(L"Assets/Shader/SepiaPS.cso");
	m_pPSO->SetDSVFormat(DXGI_FORMAT_UNKNOWN);
	m_pPSO->Create();
}
