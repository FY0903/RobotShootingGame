/*+===================================================================
	File: RenderTarget.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/03 15:07:34 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "../DirectXTex/d3dx12.h"
#include "Utility/DescriptorHeap/DescriptorHeap.hpp"

/**
 * @brief RenderTargetクラス
 */
class RenderTarget
{
public:
	/**
	 * コンストラクタ
	 */
	RenderTarget() = default;

	/**
	 * デストラクタ
	 */
	~RenderTarget();

	HRESULT Create(UINT width, UINT height, DXGI_FORMAT format, float clearColor[4] = nullptr);

	UINT Width() const { return m_Width; }
	UINT Height() const { return m_Height; }

	ID3D12Resource* Resource() const { return m_pResource.Get(); }
	D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() const { return m_ViewDesc; }
	DescriptorHandle* GetDescriptorHandle() const { return m_pDescriptorHandle; }
	D3D12_CLEAR_VALUE GetClearValue() const { return m_ClearValue; }

private:
	UINT m_Width{};		// テクスチャの幅
	UINT m_Height{};	// テクスチャの高さ
	ComPtr<ID3D12Resource> m_pResource{};			// テクスチャリソース
	D3D12_SHADER_RESOURCE_VIEW_DESC m_ViewDesc{};	// シェーダーリソースビューの設定
	D3D12_CLEAR_VALUE m_ClearValue{};				// クリア値

	DescriptorHeap* m_pSnapshotDescriptorHeap{}; // ディスクリプタヒープ
	DescriptorHandle* m_pDescriptorHandle{}; // ディスクリプタハンドル
};
