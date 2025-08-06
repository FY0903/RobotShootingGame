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
	/**
	 * コンストラクタ
	 */
	VertexBuffer() = default;

	/**
	 * デストラクタ
	 */
	~VertexBuffer();

	/**
	 * @brief ID3D12Device を使用してリソースを初期化します。
	 * @param [pDevice] 初期化に使用する ID3D12Device へのポインタ。
	 * @param [size] リソースの合計サイズ（バイト単位）。
	 * @param [stride] 各要素のサイズ（バイト単位）。
	 * @param [pInitData] 初期データへのポインタ。nullptr の場合は初期データなし。
	 * @return 初期化に成功した場合は true、失敗した場合は false を返します。
	 */
	bool Init(ID3D12Device* pDevice, size_t size, size_t stride, const void* pInitData = nullptr);

	/**
	 * @brief ID3D12Device を使用して、指定された型のデータで初期化を行います。
	 * @tparam [T] 初期化するデータの型。
	 * @param [pDevice] 初期化に使用する ID3D12Device へのポインタ。
	 * @param [count] 初期化する要素数。
	 * @param [pInitData] 初期化データへのポインタ。nullptr の場合はデフォルト値が使用されます。
	 * @return 初期化が成功した場合は true、失敗した場合は false を返します。
	 */
	template<typename T>
	inline bool Init(ID3D12Device* pDevice, size_t count, const T* pInitData = nullptr) { return Init(pDevice, sizeof(T) * count, sizeof(T), pInitData); }

	/**
	 * @brief 終了処理を実行します。
	 */
	void Term();

	/**
	 * @brief メモリ領域をマッピングします。
	 * @return マッピングされたメモリ領域へのポインタ。失敗した場合はnullptrを返すことがあります。
	 */
	void* Map() const;

	/**
	 * @brief バッファのマッピングを解除します。
	 */
	void Unmap() { m_pVB->Unmap(0, nullptr); }

	/**
	 * @brief 内部の Map 関数の結果を指定した型 T* にキャストして返します。
	 * @tparam [T] キャスト先のポインタ型。
	 * @return 内部の Map 関数の戻り値を T* 型にキャストしたもの。
	 */
	template<typename T>
	inline T* Map() const { return reinterpret_cast<T*>(Map()); }

	/**
	 * @brief 頂点バッファビューを取得します。
	 * @return このオブジェクトに関連付けられた D3D12_VERTEX_BUFFER_VIEW 構造体。
	 */
	inline D3D12_VERTEX_BUFFER_VIEW GetView() const { return m_view; }

private:
	VertexBuffer(const VertexBuffer&) = delete;	// コピーコンストラクタを禁止
	void operator=(const VertexBuffer&) = delete;	 // コピー代入演算子を禁止

	ComPtr<ID3D12Resource> m_pVB{}; // VertexBufferのリソース
	D3D12_VERTEX_BUFFER_VIEW m_view{}; // VertexBufferビュー
};
