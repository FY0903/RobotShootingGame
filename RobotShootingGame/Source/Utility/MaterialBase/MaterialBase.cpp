/*+===================================================================
	File: MaterialBase.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/30 1:11:33 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "MaterialBase.hpp"
#include "Utility/Texture/Texture.hpp"

MaterialBase::MaterialBase()
{
	// インスタンスの生成
	m_pRootSignature = new RootSignature();
	m_pPipelineState = new PipelineState();
}

MaterialBase::~MaterialBase()
{
	// インスタンスの解放
	delete m_pRootSignature;
	m_pRootSignature = nullptr;

	delete m_pPipelineState;
	m_pPipelineState = nullptr;
}

void MaterialBase::SetCBV(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility)
{
	if (!m_pRootSignature) return;
	m_pRootSignature->AddRootParameter(shaderRegister, visibility);
}

void MaterialBase::SetCBV(UINT shaderRegister, UINT numDescriptors, D3D12_SHADER_VISIBILITY visibility)
{
	if (!m_pRootSignature) return;
	m_pRootSignature->AddDescriptorRange(shaderRegister, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, numDescriptors, visibility);
}

void MaterialBase::SetSRV(UINT shaderRegister, UINT numDescriptors, D3D12_SHADER_VISIBILITY visibility)
{
	if (!m_pRootSignature) return;
	m_pRootSignature->AddDescriptorRange(shaderRegister, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, numDescriptors, visibility);
}

void MaterialBase::SetStaticSampler(UINT shaderRegister, D3D12_FILTER filter)
{
	if (!m_pRootSignature) return;
	m_pRootSignature->AddStaticSampler(shaderRegister, filter);
}

void MaterialBase::SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout)
{
	if (!m_pPipelineState) return;
	m_pPipelineState->SetInputLayout(layout);
}

void MaterialBase::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type)
{
	if (!m_pPipelineState) return;
	m_pPipelineState->SetPrimitiveTopologyType(type);
}

void MaterialBase::SetDSVFormat(DXGI_FORMAT format)
{
	if (!m_pPipelineState) return;
	m_pPipelineState->SetDSVFormat(format);
}

void MaterialBase::Create()
{
	if (!m_pRootSignature || !m_pPipelineState) return;

	// ルートシグネチャの作成
	m_pRootSignature->Create();

	// パイプラインステートの設定
	m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
	m_pPipelineState->SetVS(m_VSFilepath);
	m_pPipelineState->SetPS(m_PSFilepath);
	m_pPipelineState->Create();
}
