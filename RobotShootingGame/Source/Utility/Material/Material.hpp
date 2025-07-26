/*+===================================================================
	File: Material.hpp
	Summary: マテリアルのヘッダファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/26 16:36 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "../Pool/DescriptorPool/DescriptorPool.hpp"
#include <ResourceUploadBatch.h>
#include "../Texture/Texture.hpp"
#include "../ConstantBuffer/ConstantBuffer.hpp"
#include <map>

/**
 * @brief Materialクラス
 */
class Material
{
public:
	enum TEXTURE_USAGE
	{
		TEXTURE_USAGE_DIFFUSE,	// ディフューズマップ
		TEXTURE_USAGE_SPECULAR,	// スペキュラマップ
		TEXTURE_USAGE_SHININESS,	// シニアスマップ
		TEXTURE_USAGE_NORMAL,	// ノーマルマップ

		TEXTURE_USAGE_COUNT	// テクスチャの数
	};

	/**
	 * コンストラクタ
	 */
	Material() = default;

	/**
	 * デストラクタ
	 */
	~Material();

	bool Init(ID3D12Device* pDevice, DescriptorPool* pPool, size_t bufferSize, size_t count);

	void Term();

	bool SetTexture(size_t index, TEXTURE_USAGE usage, const char* path, DirectX::ResourceUploadBatch& batch);

	void* GetBufferPtr(size_t index) const;
	
	template<typename T>
	T* GetBufferPtr(size_t index) const { return static_cast<T*>(GetBufferPtr(index)); }

	D3D12_GPU_VIRTUAL_ADDRESS GetBufferAddress(size_t index) const;

	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(size_t index, TEXTURE_USAGE usage) const;

	inline size_t GetCount() const { return m_Subset.size(); }

private:
	struct Subset
	{
		ConstantBuffer* pConstantBuffer;	// 定数バッファ
		D3D12_GPU_DESCRIPTOR_HANDLE textureHandles[TEXTURE_USAGE_COUNT];	// テクスチャのハンドル
	};

	Material(const Material&) = delete;	// コピーコンストラクタを削除
	void operator=(const Material&) = delete;	// コピー代入演算子を削除

	std::map<const char*, Texture*> m_pTexture{};	// テクスチャのマップ
	std::vector<Subset> m_Subset{};	// サブセットのベクター
	ID3D12Device* m_pDevice{};	// デバイスへのポインタ
	DescriptorPool* m_pPool{};	// ディスクリプタプールへのポインタ(CBV_UAV_SRV)
};

constexpr auto TU_DIFFUSE = Material::TEXTURE_USAGE_DIFFUSE;	// ディフューズマップ
constexpr auto TU_SPECULAR = Material::TEXTURE_USAGE_SPECULAR;	// スペキュラマップ
constexpr auto TU_SHININESS = Material::TEXTURE_USAGE_SHININESS;	// シニアスマップ
constexpr auto TU_NORMAL = Material::TEXTURE_USAGE_NORMAL;	// ノーマルマップ
