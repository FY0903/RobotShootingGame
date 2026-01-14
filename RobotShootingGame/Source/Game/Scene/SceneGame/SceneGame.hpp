/*+===================================================================
	File: SceneGame.hpp
	Summary: ゲームシーンを表すクラスヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/21 17:30 初回作成
			26/01/14 19:13 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Game/Scene/SceneBase.hpp"

/**
 * @brief SceneGameクラス
 */
class SceneGame : public SceneBase
{
public:
	/**
	 * コンストラクタ
	 */
	SceneGame() = default;

	/**
	 * デストラクタ
	 */
	~SceneGame() = default;

private:
	/**
	 * @brief 初期化処理
	 */
	void OnInit() override final;

	/**
	 * @brief 更新処理
	 */
	void OnUpdate() override final;

	/**
	 * @brief 固定更新処理
	 */
	void OnFixedUpdate() override final;

	/**
	 * @brief 描画処理
	 */
	void OnDraw() override final;

	/**
	 * @brief 終了処理
	 */
	void OnUninit() override final;
};
