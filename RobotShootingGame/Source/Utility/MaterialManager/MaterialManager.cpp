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

Material::Material(MaterialBase* pMaterial)
	: m_pMaterial(pMaterial)
{
	m_pDescriptorHeap = new DescriptorHeap();
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

void MaterialManager::Init()
{
	auto sprite = CreateMaterialBase("Sprite");
	sprite->SetVSFilepath(L"Assets/Shader/SpriteVS.cso");
	sprite->SetPSFilepath(L"Assets/Shader/SpritePS.cso");
	sprite->SetInputLayout(Vertex::Sprite::InputLayout);
	sprite->SetCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	sprite->SetSRV(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	sprite->SetStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	sprite->Create();
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
