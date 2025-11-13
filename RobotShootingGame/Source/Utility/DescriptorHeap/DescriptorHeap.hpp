/*+===================================================================
	File: DescriptorHeap.hpp
	Summary: ディスクリプタヒープのヘッダファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/24 14:22 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/ComPtr.h"
#include "../DirectXTex/d3dx12.h"
#include <vector>

// ==============================
//	前方宣言
// ==============================
class ConstantBuffer;
class Texture2D;
class Texture;

/**
 * @brief DescriptorHandleクラス
 */
class DescriptorHandle
{
public:
	D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU{};	// CPUディスクリプタハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU{};	// GPUディスクリプタハンドル
};

/**
 * @brief DescriptorHeapクラス
 */
class DescriptorHeap
{
public:
	/**
	 * コンストラクタ
	 */
	DescriptorHeap();

	/**
	 * デストラクタ
	 */
	~DescriptorHeap();

	ID3D12DescriptorHeap* GetHeap() const { return m_pHeap.Get(); }
	DescriptorHandle* Register(ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC desc);

private:
	UINT m_IncrementSize{};	// ディスクリプタサイズ
	ComPtr<ID3D12DescriptorHeap> m_pHeap{};	// ディスクリプタヒープ
	std::vector<DescriptorHandle*> m_pHandles{};	// ディスクリプタハンドル
};
