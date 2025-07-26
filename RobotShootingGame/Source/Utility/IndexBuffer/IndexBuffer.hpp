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
#include "../../System/App.hpp"
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
	IndexBuffer() = default;

	/**
	 * デストラクタ
	 */
	~IndexBuffer();

	/**
	 * @brief ID3D12Device を使用して初期化を行います。
	 * @param [pDevice] 初期化に使用する ID3D12Device へのポインタ。
	 * @param [size] 初期化するデータのサイズ（バイト単位）。
	 * @param [pInitData] 初期データへのポインタ。nullptr の場合はデータなしで初期化されます。
	 * @return 初期化に成功した場合は true、失敗した場合は false を返します。
	 */
	bool Init(ID3D12Device* pDevice, size_t size, const uint32_t* pInitData = nullptr);

	/**
	 * @brief 終了処理を実行します。
	 */
	void Term();

	/**
	 * @brief メモリ領域をマッピングし、その先頭アドレスを返します。
	 * @return マッピングされたメモリ領域の先頭を指すuint32_t型ポインタ。
	 */
	uint32_t* Map();

	/**
	 * @brief m_pIB オブジェクトのアンマップ操作を実行します。
	 */
	inline void Unmap() { m_pIB->Unmap(0, nullptr); }

	/**
	 * @brief インデックスバッファビューを取得します。
	 * @return このオブジェクトに関連付けられた D3D12_INDEX_BUFFER_VIEW を返します。
	 */
	inline D3D12_INDEX_BUFFER_VIEW GetView() const { return m_view; }

private:
	IndexBuffer(const IndexBuffer&) = delete; // コピーコンストラクタを削除
	void operator=(const IndexBuffer&) = delete; // 代入演算子を削除

	ComPtr<ID3D12Resource> m_pIB{};	// インデックスバッファリソース
	D3D12_INDEX_BUFFER_VIEW m_view{};	// インデックスバッファビュー
};
