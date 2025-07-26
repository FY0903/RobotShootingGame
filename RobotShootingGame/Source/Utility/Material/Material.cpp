/*+===================================================================
	File: Material.cpp
	Summary: マテリアルのソースファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/26 16:36 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Material.hpp"

Material::~Material()
{
	Term();
}

bool Material::Init(ID3D12Device* pDevice, DescriptorPool* pPool, size_t bufferSize, size_t count)
{
	if (!pDevice || !pPool || bufferSize == 0 || count == 0) return false;

	Term();

	m_pDevice = pDevice;
	m_pDevice->AddRef();

	m_pPool = pPool;
	m_pPool->AddRef();
}

void Material::Term()
{
}

bool Material::SetTexture(size_t index, TEXTURE_USAGE usage, const char* path, DirectX::ResourceUploadBatch& batch)
{
	return false;
}

void* Material::GetBufferPtr(size_t index) const
{
}

D3D12_GPU_VIRTUAL_ADDRESS Material::GetBufferAddress(size_t index) const
{
	return D3D12_GPU_VIRTUAL_ADDRESS();
}

D3D12_GPU_DESCRIPTOR_HANDLE Material::GetTextureHandle(size_t index, TEXTURE_USAGE usage) const
{
	return D3D12_GPU_DESCRIPTOR_HANDLE();
}
