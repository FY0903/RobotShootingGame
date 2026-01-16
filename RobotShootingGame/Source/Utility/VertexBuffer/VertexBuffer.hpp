/*+===================================================================
	File: VertexBuffer.hpp
	Summary: VertexBufferのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/26 12:23 初回作成
            26/01/16 11:48 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <d3d12.h>

/**
 * @brief VertexBufferクラス
 */
class VertexBuffer
{
public:
	/**
	 * @brief 頂点バッファを初期化するコンストラクタ。
	 * @param size バッファ全体のサイズ（バイト単位）。
	 * @param stride 各頂点のサイズ（バイト単位、頂点ストライド）。
	 * @param pInitData 初期データへのポインタ。nullptrの場合はデータを設定せずにバッファを作成する。
	 */
	VertexBuffer(size_t size, size_t stride, const void* pInitData);

	/**
	 * デストラクタ
	 */
	~VertexBuffer() = default;

	/**
	 * @brief 頂点バッファビューを取得します。
	 * @return 頂点バッファビューの参照。
	 */
	D3D12_VERTEX_BUFFER_VIEW GetView() const { return m_View; }

private:
	VertexBuffer(const VertexBuffer&) = delete;		// コピーコンストラクタを禁止
	void operator=(const VertexBuffer&) = delete;	// コピー代入演算子を禁止

	ComPtr<ID3D12Resource> m_pBuffer{};	// VertexBufferのリソース
	D3D12_VERTEX_BUFFER_VIEW m_View{};	// VertexBufferビュー
};
