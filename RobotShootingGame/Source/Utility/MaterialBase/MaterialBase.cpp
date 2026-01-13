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

MaterialBase::MaterialBase(size_t renderTargetNum, bool alphaBlend)
{
	// インスタンスの生成
	m_pSnapshotRootSignature = new RootSignature();
	m_pSnapshotPSO = new PipelineState(renderTargetNum, alphaBlend);
}

MaterialBase::~MaterialBase()
{
	// インスタンスの解放
	delete m_pSnapshotRootSignature;
	m_pSnapshotRootSignature = nullptr;

	delete m_pSnapshotPSO;
	m_pSnapshotPSO = nullptr;
}

void MaterialBase::SetCBV(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility)
{
	if (!m_pSnapshotRootSignature) return;
	m_pSnapshotRootSignature->AddRootParameter(shaderRegister, visibility);
	++m_rootParameterIndex;
}

void MaterialBase::SetCBV(UINT shaderRegister, UINT numDescriptors, D3D12_SHADER_VISIBILITY visibility)
{
	if (!m_pSnapshotRootSignature) return;
	m_pSnapshotRootSignature->AddDescriptorRange(shaderRegister, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, numDescriptors, visibility);
	++m_rootParameterIndex;
}

void MaterialBase::SetSRV(UINT shaderRegister, UINT numDescriptors, D3D12_SHADER_VISIBILITY visibility)
{
	if (!m_pSnapshotRootSignature) return;
	m_pSnapshotRootSignature->AddDescriptorRange(shaderRegister, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, numDescriptors, visibility);
	++m_rootParameterIndex;
}

void MaterialBase::SetStaticSampler(UINT shaderRegister, D3D12_FILTER filter)
{
	if (!m_pSnapshotRootSignature) return;
	m_pSnapshotRootSignature->AddStaticSampler(shaderRegister, filter);
}

void MaterialBase::SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout, InputLayoutType type)
{
	if (!m_pSnapshotPSO) return;
	m_pSnapshotPSO->SetInputLayout(layout);
	m_InputLayoutType = type;
}

void MaterialBase::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type)
{
	if (!m_pSnapshotPSO) return;
	m_pSnapshotPSO->SetPrimitiveTopologyType(type);
}

void MaterialBase::SetRTVFormat(DXGI_FORMAT format, size_t index)
{
	if (!m_pSnapshotPSO) return;
	m_pSnapshotPSO->SetRTVFormat(format, index);
}

void MaterialBase::SetDSVFormat(DXGI_FORMAT format)
{
	if (!m_pSnapshotPSO) return;
	m_pSnapshotPSO->SetDSVFormat(format);
}

void MaterialBase::Create()
{
	if (!m_pSnapshotRootSignature || !m_pSnapshotPSO) return;

	// ルートシグネチャの作成
	m_pSnapshotRootSignature->Create();

	// パイプラインステートの設定
	m_pSnapshotPSO->SetRootSignature(m_pSnapshotRootSignature->Get());
	m_pSnapshotPSO->SetVS(m_VSFilepath);
	m_pSnapshotPSO->SetPS(m_PSFilepath);
	m_pSnapshotPSO->Create();
}
