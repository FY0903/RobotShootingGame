/*+===================================================================
	File: DepthStencil.hpp
	Summary: DepthStencilクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/11 13:37 初回作成
			26/01/15 18:20 コメント記載
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
	/**
	 * @brief コンストラクタ
	 */
	DepthStencil() = default;

	/**
	 * @brief デストラクタ
	 */
	~DepthStencil();

	/**
	 * @brief 指定した幅・高さとフォーマットでリソースを作成します。初期の深度およびステンシルのクリア値を指定できます。
	 * @param width 作成するリソースの幅（ピクセル）。
	 * @param height 作成するリソースの高さ（ピクセル）。
	 * @param format 使用するピクセルフォーマット（DXGI_FORMAT）。
	 * @param clearDepth 初期の深度クリア値。
	 * @param clearStencil 初期のステンシルクリア値。
	 * @return 操作の成否を示す HRESULT。
	 */
	HRESULT Create(UINT width, UINT height, DXGI_FORMAT format, float clearDepth = 1.0f, UINT8 clearStencil = 0);

	/**
	 * @brief オブジェクトの幅を返します。
	 * @return 幅を表す UINT 値を返します。
	 */
	UINT Width() const { return m_Width; }

	/**
	 * @brief オブジェクトの高さを返します。
	 * @return 高さを表す UINT 値を返します。
	 */
	UINT Height() const { return m_Height; }

	/**
	 * @brief テクスチャリソースへのポインタを返します。
	 * @return ID3D12Resource へのポインタを返します。
	 */
	ID3D12Resource* Resource() const { return m_pResource.Get(); }
	
	/**
	 * @brief スナップショット用のディスクリプタヒープを返します。
	 * @return DescriptorHeap へのポインタを返します。
	 */
	DescriptorHandle* GetDescriptorHandle() const { return m_pDescriptorHandle; }
	
	/**
	 * @brief クリア値を返します。
	 * @return D3D12_CLEAR_VALUE 構造体を返します。
	 */
	D3D12_CLEAR_VALUE GetClearValue() const { return m_ClearValue; }

private:
	UINT m_Width{};		// テクスチャの幅
	UINT m_Height{};	// テクスチャの高さ

	ComPtr<ID3D12Resource> m_pResource{};			// テクスチャリソース
	D3D12_CLEAR_VALUE m_ClearValue{};				// クリア値

	DescriptorHeap* m_pSnapshotDescriptorHeap{}; // ディスクリプタヒープ
	DescriptorHandle* m_pDescriptorHandle{}; // ディスクリプタハンドル
};
