/*+===================================================================
	File: RenderTarget.hpp
	Summary: RenderTargetクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/03 15:07 初回作成
			26/01/16 11:08 コメント記載
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

	/**
	 * @brief 幅、高さ、フォーマット、およびオプションのクリア色を指定してオブジェクトを作成します。
	 * @param width 作成するオブジェクトの幅（ピクセル単位）。
	 * @param height 作成するオブジェクトの高さ（ピクセル単位）。
	 * @param format ピクセルフォーマットを指定する DXGI_FORMAT の列挙値。
	 * @param clearColor RGBA を表す 4 要素の float 配列。デフォルトは nullptr で、クリア色が指定されていないことを示します。
	 * @return 操作の成否を示す HRESULT。成功時は通常 S_OK、失敗時はエラーコードを返します。
	 */
	HRESULT Create(UINT width, UINT height, DXGI_FORMAT format, float clearColor[4] = nullptr);

	/**
	 * @brief オブジェクトの幅を取得します。
	 * @return 幅を表すUINT値（m_Width）を返します。
	 */
	UINT Width() const { return m_Width; }

	/**
	 * @brief オブジェクトの高さを取得します。
	 * @return 高さを表すUINT値（m_Height）を返します。
	 */
	UINT Height() const { return m_Height; }

	/**
	 * @brief テクスチャリソースを取得します。
	 * @return テクスチャリソースへのポインタ（m_pResource）を返します。
	 */
	ID3D12Resource* Resource() const { return m_pResource.Get(); }

	/**
	 * @brief シェーダーリソースビューの設定を取得します。
	 * @return シェーダーリソースビューの設定（m_ViewDesc）を返します。
	 */
	D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() const { return m_ViewDesc; }

	/**
	 * @brief ディスクリプタハンドルを取得します。
	 * @return ディスクリプタハンドルへのポインタ（m_pDescriptorHandle）を返します。
	 */
	DescriptorHandle* GetDescriptorHandle() const { return m_pDescriptorHandle; }

	/**
	 * @brief クリア値を取得します。
	 * @return クリア値（m_ClearValue）を返します。
	 */
	D3D12_CLEAR_VALUE GetClearValue() const { return m_ClearValue; }

private:
	UINT m_Width{};		// テクスチャの幅
	UINT m_Height{};	// テクスチャの高さ

	ComPtr<ID3D12Resource> m_pResource{};			// テクスチャリソース
	D3D12_SHADER_RESOURCE_VIEW_DESC m_ViewDesc{};	// シェーダーリソースビューの設定
	D3D12_CLEAR_VALUE m_ClearValue{};				// クリア値

	DescriptorHeap* m_pSnapshotDescriptorHeap{};	// ディスクリプタヒープ
	DescriptorHandle* m_pDescriptorHandle{};		// ディスクリプタハンドル
};
