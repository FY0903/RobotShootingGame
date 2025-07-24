/*+===================================================================
	File: DiscriptorPool.hpp
	Summary: ディスクリプタプールのヘッダファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/24 14:22 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <d3d12.h>
#include <atomic>
#include "../../../System/App.hpp"
#include "../Pool.hpp"

/**
 * @brief DescriptorHandleクラス
 */
class DescriptorHandle
{
public:
	D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU;	// CPUディスクリプタハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU;	// GPUディスクリプタハンドル

	inline bool HasCPU() const { return HandleCPU.ptr != 0; }
	inline bool HasGPU() const { return HandleGPU.ptr != 0; }
};

/**
 * @brief DiscriptorPoolクラス
 */
class DiscriptorPool
{
public:
	static bool Create(ID3D12Device* pDevice, const D3D12_DESCRIPTOR_HEAP_DESC* desc, DiscriptorPool** ppPool);

	void AddRef();

	void Release();	

	uint32_t GetCount() const { return m_RefCount; }

	DescriptorHandle* AllocHandle();

	void FreeHandle(DescriptorHandle* pHandle);

	/**
	 * @brief 利用可能なハンドルの数を取得します。
	 * @return 利用可能なハンドルの数（uint32_t 型）。
	 */
	uint32_t GetAvailableHandleCount() const { return m_Pool.GetAvailableCount(); }

	/**
	 * @brief 現在割り当てられているハンドルの数を取得します。
	 * @return 割り当てられているハンドルの数（uint32_t 型）。
	 */
	uint32_t GetAllocatedHandleCount() const { return m_Pool.GetUsedCount(); }

	/**
	 * @brief ハンドルの数を取得します。
	 * @return 現在のハンドルの数（uint32_t 型）。
	 */
	uint32_t GetHandleCount() const { return m_Pool.GetSize(); }

	/**
	 * @brief ディスクリプタヒープへのポインタを取得します。
	 * @return ID3D12DescriptorHeap への定数ポインタ。
	 */
	ID3D12DescriptorHeap* const GetHeap() const { return m_pHeap.Get(); }

private:
	/**
	 * コンストラクタ
	 */
	DiscriptorPool() = default;

	/**
	 * デストラクタ
	 */
	~DiscriptorPool();

	DiscriptorPool(const DiscriptorPool&) = delete;	// コピーコンストラクタを削除
	void operator=(const DiscriptorPool&) = delete;	// コピー代入演算子を削除

	std::atomic<uint32_t> m_RefCount = 1;	// 参照カウント
	Pool<DescriptorHandle> m_Pool{};	// ディスクリプタハンドルのプール
	ComPtr<ID3D12DescriptorHeap> m_pHeap{};	// ディスクリプタヒープ
	uint32_t m_DescriptorSize{};	// ディスクリプタサイズ
};
