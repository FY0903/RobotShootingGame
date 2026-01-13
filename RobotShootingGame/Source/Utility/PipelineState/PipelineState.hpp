/*+===================================================================
	File: PipelineState.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/03 13:39:26 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "../DirectXTex/d3dx12.h"

/**
 * @brief PipelineStateクラス
 */
class PipelineState
{
public:
	/**
	 * コンストラクタ
	 */
	PipelineState(size_t renderTargetNum = 1, bool alphaBlend = false);

	/**
	 * デストラクタ
	 */
	~PipelineState() = default;

	void SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout);
	void SetRootSignature(ID3D12RootSignature* pRootSignature);
	void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);
	void SetVS(std::wstring filepath);
	void SetPS(std::wstring filepath);
	void SetRTVFormat(DXGI_FORMAT format, size_t index);
	void SetDSVFormat(DXGI_FORMAT format);
	void Create();

	ID3D12PipelineState* Get() const { return m_pSnapshotPSO.Get(); }

private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_Desc{};	// グラフィックスパイプラインステートの記述
	ComPtr<ID3D12PipelineState> m_pSnapshotPSO{};	// パイプラインステートオブジェクト
	ComPtr<ID3DBlob> m_pVSBlob{};					// 頂点シェーダーのバイナリ
	ComPtr<ID3DBlob> m_pPSBlob{};					// ピクセルシェーダーのバイナリ
};
