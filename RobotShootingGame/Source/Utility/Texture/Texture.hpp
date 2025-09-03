/*+===================================================================
	File: Texture.hpp
	Summary: テクスチャクラスのヘッダファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/24 16:30 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <d3d12.h>
#include "../ComPtr.h"
#include <ResourceUploadBatch.h>

// ==============================
//	前方宣言
// ==============================
class DescriptorHandle;
class DescriptorHeap;

/**
 * @brief Textureクラス
 */
class Texture
{
public:
	/**
	 * コンストラクタ
	 */
	Texture() = default;

	/**
	 * デストラクタ
	 */
	~Texture();

	/**
	 * @brief 指定されたデバイス、ディスクリプタプール、ファイル名、およびリソースアップロードバッチを使用して初期化を行います。
	 * @param [pDevice] 初期化に使用するID3D12Deviceへのポインタ。
	 * @param [pPool] ディスクリプタプールへのポインタ。
	 * @param [filename] 初期化に使用するファイル名（ワイド文字列）。
	 * @param [uploadBatch] DirectX::ResourceUploadBatchへの参照。リソースのアップロードに使用されます。
	 * @return 初期化が成功した場合はtrue、失敗した場合はfalseを返します。
	 */
	bool Init(ID3D12Device* pDevice, DescriptorHeap* pPool, const wchar_t* filename, DirectX::ResourceUploadBatch& uploadBatch);

	/**
	 * @brief ID3D12Device、DiscriptorPool、およびリソース記述を使用して初期化を行います。
	 * @param [pDevice] 初期化に使用するID3D12Deviceへのポインタ。
	 * @param [pPool] ディスクリプタプールへのポインタ。
	 * @param [pDesc] リソースの記述を示すD3D12_RESOURCE_DESC構造体へのポインタ。
	 * @param [isCube] リソースがキューブマップかどうかを示す真偽値。
	 * @return 初期化が成功した場合はtrue、失敗した場合はfalseを返します。
	 */
	bool Init(ID3D12Device* pDevice, DescriptorHeap* pPool, const D3D12_RESOURCE_DESC* pDesc, bool isCube);

	/**
	 * @brief 終了処理を実行します。
	 */
	void Term();

	/**
	 * @brief CPU ディスクリプタ ハンドルを取得します。
	 * @return D3D12_CPU_DESCRIPTOR_HANDLE 型の CPU ディスクリプタ ハンドルを返します。
	 */
	D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU() const;

	/**
	 * @brief GPU ディスクリプタ ハンドルを取得します。
	 * @return D3D12_GPU_DESCRIPTOR_HANDLE 型の GPU ディスクリプタ ハンドルを返します。
	 */
	D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU() const;

private:
	/**
	 * @brief キューブマップかどうかに応じた D3D12 シェーダーリソースビューの記述子を取得します。
	 * @param [isCube] キューブマップの場合は true、そうでない場合は false を指定します。
	 * @return 指定された条件に基づく D3D12_SHADER_RESOURCE_VIEW_DESC 構造体を返します。
	 */
	D3D12_SHADER_RESOURCE_VIEW_DESC GetViewDesc(bool isCube);

	Texture(const Texture&) = delete; // コピーコンストラクタを削除
	void operator=(const Texture&) = delete; // コピー代入演算子を削除

	ComPtr<ID3D12Resource> m_pTex{};	// テクスチャリソース
	DescriptorHandle* m_pHandle{};	// ディスクリプタハンドル
	DescriptorHeap* m_pPool{};		// ディスクリプタプール
};
