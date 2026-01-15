/*+===================================================================
	File: PipelineState.hpp
	Summary: PipelineStateクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/03 13:39 初回作成
			26/01/15 19:09 コメント記載
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
	 * @brief PipelineState のコンストラクタ。レンダーターゲット数とアルファブレンド設定でオブジェクトを初期化します。
	 * @param renderTargetNum 使用するレンダーターゲットの数。
	 * @param alphaBlend アルファブレンドを有効にするかどうか。
	 */
	PipelineState(size_t renderTargetNum = 1, bool alphaBlend = false);

	/**
	 * デストラクタ
	 */
	~PipelineState() = default;

	/**
	 * @brief 入力レイアウトを設定します。
	 * @param layout 設定する入力レイアウトの記述。
	 */
	void SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout);
	
	/**
	 * @brief ルートシグネチャを設定します。
	 * @param pRootSignature 設定するルートシグネチャのポインタ。
	 */
	void SetRootSignature(ID3D12RootSignature* pRootSignature);
	
	/**
	 * @brief プリミティブトポロジータイプを設定します。
	 * @param type 設定するプリミティブトポロジータイプ。
	 */
	void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);
	
	/**
	 * @brief 頂点シェーダーを設定します。
	 * @param filepath 頂点シェーダーのファイルパス。
	 */
	void SetVS(std::wstring filepath);
	
	/**
	 * @brief ピクセルシェーダーを設定します。
	 * @param filepath ピクセルシェーダーのファイルパス。
	 */
	void SetPS(std::wstring filepath);
	
	/**
	 * @brief レンダーターゲットビューのフォーマットを設定します。
	 * @param format 設定するレンダーターゲットビューのフォーマット。
	 * @param index 設定するレンダーターゲットのインデックス。
	 */
	void SetRTVFormat(DXGI_FORMAT format, size_t index);
	
	/**
	 * @brief 深度ステンシルビューのフォーマットを設定します。
	 * @param format 設定する深度ステンシルビューのフォーマット。
	 */
	void SetDSVFormat(DXGI_FORMAT format);
	
	/**
	 * @brief パイプラインステートオブジェクトを作成します。
	 */
	void Create();

	/**
	 * @brief パイプラインステートオブジェクトを取得します。
	 * @return パイプラインステートオブジェクトのポインタ。
	 */
	ID3D12PipelineState* Get() const { return m_pPSO.Get(); }

private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_Desc{};	// グラフィックスパイプラインステートの記述
	ComPtr<ID3D12PipelineState> m_pPSO{};			// パイプラインステートオブジェクト
	ComPtr<ID3DBlob> m_pVSBlob{};					// 頂点シェーダーのバイナリ
	ComPtr<ID3DBlob> m_pPSBlob{};					// ピクセルシェーダーのバイナリ
};
