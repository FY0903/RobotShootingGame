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

	/**
	 * @brief ID3D12Device と DescriptorPool を使用してバッファを初期化します。
	 * @param [pDevice] 初期化に使用する ID3D12Device へのポインタ。
	 * @param [pPool] ディスクリプタプールへのポインタ。
	 * @param [bufferSize] 各バッファのサイズ（バイト単位）。
	 * @param [count] 初期化するバッファの数。
	 * @return 初期化に成功した場合は true、失敗した場合は false を返します。
	 */
	bool Init(ID3D12Device* pDevice, DescriptorPool* pPool, size_t bufferSize, size_t count);

	/**
	 * @brief 終了処理を実行します。
	 */
	void Term();

	/**
	 * @brief 指定したインデックスにテクスチャを設定します。
	 * @param [index] テクスチャを設定するインデックス。
	 * @param [usage] テクスチャの使用方法を指定するTEXTURE_USAGE列挙体の値。
	 * @param [path] 読み込むテクスチャファイルのパス（ワイド文字列）。
	 * @param [batch] リソースのアップロードに使用するDirectX::ResourceUploadBatchオブジェクトへの参照。
	 * @return テクスチャの設定に成功した場合はtrue、失敗した場合はfalseを返します。
	 */
	bool SetTexture(size_t index, TEXTURE_USAGE usage, const wchar_t* path, DirectX::ResourceUploadBatch& batch);

	/**
	 * @brief 指定したインデックスのバッファポインタを取得します。
	 * @param [index] 取得するバッファのインデックス。
	 * @return バッファの先頭アドレスを指すポインタ。
	 */
	void* GetBufferPtr(size_t index) const;
	
	/**
	 * @brief 指定したインデックスのバッファポインタを取得します。
	 * @tparam [T] 返されるポインタの型。
	 * @param [index] 取得するバッファのインデックス。
	 * @return 指定した型Tのバッファポインタ。
	 */
	template<typename T>
	T* GetBufferPtr(size_t index) const { return static_cast<T*>(GetBufferPtr(index)); }

	/**
	 * @brief 指定したインデックスのバッファのGPU仮想アドレスを取得します。
	 * @param [index] 取得するバッファのインデックス。
	 * @return バッファのD3D12_GPU_VIRTUAL_ADDRESS（GPU仮想アドレス）。
	 */
	D3D12_GPU_VIRTUAL_ADDRESS GetBufferAddress(size_t index) const;

	/**
	 * @brief 指定されたインデックスと使用用途に対応するテクスチャのGPUディスクリプタハンドルを取得します。
	 * @param [index] 取得するテクスチャのインデックス。
	 * @param [usage] テクスチャの使用用途を示すTEXTURE_USAGE列挙体。
	 * @return 対応するテクスチャのD3D12_GPU_DESCRIPTOR_HANDLEを返します。
	 */
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(size_t index, TEXTURE_USAGE usage) const;

	/**
	 * @brief m_Subsetの要素数を取得します。
	 * @return m_Subsetに含まれる要素の数（size_t型）。
	 */
	inline size_t GetCount() const { return m_Subset.size(); }

private:
	struct Subset
	{
		ConstantBuffer* pConstantBuffer;	// 定数バッファ
		D3D12_GPU_DESCRIPTOR_HANDLE textureHandles[TEXTURE_USAGE_COUNT];	// テクスチャのハンドル
	};

	Material(const Material&) = delete;	// コピーコンストラクタを禁止
	void operator=(const Material&) = delete;	// コピー代入演算子を禁止

	std::map<const wchar_t*, Texture*> m_pTexture{};	// テクスチャのマップ
	std::vector<Subset> m_Subset{};	// サブセットのベクター
	ID3D12Device* m_pDevice{};	// デバイスへのポインタ
	DescriptorPool* m_pPool{};	// ディスクリプタプールへのポインタ(CBV_UAV_SRV)
};

constexpr auto TU_DIFFUSE = Material::TEXTURE_USAGE_DIFFUSE;	// ディフューズマップ
constexpr auto TU_SPECULAR = Material::TEXTURE_USAGE_SPECULAR;	// スペキュラマップ
constexpr auto TU_SHININESS = Material::TEXTURE_USAGE_SHININESS;	// シニアスマップ
constexpr auto TU_NORMAL = Material::TEXTURE_USAGE_NORMAL;	// ノーマルマップ
