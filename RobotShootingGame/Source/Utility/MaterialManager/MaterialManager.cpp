/*+===================================================================
	File: MaterialManager.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/30 5:41:16 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "MaterialManager.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"
#include "Utility/Texture/Texture.hpp"
#include "Utility/RenderTarget/RenderTarget.hpp"

Material::Material(MaterialBase* pMaterial)
	: m_pMaterial(pMaterial)
{
	m_pDescriptorHeap = new DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
}

Material::~Material()
{
	m_pDescriptorHandle.clear();

	delete m_pDescriptorHeap;
	m_pDescriptorHeap = nullptr;
	m_pMaterial = nullptr;
}

void Material::SetTexture(Texture* pTexture)
{
	if (!m_pDescriptorHeap || !pTexture) return;

	DescriptorHandle* handle = m_pDescriptorHeap->Register(pTexture->Resource(), pTexture->ViewDesc());
	if (!handle) return;
	m_pDescriptorHandle.push_back(handle);
}

void Material::SetTexture(RenderTarget* pRTV)
{
	if (!m_pDescriptorHeap || !pRTV) return;

	DescriptorHandle* handle = m_pDescriptorHeap->Register(pRTV->Resource(), pRTV->ViewDesc());
	if (!handle) return;
	m_pDescriptorHandle.push_back(handle);
}

DescriptorHeap* Material::GetDescriptorHeap() const
{
	// ヒープが存在しない場合はnullptrを返す
	if (!m_pDescriptorHeap) return nullptr;
	
	return m_pDescriptorHeap;
}

DescriptorHandle* Material::GetDescriptorHandle(size_t index) const
{
	// 範囲外チェック
	if (index >= m_pDescriptorHandle.size() || !m_pDescriptorHandle[index] || !m_pDescriptorHandle.size()) return nullptr;

	return m_pDescriptorHandle[index];
}

RootSignature* Material::GetRootSignature() const
{
	// マテリアルが存在しない場合はnullptrを返す
	if (!m_pMaterial) return nullptr;

	return m_pMaterial->GetRootSignature();
}

PipelineState* Material::GetPipelineState() const
{
	// マテリアルが存在しない場合はnullptrを返す
	if (!m_pMaterial) return nullptr;

	return m_pMaterial->GetPipelineState();
}

void MaterialManager::Init()
{
	// Spriteマテリアルの作成
	auto sprite = CreateMaterialBase("Sprite");
	sprite->SetVSFilepath(L"Assets/Shader/SpriteVS.cso");
	sprite->SetPSFilepath(L"Assets/Shader/SpritePS.cso");
	sprite->SetInputLayout(Vertex::Sprite::InputLayout);
	sprite->SetCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	sprite->SetSRV(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	sprite->SetStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	sprite->Create();

	// Lineマテリアルの作成
	auto line = CreateMaterialBase("Line");
	line->SetVSFilepath(L"Assets/Shader/LineVS.cso");
	line->SetPSFilepath(L"Assets/Shader/LinePS.cso");
	line->SetInputLayout(Vertex::Line::InputLayout);
	line->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
	line->SetCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	line->Create();

	// DebugMeshマテリアルの作成
	auto debugMesh = CreateMaterialBase("Debug");
	debugMesh->SetVSFilepath(L"Assets/Shader/SimpleVS.cso");
	debugMesh->SetPSFilepath(L"Assets/Shader/DebugPS.cso");
	debugMesh->SetInputLayout(Vertex::Mesh::InputLayout);
	debugMesh->SetCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	debugMesh->Create();

	// SkeletalMeshマテリアルの作成
	auto skeletalMesh = CreateMaterialBase("Skeletal");
	skeletalMesh->SetVSFilepath(L"Assets/Shader/SkeletalMeshVS.cso");
	skeletalMesh->SetPSFilepath(L"Assets/Shader/SimplePS.cso");
	skeletalMesh->SetInputLayout(Vertex::Mesh::InputLayout);
	skeletalMesh->SetCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	skeletalMesh->SetCBV(1, D3D12_SHADER_VISIBILITY_VERTEX);
	skeletalMesh->SetSRV(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	skeletalMesh->SetStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	skeletalMesh->Create();
}

Material* MaterialManager::CreateMaterial(const std::string& materialName)
{
	MaterialBase* pMaterial = GetMaterial(materialName);
	if (!pMaterial) return nullptr;

	Material* pInstance = new Material(pMaterial);
	m_MaterialInstanceData[materialName].push_back(pInstance);

	return pInstance;
}

MaterialBase* MaterialManager::CreateMaterialBase(const std::string& name)
{
	if (m_MaterialData.find(name) != m_MaterialData.end())
	{
		return m_MaterialData[name];
	}

	MaterialBase* pMaterial = new MaterialBase();
	m_MaterialData[name] = pMaterial;

	return pMaterial;
}

MaterialBase* MaterialManager::GetMaterial(const std::string& name)
{
	if (m_MaterialData.find(name) != m_MaterialData.end())
	{
		return m_MaterialData[name];
	}
	return nullptr;
}

MaterialManager::~MaterialManager()
{
	for (auto& pair : m_MaterialData)
	{
		delete pair.second;
		pair.second = nullptr;
	}
	m_MaterialData.clear();

	for (auto& pair : m_MaterialInstanceData)
	{
		for (auto* instance : pair.second)
		{
			delete instance;
			instance = nullptr;
		}
		pair.second.clear();
	}
	m_MaterialInstanceData.clear();
}
