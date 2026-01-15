/*+===================================================================
	File: Sepia.hpp
	Summary: Sepiaレンダーパスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2026/01/13 16:41 初回作成
			   01/15 10:18 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/RenderPass/RenderPass.hpp"

/**
 * @brief Sepiaクラス
 */
class Sepia : public RenderPass
{
public:
	using RenderPass::RenderPass;

	/**
	 * コンストラクタ
	 */
	Sepia() = default;

	/**
	 * デストラクタ
	 */
	~Sepia() = default;

	/**
	 * @brief 初期化処理
	 */
	void Init() override final;

private:
	/**
	 * @brief ルートシグネチャ生成
	 */
	void CreateRootSignature() override final;

	/**
	 * @brief パイプラインステート生成
	 */
	void CreatePSO() override final;
};
