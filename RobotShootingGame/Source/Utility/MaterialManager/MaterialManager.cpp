/*+===================================================================
	File: MaterialManager.cpp
	Summary: MaterialManagerクラスのソースファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/30 05:41 初回作成
			26/01/15 18:58 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "MaterialManager.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"
#include "Utility/Texture/Texture.hpp"
#include "Utility/RenderTarget/RenderTarget.hpp"

Material::Material(MaterialBase* pMaterial)
	: m_pMaterial(pMaterial), m_pDescriptorHeap(nullptr), m_IsOpaque(true)
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

MaterialBase::InputLayoutType Material::GetInputLayoutType() const
{
	// マテリアルが存在しない場合はNoneを返す
	if (!m_pMaterial) return MaterialBase::InputLayoutType::None;
		
	return m_pMaterial->GetInputLayoutType();
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
	sprite->SetInputLayout(Vertex::Sprite::InputLayout, MaterialBase::Sprite);
	sprite->SetCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	sprite->SetSRV(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	sprite->SetStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	sprite->Create();

	// Lineマテリアルの作成
	auto line = CreateMaterialBase("Line");
	line->SetVSFilepath(L"Assets/Shader/LineVS.cso");
	line->SetPSFilepath(L"Assets/Shader/LinePS.cso");
	line->SetInputLayout(Vertex::Line::InputLayout, MaterialBase::Line);
	line->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
	line->SetCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	line->Create();

	// DebugMeshマテリアルの作成
	auto debugMesh = CreateMaterialBase("Debug");
	debugMesh->SetVSFilepath(L"Assets/Shader/SimpleVS.cso");
	debugMesh->SetPSFilepath(L"Assets/Shader/DebugPS.cso");
	debugMesh->SetInputLayout(Vertex::SkeletalMesh::InputLayout, MaterialBase::Mesh);
	debugMesh->SetCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	debugMesh->Create();

	// SkeletalMeshマテリアルの作成
	auto skeletalMesh = CreateMaterialBase("Skeletal");
	skeletalMesh->SetVSFilepath(L"Assets/Shader/SkeletalMeshVS.cso");
	skeletalMesh->SetPSFilepath(L"Assets/Shader/SimplePS.cso");
	skeletalMesh->SetInputLayout(Vertex::SkeletalMesh::InputLayout, MaterialBase::SkeletalMesh);
	skeletalMesh->SetCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	skeletalMesh->SetCBV(1, D3D12_SHADER_VISIBILITY_VERTEX);
	skeletalMesh->SetSRV(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	skeletalMesh->SetStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	skeletalMesh->Create();

	auto debugGBuffer = CreateMaterialBase("DebugGBuffer", 3);
	debugGBuffer->SetVSFilepath(L"Assets/Shader/MeshGBufferVS.cso");
	debugGBuffer->SetPSFilepath(L"Assets/Shader/DebugGBufferPS.cso");
	debugGBuffer->SetInputLayout(Vertex::SkeletalMesh::InputLayout, MaterialBase::Mesh);
	debugGBuffer->SetCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	debugGBuffer->SetSRV(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	debugGBuffer->SetStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	debugGBuffer->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 0);		// Albedo
	debugGBuffer->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 1);		// Normal
	debugGBuffer->SetRTVFormat(DXGI_FORMAT_R32G32B32A32_FLOAT, 2);	// WorldPos
	debugGBuffer->Create();

	auto spriteGBuffer = CreateMaterialBase("SpriteGBuffer", 3);
	spriteGBuffer->SetVSFilepath(L"Assets/Shader/SpriteGBufferVS.cso");
	spriteGBuffer->SetPSFilepath(L"Assets/Shader/GBufferPS.cso");
	spriteGBuffer->SetInputLayout(Vertex::Sprite::InputLayout, MaterialBase::Sprite);
	spriteGBuffer->SetCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	spriteGBuffer->SetSRV(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	spriteGBuffer->SetStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	spriteGBuffer->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 0);		// Albedo
	spriteGBuffer->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 1);		// Normal
	spriteGBuffer->SetRTVFormat(DXGI_FORMAT_R32G32B32A32_FLOAT, 2);	// WorldPos
	spriteGBuffer->Create();

	auto meshGBuffer = CreateMaterialBase("MeshGBuffer", 3);
	meshGBuffer->SetVSFilepath(L"Assets/Shader/MeshGBufferVS.cso");
	meshGBuffer->SetPSFilepath(L"Assets/Shader/GBufferPS.cso");
	meshGBuffer->SetInputLayout(Vertex::SkeletalMesh::InputLayout, MaterialBase::Mesh);
	meshGBuffer->SetCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	meshGBuffer->SetSRV(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	meshGBuffer->SetStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	meshGBuffer->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 0);		// Albedo
	meshGBuffer->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 1);		// Normal
	meshGBuffer->SetRTVFormat(DXGI_FORMAT_R32G32B32A32_FLOAT, 2);	// WorldPos
	meshGBuffer->Create();

	auto skeletalGBuffer = CreateMaterialBase("SkeletalGBuffer", 3);
	skeletalGBuffer->SetVSFilepath(L"Assets/Shader/SkeletalMeshGBufferVS.cso");
	skeletalGBuffer->SetPSFilepath(L"Assets/Shader/GBufferPS.cso");
	skeletalGBuffer->SetInputLayout(Vertex::SkeletalMesh::InputLayout, MaterialBase::SkeletalMesh);
	skeletalGBuffer->SetCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	skeletalGBuffer->SetCBV(1, D3D12_SHADER_VISIBILITY_VERTEX);
	skeletalGBuffer->SetSRV(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	skeletalGBuffer->SetStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	skeletalGBuffer->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 0);		// Albedo
	skeletalGBuffer->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 1);		// Normal
	skeletalGBuffer->SetRTVFormat(DXGI_FORMAT_R32G32B32A32_FLOAT, 2);	// WorldPos
	skeletalGBuffer->Create();

	auto water = CreateMaterialBase("Water");
	water->SetVSFilepath(L"Assets/Shader/SpriteVS.cso");
	water->SetPSFilepath(L"Assets/Shader/WaterPS.cso");
	water->SetInputLayout(Vertex::Sprite::InputLayout, MaterialBase::Sprite);
	water->SetCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	water->SetCBV(1, D3D12_SHADER_VISIBILITY_PIXEL);
	water->SetSRV(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	water->SetStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	water->Create();

	auto ground = CreateMaterialBase("Ground");
	ground->SetVSFilepath(L"Assets/Shader/GroundVS.cso");
	ground->SetPSFilepath(L"Assets/Shader/GroundPS.cso");
	ground->SetInputLayout(Vertex::Sprite::InputLayout, MaterialBase::Sprite);
	ground->SetCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	ground->SetCBV(1, D3D12_SHADER_VISIBILITY_PIXEL);
	ground->SetSRV(0, 2, D3D12_SHADER_VISIBILITY_PIXEL);
	ground->SetStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	ground->SetStaticSampler(1, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_COMPARISON_FUNC_GREATER);
	ground->Create();
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
