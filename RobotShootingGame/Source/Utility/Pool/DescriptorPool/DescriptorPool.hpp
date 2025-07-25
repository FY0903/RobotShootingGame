/*+===================================================================
	File: DescriptorPool.hpp
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
	D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU{};	// CPUディスクリプタハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU{};	// GPUディスクリプタハンドル

	/**
	 * @brief CPUが存在するかどうかを判定します。
	 * @return CPUが存在する場合はtrue、存在しない場合はfalseを返します。
	 */
	inline bool HasCPU() const { return HandleCPU.ptr != 0; }

	/**
	 * @brief GPUが存在するかどうかを判定します。
	 * @return GPUが存在する場合はtrue、存在しない場合はfalseを返します。
	 */
	inline bool HasGPU() const { return HandleGPU.ptr != 0; }
};

/**
 * @brief DiscriptorPoolクラス
 */
class DescriptorPool
{
public:
	/**
	 * @brief ID3D12Device と記述子ヒープ記述を使用して DiscriptorPool を作成します。
	 * @param [pDevice] ID3D12Device へのポインタ。
	 * @param [desc] D3D12_DESCRIPTOR_HEAP_DESC 構造体へのポインタ。作成する記述子ヒープの情報を指定します。
	 * @param [ppPool] 作成された DiscriptorPool オブジェクトへのポインタを受け取るポインタ。
	 * @return 作成に成功した場合は true、失敗した場合は false を返します。
	 */
	static bool Create(ID3D12Device* pDevice, const D3D12_DESCRIPTOR_HEAP_DESC* desc, DescriptorPool** ppPool);

	/**
	 * @brief 参照カウントを1増やします。
	 */
	void AddRef();

	/**
	 * @brief リソースを解放します。
	 */
	void Release();	

	/**
	 * @brief 現在の参照カウントを取得します。
	 * @return 現在の参照カウント値（uint32_t 型）。
	 */
	uint32_t GetCount() const { return m_RefCount; }

	/**
	 * @brief ハンドルを割り当てて返します。
	 * @return 新しく割り当てられた DescriptorHandle へのポインタ。
	 */
	DescriptorHandle* AllocHandle();

	/**
	 * @brief ハンドルを解放します。
	 * @param pHandle 解放するDescriptorHandleへのポインタ。
	 */
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
	DescriptorPool() = default;

	/**
	 * デストラクタ
	 */
	~DescriptorPool();

	DescriptorPool(const DescriptorPool&) = delete;	// コピーコンストラクタを削除
	void operator=(const DescriptorPool&) = delete;	// コピー代入演算子を削除

	std::atomic<uint32_t> m_RefCount = 1;	// 参照カウント
	Pool<DescriptorHandle> m_Pool{};	// ディスクリプタハンドルのプール
	ComPtr<ID3D12DescriptorHeap> m_pHeap{};	// ディスクリプタヒープ
	uint32_t m_DescriptorSize{};	// ディスクリプタサイズ
};
