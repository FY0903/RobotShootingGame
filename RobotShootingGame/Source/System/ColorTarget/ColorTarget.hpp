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
class DescriptorHeap;

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

	bool Init(ID3D12Device* pDevice, DescriptorHeap* pPoolRTV, uint32_t width, uint32_t height, DXGI_FORMAT format);

	bool InitFromBackBuffer(ID3D12Device* pDevice, DescriptorHeap* pPoolRTV, uint32_t index, IDXGISwapChain* pSwapChain);

	void Term();

	inline DescriptorHandle* GetHandleRTV() const { return m_pHandleRTV; }

	inline ID3D12Resource* GetResource() const { return m_pTarget.Get(); }

	D3D12_RESOURCE_DESC GetDesc() const;

	inline D3D12_RENDER_TARGET_VIEW_DESC GetViewDesc() const { return m_ViewDesc; }

private:
	ColorTarget(const ColorTarget&) = delete;	// コピーコンストラクタを禁止
	void operator = (const ColorTarget&) = delete;	// 代入演算子を禁止

	ComPtr<ID3D12Resource> m_pTarget{};	// リソースへのポインタ
	DescriptorHandle* m_pHandleRTV{};	// レンダーターゲットビューのハンドル
	DescriptorHeap* m_pPoolRTV{};	// ディスクリプタプールへのポインタ
	D3D12_RENDER_TARGET_VIEW_DESC m_ViewDesc{};	// レンダーターゲットビューの説明
};
