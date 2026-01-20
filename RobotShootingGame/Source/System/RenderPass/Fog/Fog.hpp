/*+===================================================================
	File: Fog.hpp
	Summary: Fogクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2026/01/17 17:52 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/RenderPass/RenderPass.hpp"

/**
 * @brief Fogクラス
 */
class Fog : public RenderPass
{
public:
	using RenderPass::RenderPass;

	/**
	 * コンストラクタ
	 */
	Fog() = default;

	/**
	 * デストラクタ
	 */
	~Fog();

	/**
	 * @brief 初期化処理
	 */
	void Init() override final;

private:
	/**
	 * @brief 定数バッファ更新処理
	 */
	void UpdateCB() override final;

	/**
	 * @brief ルートシグネチャ生成
	 */
	void CreateRootSignature() override final;
	
	/**
	 * @brief パイプラインステート生成
	 */
	void CreatePSO() override final;

	ConstantBuffer* m_pCameraCB[FRAME_BUFFER_COUNT]{};	// カメラ用定数バッファ
};
