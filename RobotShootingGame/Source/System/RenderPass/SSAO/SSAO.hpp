/*+===================================================================
	File: SSAO.hpp
	Summary: スクリーンスペースアンビエントオクルージョン(SSAO)を管理するクラスヘッダ
	Author: AT13C192 23 藤原佑埜
	Date: 2026/01/16 20:41 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/RenderPass/RenderPass.hpp"
#include "Utility/ConstantBuffer/ConstantBuffer.hpp"

/**
 * @brief SSAOクラス
 */
class SSAO : public RenderPass
{
public:
	using RenderPass::RenderPass;

	/**
	 * コンストラクタ
	 */
	SSAO() = default;

	/**
	 * デストラクタ
	 */
	~SSAO() = default;

	/**
	 * @brief 初期化処理
	 */
	void Init() override final;

	/**
	 * @brief 深度レンダーターゲットのSRV設定
	 * @param rt 深度レンダーターゲット
	 */
	void SetDepthSRV(RenderTarget* rt);

private:
	/**
	 * @brief ルートシグネチャ生成
	 */
	void CreateRootSignature() override final;

	/**
	 * @brief パイプラインステート生成
	 */
	void CreatePSO() override final;

	ConstantBuffer* m_pCB[FRAME_BUFFER_COUNT]{};	// カーネル用定数バッファ
};
