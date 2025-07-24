/*+===================================================================
	File: Texture.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/24 16:30 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <d3d12.h>
#include "../../System/App.hpp"
#include <ResourceUploadBatch.h>

// ==============================
//	前方宣言
// ==============================
class DiscriptorHandle;
class DiscriptorPool;

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

	bool Init(ID3D12Device* pDevice, DiscriptorPool* pPool, const wchar_t* filename, DirectX::ResourceUploadBatch& uploadBatch);

	bool Init(ID3D12Device* pDevice, DiscriptorPool* pPool, const D3D12_RESOURCE_DESC* pDesc, bool isCube);

	void Term();

	D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU() const;

	D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU() const;

private:
	D3D12_SHADER_RESOURCE_VIEW_DESC GetViewDesc(bool isCube);

	Texture(const Texture&) = delete; // コピーコンストラクタを削除
	Texture& operator=(const Texture&) = delete; // コピー代入演算子を削除

	ComPtr<ID3D12Resource> m_pTex;	// テクスチャリソース
	DescriptorHandle* m_pHandle;	// ディスクリプタハンドル
	DiscriptorPool* m_pPool;		// ディスクリプタプール
};
