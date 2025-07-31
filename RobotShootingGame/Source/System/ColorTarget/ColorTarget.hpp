/*+===================================================================
	File: ColorTarget.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/28 13:57 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <d3d12.h>
#include <dxgi1_6.h>
#include "../../Utility/ComPtr.h"
#include <cstdint>

// ==============================
//	前方宣言
// ==============================
class DescriptorHandle;
class DescriptorPool;

/**
 * @brief ColorTargetクラス
 */
class ColorTarget
{
public:
	/**
	 * コンストラクタ
	 */
	ColorTarget() = default;

	/**
	 * デストラクタ
	 */
	~ColorTarget();

	/**
	 * @brief レンダリングターゲットを初期化します。
	 * @param [pDevice] Direct3D 12 デバイスへのポインタ。
	 * @param [pPoolRTV] RTV（レンダリングターゲットビュー）ディスクリプタプールへのポインタ。
	 * @param [width] レンダリングターゲットの幅（ピクセル単位）。
	 * @param [height] レンダリングターゲットの高さ（ピクセル単位）。
	 * @param [format] レンダリングターゲットの DXGI フォーマット。
	 * @param [useSRGB] sRGB カラースペースを使用するかどうか。
	 * @return 初期化が成功した場合は true、失敗した場合は false を返します。
	 */
	bool Init(ID3D12Device* pDevice, DescriptorPool* pPoolRTV, uint32_t width, uint32_t height, DXGI_FORMAT format, bool useSRGB);

	/**
	 * @brief バックバッファから初期化を行います。
	 * @param [pDevice] Direct3D 12 デバイスへのポインタ。
	 * @param [pPoolRTV] レンダーターゲットビューのディスクリプタプールへのポインタ。
	 * @param [useSRGB] sRGB カラースペースを使用するかどうかを指定するフラグ。
	 * @param [index] スワップチェーンのバックバッファのインデックス。
	 * @param [pSwapChain] DXGI スワップチェーンへのポインタ。
	 * @return 初期化が成功した場合は true、失敗した場合は false を返します。
	 */
	bool InitFromBackBuffer(ID3D12Device* pDevice, DescriptorPool* pPoolRTV, bool useSRGB, uint32_t index, IDXGISwapChain* pSwapChain);

	/**
	 * @brief 終了処理を実行します。
	 */
	void Term();

	/**
	 * @brief レンダーターゲットビューのハンドルを取得します。
	 * @return レンダーターゲットビューのハンドルへのポインタ。
	 */
	inline DescriptorHandle* GetHandleRTV() const { return m_pHandleRTV; }

	/**
	 * @brief m_pTarget から ID3D12Resource オブジェクトへのポインタを取得します。
	 * @return m_pTarget が保持する ID3D12Resource オブジェクトへのポインタ。
	 */
	inline ID3D12Resource* GetResource() const { return m_pTarget.Get(); }

	/**
	 * @brief リソースの説明を取得します。
	 * @return D3D12_RESOURCE_DESC 型のリソースの説明を返します。
	 */
	D3D12_RESOURCE_DESC GetDesc() const;

	/**
	 * @brief レンダーターゲットビューの記述情報を取得します。
	 * @return m_ViewDesc メンバーに格納されている D3D12_RENDER_TARGET_VIEW_DESC オブジェクトを返します。
	 */
	inline D3D12_RENDER_TARGET_VIEW_DESC GetViewDesc() const { return m_ViewDesc; }

private:
	ColorTarget(const ColorTarget&) = delete;	// コピーコンストラクタを禁止
	void operator = (const ColorTarget&) = delete;	// 代入演算子を禁止

	ComPtr<ID3D12Resource> m_pTarget{};	// リソースへのポインタ
	DescriptorHandle* m_pHandleRTV{};	// レンダーターゲットビューのハンドル
	DescriptorPool* m_pPoolRTV{};	// ディスクリプタプールへのポインタ
	D3D12_RENDER_TARGET_VIEW_DESC m_ViewDesc{};	// レンダーターゲットビューの説明
};
