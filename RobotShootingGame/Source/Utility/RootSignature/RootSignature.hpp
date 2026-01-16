/*+===================================================================
	File: RootSignature.hpp
	Summary: RootSignatureクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/03 12:56 初回作成
			26/01/16 11:11 コメント記載
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
	RootSignature() = default;

	/**
	 * デストラクタ
	 */
	~RootSignature() = default;

	/**
	 * @brief ルートシグネチャの取得
	 * @return ルートシグネチャのポインタ
	 */
	ID3D12RootSignature* Get() const { return m_pRootSignature.Get(); }

	/**
	 * @brief ルートパラメータの追加
	 * @param shaderRegiser シェーダーレジスタ
	 * @param visibility シェーダーの可視性
	 */
	void AddRootParameter(UINT shaderRegiser, D3D12_SHADER_VISIBILITY visibility);

	/**
	 * @brief ディスクリプタレンジの追加
	 * @param shaderRegiser シェーダーレジスタ
	 * @param rangeType ディスクリプタレンジのタイプ
	 * @param numDescriptors ディスクリプタの数
	 * @param visibility シェーダーの可視性
	 */
	void AddDescriptorRange(UINT shaderRegiser, D3D12_DESCRIPTOR_RANGE_TYPE rangeType, UINT numDescriptors, D3D12_SHADER_VISIBILITY visibility);

	/**
	 * @brief スタティックサンプラーの追加
	 * @param shaderRegiser シェーダーレジスタ
	 * @param filter フィルターの種類
	 */
	void AddStaticSampler(UINT shaderRegiser, D3D12_FILTER filter);

	/**
	 * @brief ルートシグネチャの作成
	 */
	void Create();

private:
	ComPtr<ID3D12RootSignature> m_pRootSignature{};	// ルートシグネチャ

	std::vector<CD3DX12_ROOT_PARAMETER> m_RootParams{};				// ルートパラメータ
	std::vector<CD3DX12_DESCRIPTOR_RANGE> m_DescriptorRanges{};		// ディスクリプタレンジ
	std::vector<CD3DX12_STATIC_SAMPLER_DESC> m_StaticSamplers{};	// スタティックサンプラー
};
