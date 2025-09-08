/*+===================================================================
	File: IndexBuffer.hpp
	Summary: インデックスバッファのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/26 11:04 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <d3d12.h>
#include "../ComPtr.h"
#include <cstdint>

/**
 * @brief IndexBufferクラス
 */
class IndexBuffer
{
public:
	/**
	 * コンストラクタ
	 */
	IndexBuffer(size_t size, const uint32_t* pInitData = nullptr);

	/**
	 * デストラクタ
	 */
	~IndexBuffer() = default;

	D3D12_INDEX_BUFFER_VIEW GetView() const { return m_View; }

private:
	IndexBuffer(const IndexBuffer&) = delete; // コピーコンストラクタを削除
	void operator=(const IndexBuffer&) = delete; // 代入演算子を削除

	ComPtr<ID3D12Resource> m_pBuffer{};	// インデックスバッファリソース
	D3D12_INDEX_BUFFER_VIEW m_View{};	// インデックスバッファビュー
};
