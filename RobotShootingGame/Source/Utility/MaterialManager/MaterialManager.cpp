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
}

Material::~Material()
{
	m_pDescriptorHandle.clear();

	delete m_pDescriptorHeap;
	m_pDescriptorHeap = nullptr;

	for (auto& pair : m_pCBVs)
	{
		for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
		{
			delete pair.second[i];
			pair.second[i] = nullptr;
		}
	}

	m_pMaterial = nullptr;
}

void Material::SetTexture(Texture* pTexture)
{
	if (!pTexture) return;

	// ディスクリプタヒープが存在しない場合は作成
	if (!m_pDescriptorHeap)
		m_pDescriptorHeap = new DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	m_IsOpaque = pTexture->IsOpaque();
	DescriptorHandle* handle = m_pDescriptorHeap->Register(pTexture->Resource(), pTexture->ViewDesc());
	if (!handle) return;
	m_pDescriptorHandle.push_back(handle);
}

void Material::SetTexture(RenderTarget* pRTV)
{
	if (!pRTV) return;

	// ディスクリプタヒープが存在しない場合は作成
	if (!m_pDescriptorHeap)
		m_pDescriptorHeap = new DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	DescriptorHandle* handle = m_pDescriptorHeap->Register(pRTV->Resource(), pRTV->ViewDesc());
	if (!handle) return;
	m_pDescriptorHandle.push_back(handle);
}

std::array<ConstantBuffer*, FRAME_BUFFER_COUNT>& Material::SetCBAtRegister(int index, size_t size)
{
	auto& array = m_pCBVs[index];
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		if (!array[i])
		{
			array[i] = new ConstantBuffer(size);
		}
	}
	m_CBSize[index] = size;
	return array;
}

const std::array<ConstantBuffer*, FRAME_BUFFER_COUNT>* Material::GetCBByRegister(int index) const
{
	auto it = m_pCBVs.find(index);
	if (it == m_pCBVs.end()) return nullptr;

	return &it->second;
}

ConstantBuffer* Material::GetCBByRegisterForFrame(int index, size_t frameIndex) const
{
	auto it = m_pCBVs.find(index);
	if (it == m_pCBVs.end()) return nullptr;
	if (frameIndex >= FRAME_BUFFER_COUNT) return nullptr;
	
	return it->second[frameIndex];
}

size_t Material::GetCBSizeForRegister(int index) const
{
	auto it = m_CBSize.find(index);
	if (it == m_CBSize.end()) return -1;

	return it->second;
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

int Material::GetRootParameterIndex() const
{
	// マテリアルが存在しない場合は-1を返す
	if (!m_pMaterial) return -1;

	return m_pMaterial->GetRootParameterIndex();
}

int Material::GetCBSize() const
{
	return static_cast<int>(m_pCBVs.size());
}

void MaterialManager::Init()
{
	// MEMO: マテリアルの作成はCBVを登録してからSRVを登録すること
	// SRVを先に登録するとルートパラメータのインデックスがずれるから

	// Spriteマテリアルの作成
	auto sprite = CreateMaterialBase("Sprite", 1, true);
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

	auto gbufferMesh = CreateMaterialBase("DebugGBuffer", 3);
	gbufferMesh->SetVSFilepath(L"Assets/Shader/MeshGBufferVS.cso");
	gbufferMesh->SetPSFilepath(L"Assets/Shader/MeshGBufferPS.cso");
	gbufferMesh->SetInputLayout(Vertex::Mesh::InputLayout);
	gbufferMesh->SetCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	gbufferMesh->SetSRV(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	gbufferMesh->SetStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	gbufferMesh->Create();

	auto skeletalGBuffer = CreateMaterialBase("SkeletalGBuffer", 3);
	skeletalGBuffer->SetVSFilepath(L"Assets/Shader/SkeletalMeshGBufferVS.cso");
	skeletalGBuffer->SetPSFilepath(L"Assets/Shader/SkeletalMeshGBufferPS.cso");
	skeletalGBuffer->SetInputLayout(Vertex::Mesh::InputLayout);
	skeletalGBuffer->SetCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	skeletalGBuffer->SetCBV(1, D3D12_SHADER_VISIBILITY_VERTEX);
	skeletalGBuffer->SetSRV(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	skeletalGBuffer->SetStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	skeletalGBuffer->Create();

	auto water = CreateMaterialBase("Water");
	water->SetVSFilepath(L"Assets/Shader/WaterVS.cso");
	water->SetPSFilepath(L"Assets/Shader/WaterPS.cso");
	water->SetInputLayout(Vertex::Sprite::InputLayout);
	water->SetCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	water->SetCBV(1, D3D12_SHADER_VISIBILITY_PIXEL);
	water->SetSRV(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	water->SetStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	water->Create();

	auto ui = CreateMaterialBase("UI", 1, true);
	ui->SetVSFilepath(L"Assets/Shader/SpriteVS.cso");
	ui->SetPSFilepath(L"Assets/Shader/SpritePS.cso");
	ui->SetInputLayout(Vertex::Sprite::InputLayout);
	ui->SetCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	ui->SetSRV(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	ui->SetStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	ui->SetDSVFormat(DXGI_FORMAT_UNKNOWN);
	ui->Create();

	auto text = CreateMaterialBase("Text", 1, true);
	text->SetVSFilepath(L"Assets/Shader/SpriteVS.cso");
	text->SetPSFilepath(L"Assets/Shader/TextPS.cso");
	text->SetInputLayout(Vertex::Sprite::InputLayout);
	text->SetCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	text->SetCBV(0, D3D12_SHADER_VISIBILITY_PIXEL);
	text->SetSRV(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	text->SetStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	text->SetDSVFormat(DXGI_FORMAT_UNKNOWN);
	text->Create();
}

Material* MaterialManager::CreateMaterial(const std::string& materialName)
{
	MaterialBase* pMaterial = GetMaterial(materialName);
	if (!pMaterial) return nullptr;

	Material* pInstance = new Material(pMaterial);
	m_MaterialInstanceData[materialName].push_back(pInstance);

	return pInstance;
}

MaterialBase* MaterialManager::CreateMaterialBase(const std::string& name, size_t renderTargetNum, bool alphaBlend)
{
	if (m_MaterialData.find(name) != m_MaterialData.end())
	{
		return m_MaterialData[name];
	}

	MaterialBase* pMaterial = new MaterialBase(renderTargetNum, alphaBlend);
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
