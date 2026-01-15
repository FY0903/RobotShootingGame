/*+===================================================================
	File: IndexBuffer.hpp
	Summary: インデックスバッファのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/26 11:04 初回作成
			26/01/15 18:31 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <d3d12.h>

/**
 * @brief IndexBufferクラス
 */
class IndexBuffer
{
public:
	/**
	 * @brief 指定したサイズのインデックスバッファを生成し、必要に応じて初期データで初期化するコンストラクタ。
	 * @param size 作成するインデックスバッファのサイズ（通常は要素数）。
	 * @param pInitData インデックスの初期データへのポインタ。nullptr の場合は初期データを使用せずに作成される。
	 */
	IndexBuffer(size_t size, const uint32_t* pInitData = nullptr);

	/**
	 * デストラクタ
	 */
	~IndexBuffer() = default;

	/**
	 * @brief インデックスバッファビューを取得します。
	 * @return インデックスバッファビュー（D3D12_INDEX_BUFFER_VIEW 型）。
	 */
	D3D12_INDEX_BUFFER_VIEW GetView() const { return m_View; }

private:
	IndexBuffer(const IndexBuffer&) = delete;		// コピーコンストラクタを削除
	void operator=(const IndexBuffer&) = delete;	// 代入演算子を削除

	ComPtr<ID3D12Resource> m_pBuffer{};	// インデックスバッファリソース
	D3D12_INDEX_BUFFER_VIEW m_View{};	// インデックスバッファビュー
};
