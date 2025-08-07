/*+===================================================================
	File: DepthTarget.hpp
	Summary: DSVのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/27 20:55 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <d3d12.h>
#include "../../Utility/ComPtr.h"
#include <cstdint>

// ==============================
//	前方宣言
// ==============================
class DescriptorHandle;
class DescriptorPool;

/**
 * @brief DepthTargetクラス
 */
class DepthTarget
{
public:
	/**
	 * コンストラクタ
	 */
	DepthTarget() = default;

	/**
	 * デストラクタ
	 */
	~DepthTarget();

	bool Init(ID3D12Device* pDevice, DescriptorPool* pPoolDSV, DescriptorPool* pPoolSRV, uint32_t width, uint32_t height, DXGI_FORMAT format, float clearDepth, uint8_t clearStencil);

	void Term();

	inline DescriptorHandle* GetHandleDSV() const { return m_pHandleDSV; }

	inline DescriptorHandle* GetHandleSRV() const { return m_pHandleSRV; }

	inline ID3D12Resource* GetResource() const { return m_pTarget.Get(); }

	D3D12_RESOURCE_DESC GetDesc() const;

	inline D3D12_DEPTH_STENCIL_VIEW_DESC GetDSVDesc() const { return m_DSVDesc; }

	inline D3D12_SHADER_RESOURCE_VIEW_DESC GetSRVDesc() const { return m_SRVDesc; }

	void ClearViews(ID3D12GraphicsCommandList* pCmdList) const;

private:
	DepthTarget(const DepthTarget&) = delete; // コピーコンストラクタを削除
	void operator=(const DepthTarget&) = delete; // コピー代入演算子を削除

	ComPtr<ID3D12Resource> m_pTarget{};	 // 深度ターゲットリソース
	DescriptorHandle* m_pHandleDSV{}; // 深度ステンシルビューのディスクリプタハンドル
	DescriptorHandle* m_pHandleSRV{}; // 深度ステンシルビューのSRVディスクリプタハンドル
	DescriptorPool* m_pPoolDSV{}; // ディスクリプタプール
	DescriptorPool* m_pPoolSRV{}; // SRVディスクリプタプール
	D3D12_DEPTH_STENCIL_VIEW_DESC m_DSVDesc{}; // 深度ステンシルビューの説明
	D3D12_SHADER_RESOURCE_VIEW_DESC m_SRVDesc{}; // シェーダーリソースビューの説明
	float m_ClearDepth{};	// クリア深度値
	uint8_t m_ClearStencil{}; // クリアステンシル値
};
