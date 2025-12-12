/*+===================================================================
	File: DepthStencil.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/11 13:37:02 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "../DirectXTex/d3dx12.h"
#include "Utility/DescriptorHeap/DescriptorHeap.hpp"

/**
 * @brief DepthStencilクラス
 */
class DepthStencil
{
public:
	DepthStencil() = default;
	~DepthStencil();

	HRESULT Create(UINT width, UINT height, DXGI_FORMAT format, float clearDepth = 1.0f, UINT8 clearStencil = 0);

	UINT Width() const { return m_Width; }
	UINT Height() const { return m_Height; }

	ID3D12Resource* Resource() const { return m_pResource.Get(); }
	DescriptorHandle* GetDescriptorHandle() const { return m_pDescriptorHandle; }
	D3D12_CLEAR_VALUE GetClearValue() const { return m_ClearValue; }

private:
	UINT m_Width{};		// テクスチャの幅
	UINT m_Height{};	// テクスチャの高さ
	ComPtr<ID3D12Resource> m_pResource{};			// テクスチャリソース
	D3D12_CLEAR_VALUE m_ClearValue{};				// クリア値

	DescriptorHeap* m_pDescriptorHeap{}; // ディスクリプタヒープ
	DescriptorHandle* m_pDescriptorHandle{}; // ディスクリプタハンドル
};
