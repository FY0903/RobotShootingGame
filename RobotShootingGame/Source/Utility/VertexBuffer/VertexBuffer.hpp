/*+===================================================================
	File: VertexBuffer.hpp
	Summary: VertexBufferのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/26 12:23 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <d3d12.h>
#include "../ComPtr.h"

/**
 * @brief VertexBufferクラス
 */
class VertexBuffer
{
public:

	VertexBuffer(size_t size, size_t stride, const void* pInitData);

	/**
	 * デストラクタ
	 */
	~VertexBuffer() = default;

	D3D12_VERTEX_BUFFER_VIEW GetView() const { return m_View; }

private:
	VertexBuffer(const VertexBuffer&) = delete;	// コピーコンストラクタを禁止
	void operator=(const VertexBuffer&) = delete;	 // コピー代入演算子を禁止

	ComPtr<ID3D12Resource> m_pBuffer{};	// VertexBufferのリソース
	D3D12_VERTEX_BUFFER_VIEW m_View{};	// VertexBufferビュー
};
