/*+===================================================================
	File: ConstantBuffer.hpp
	Summary: 定数バッファのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/25 09:52 初回作成
			26/01/15 18:16 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <d3d12.h>

/**
 * @brief ConstantBufferクラス
 */
class ConstantBuffer
{
public:
	/**
	 * コンストラクタ
	 */
	ConstantBuffer(size_t size);

	/**
	 * デストラクタ
	 */
	~ConstantBuffer() = default;

	/**
	 * @brief バッファのGPU仮想アドレスを取得します。
	 * @return バッファのGPU仮想アドレス (D3D12_GPU_VIRTUAL_ADDRESS)。
	 */
	D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const { return m_Desc.BufferLocation; }

	/**
	 * @brief 定数バッファビューの記述を取得します。
	 * @return 定数バッファビューの記述 (D3D12_CONSTANT_BUFFER_VIEW_DESC)。
	 */
	D3D12_CONSTANT_BUFFER_VIEW_DESC GetDesc() const { return m_Desc; }

	/**
	 * @brief マッピングされたデータへのポインタを取得します。
	 * @return マッピングされたデータへのポインタ (void*)。
	 */
	void* GetPtr() const { return m_pMappedPtr; }

	/**
	 * @brief 指定された型 T のポインタを取得します。
	 * @tparam T 取得するポインタの型。
	 * @return 指定された型 T のポインタ (T*)。
	 */
	template <typename T>
	T* GetPtr() const
	{
		return static_cast<T*>(GetPtr());
	}

private:
	ConstantBuffer(const ConstantBuffer&) = delete; // コピーコンストラクタを禁止
	void operator=(const ConstantBuffer&) = delete; // コピー代入演算子を禁止

	ComPtr<ID3D12Resource> m_pBuffer{};			// 定数バッファリソース
	D3D12_CONSTANT_BUFFER_VIEW_DESC m_Desc{};	// 定数バッファビューの記述
	void* m_pMappedPtr{};						// マッピングされたデータへのポインタ
};
