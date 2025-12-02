/*+===================================================================
	File: RootSignature.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/03 12:56:59 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "../DirectXTex/d3dx12.h"

// ==============================
//	前方宣言
// ==============================
struct ID3D12RootSignature;

/**
 * @brief RootSignatureクラス
 */
class RootSignature
{
public:
	/**
	 * コンストラクタ
	 */
	RootSignature();

	/**
	 * デストラクタ
	 */
	~RootSignature() = default;

	ID3D12RootSignature* Get() const { return m_pRootSignature.Get(); }

	void AddRootParameter(UINT shaderRegiser, D3D12_SHADER_VISIBILITY visibility);

	void AddDescriptorRange(UINT shaderRegiser, D3D12_DESCRIPTOR_RANGE_TYPE rangeType, UINT numDescriptors, D3D12_SHADER_VISIBILITY visibility);

	void AddStaticSampler(UINT shaderRegiser, D3D12_FILTER filter);

	void Create();

private:
	ComPtr<ID3D12RootSignature> m_pRootSignature{};	// ルートシグネチャ

	std::vector<CD3DX12_ROOT_PARAMETER> m_RootParams{}; // ルートパラメータ
	std::vector<CD3DX12_DESCRIPTOR_RANGE> m_DescriptorRanges{}; // ディスクリプタレンジ
	std::vector<CD3DX12_STATIC_SAMPLER_DESC> m_StaticSamplers{}; // スタティックサンプラー
};
