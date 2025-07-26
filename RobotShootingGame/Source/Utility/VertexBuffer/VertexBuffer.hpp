/*+===================================================================
	File: VertexBuffer.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/26 12:23 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <d3d12.h>
#include "../../System/App.hpp"

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

	bool Init(ID3D12Device* pDevice, size_t size, size_t stride, const void* pInitData = nullptr);

	template<typename T>
	inline bool Init(ID3D12Device* pDevice, size_t size, const T* pInitData = nullptr) { return Init(pDevice, size, sizeof(T), pInitData); }

	void Term();

	void* Map();

	void Unmap() { m_pVB->Unmap(0, nullptr); }

	template<typename T>
	inline T* Map() const { return static_cast<T*>(Map()); }

	D3D12_VERTEX_BUFFER_VIEW GetView() const { return m_view; }

private:
	VertexBuffer(const VertexBuffer&) = delete;	// コピーコンストラクタを禁止
	void operator=(const VertexBuffer&) = delete;	 // コピー代入演算子を禁止

	ComPtr<ID3D12Resource> m_pVB; // VertexBufferのリソース
	D3D12_VERTEX_BUFFER_VIEW m_view; // VertexBufferビュー
};
