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
#include "../ComPtr.h"
#include <vector>

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

	D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const { return m_Desc.BufferLocation; }
	D3D12_CONSTANT_BUFFER_VIEW_DESC GetDesc() const { return m_Desc; }

	void* GetPtr() const { return m_pMappedPtr; }

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
