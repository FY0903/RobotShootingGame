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
	 * @brief 指定されたデバイス、RTVディスクリプタプール、幅、高さ、フォーマットで初期化を行います。
	 * @param [pDevice] 初期化に使用するID3D12Deviceへのポインタ。
	 * @param [pPoolRTV] RTVディスクリプタプールへのポインタ。
	 * @param [width] 初期化するリソースの幅（ピクセル単位）。
	 * @param [height] 初期化するリソースの高さ（ピクセル単位）。
	 * @param [format] リソースのDXGIフォーマット。
	 * @return 初期化が成功した場合はtrue、失敗した場合はfalseを返します。
	 */
	bool Init(ID3D12Device* pDevice, DescriptorPool* pPoolRTV, uint32_t width, uint32_t height, DXGI_FORMAT format);

	/**
	 * @brief バックバッファから初期化を行います。
	 * @param [pDevice] Direct3D 12 デバイスへのポインタ。
	 * @param [pPoolRTV] レンダーターゲットビューのディスクリプタプールへのポインタ。
	 * @param [index] スワップチェーンのバックバッファのインデックス。
	 * @param [pSwapChain] DXGI スワップチェーンへのポインタ。
	 * @return 初期化が成功した場合は true、失敗した場合は false を返します。
	 */
	bool InitFromBackBuffer(ID3D12Device* pDevice, DescriptorPool* pPoolRTV, uint32_t index, IDXGISwapChain* pSwapChain);

	/**
	 * @brief 終了処理を実行します。
	 */
	void Term();

	/**
	 * @brief RTVハンドルを取得します。
	 * @return RTVハンドルへのポインタ（DescriptorHandle*型）を返します。
	 */
	inline DescriptorHandle* GetHandleRTV() const { return m_pHandleRTV; }

	/**
	 * @brief m_pTargetからID3D12Resourceポインタを取得します。
	 * @return m_pTargetに格納されているID3D12Resource型のポインタ。
	 */
	inline ID3D12Resource* GetResource() const { return m_pTarget.Get(); }

	/**
	 * @brief リソースの記述情報を取得します。
	 * @return リソースの詳細情報を含む D3D12_RESOURCE_DESC 構造体。
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
