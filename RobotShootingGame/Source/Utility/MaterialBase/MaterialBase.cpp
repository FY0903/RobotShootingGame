/*+===================================================================
	File: MaterialBase.cpp
	Summary: MaterialBaseクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/30 1:11:33 初回作成
			26/01/15 18:55 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "MaterialBase.hpp"
#include "Utility/Texture/Texture.hpp"

MaterialBase::MaterialBase(size_t renderTargetNum, bool alphaBlend)
{
	// インスタンスの生成
	m_pRootSignature = new RootSignature();
	m_pPSO = new PipelineState(renderTargetNum, alphaBlend);
}

MaterialBase::~MaterialBase()
{
	// インスタンスの解放
	delete m_pRootSignature;
	m_pRootSignature = nullptr;

	delete m_pPSO;
	m_pPSO = nullptr;
}

void MaterialBase::SetCBV(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility)
{
	if (!m_pRootSignature) return;
	m_pRootSignature->AddRootParameter(shaderRegister, visibility);
	++m_rootParameterIndex;
}

void MaterialBase::SetCBV(UINT shaderRegister, UINT numDescriptors, D3D12_SHADER_VISIBILITY visibility)
{
	if (!m_pRootSignature) return;
	m_pRootSignature->AddDescriptorRange(shaderRegister, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, numDescriptors, visibility);
	++m_rootParameterIndex;
}

void MaterialBase::SetSRV(UINT shaderRegister, UINT numDescriptors, D3D12_SHADER_VISIBILITY visibility)
{
	if (!m_pRootSignature) return;
	m_pRootSignature->AddDescriptorRange(shaderRegister, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, numDescriptors, visibility);
	++m_rootParameterIndex;
}

void MaterialBase::SetStaticSampler(UINT shaderRegister, D3D12_FILTER filter)
{
	if (!m_pRootSignature) return;
	m_pRootSignature->AddStaticSampler(shaderRegister, filter);
}

void MaterialBase::SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout, InputLayoutType type)
{
	if (!m_pPSO) return;
	m_pPSO->SetInputLayout(layout);
	m_InputLayoutType = type;
}

void MaterialBase::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type)
{
	if (!m_pPSO) return;
	m_pPSO->SetPrimitiveTopologyType(type);
}

void MaterialBase::SetRTVFormat(DXGI_FORMAT format, size_t index)
{
	if (!m_pPSO) return;
	m_pPSO->SetRTVFormat(format, index);
}

void MaterialBase::SetDSVFormat(DXGI_FORMAT format)
{
	if (!m_pPSO) return;
	m_pPSO->SetDSVFormat(format);
}

void MaterialBase::Create()
{
	if (!m_pRootSignature || !m_pPSO) return;

	// ルートシグネチャの作成
	m_pRootSignature->Create();

	// パイプラインステートの設定
	m_pPSO->SetRootSignature(m_pRootSignature->Get());
	m_pPSO->SetVS(m_VSFilepath);
	m_pPSO->SetPS(m_PSFilepath);
	m_pPSO->Create();
}
