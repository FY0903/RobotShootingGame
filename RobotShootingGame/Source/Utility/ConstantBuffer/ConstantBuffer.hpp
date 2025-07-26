/*+===================================================================
	File: ConstantBuffer.hpp
	Summary: 定数バッファのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/25 9:52 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <d3d12.h>
#include "../../System/App.hpp"
#include <vector>

// ==============================
//	前方宣言
// ==============================
class DescriptorHandle;
class DescriptorPool;

/**
 * @brief ConstantBufferクラス
 */
class ConstantBuffer
{
public:
	/**
	 * コンストラクタ
	 */
	ConstantBuffer() = default;

	/**
	 * デストラクタ
	 */
	~ConstantBuffer();

	/**
	 * @brief ID3D12Device と DescriptorPool を使用して初期化を行います。
	 * @param [pDevice] 初期化に使用する ID3D12Device へのポインタ。
	 * @param [pPool] 初期化に使用する DescriptorPool へのポインタ。
	 * @param [size] 初期化時に使用するサイズ。
	 * @return 初期化に成功した場合は true、失敗した場合は false を返します。
	 */
	bool Init(ID3D12Device* pDevice, DescriptorPool* pPool, size_t size);

	/**
	 * @brief 終了処理を実行します。
	 */
	void Term();

	/**
	 * @brief バッファの仮想アドレスを取得します。
	 * @return バッファのGPU仮想アドレス（D3D12_GPU_VIRTUAL_ADDRESS型）。
	 */
	inline D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const { return m_Desc.BufferLocation; }

	/**
	 * @brief CPU ディスクリプタ ハンドルを取得します。
	 * @return D3D12_CPU_DESCRIPTOR_HANDLE 型の CPU ディスクリプタ ハンドルを返します。
	 */
	D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU() const;

	/**
	 * @brief GPU ディスクリプタハンドルを取得します。
	 * @return GPU 用の D3D12_GPU_DESCRIPTOR_HANDLE を返します。
	 */
	D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU() const;

	/**
	 * @brief m_pMappedPtr メンバーの値を返します。
	 * @return m_pMappedPtr の値へのポインタ。
	 */
	inline void* GetPtr() const { return m_pMappedPtr; }

	/**
	 * @brief ポインタを取得し、指定された型にキャストします。
	 * @tparam [T] 取得するポインタの型。
	 * @return 指定された型 T* へのポインタ。
	 */
	template<typename T>
	inline T* GetPtr() { return reinterpret_cast<T*>(GetPtr()); }

private:
	ConstantBuffer(const ConstantBuffer&) = delete; // コピーコンストラクタを禁止
	void operator=(const ConstantBuffer&) = delete; // コピー代入演算子を禁止

	ComPtr<ID3D12Resource> m_pCB{};	// 定数バッファリソース
	DescriptorHandle* m_pHandle{};	// ディスクリプタハンドル
	DescriptorPool* m_pPool{};	// ディスクリプタプール
	D3D12_CONSTANT_BUFFER_VIEW_DESC m_Desc{};	// 定数バッファビューの記述
	void* m_pMappedPtr{};	// マッピングされたデータへのポインタ
};
