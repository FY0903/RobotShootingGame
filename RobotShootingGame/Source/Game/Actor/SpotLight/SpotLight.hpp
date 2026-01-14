/*+===================================================================
	File: SpotLight.hpp
	Summary: スポットライトを表すクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/27 18:49 初回作成
			26/01/14 18:40 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Game/Actor/Actor.hpp"

/**
 * @brief SpotLightクラス
 */
class SpotLight : public Actor
{
public:
	/**
	 * コンストラクタ
	 */
	SpotLight() = default;

	/**
	 * デストラクタ
	 */
	~SpotLight() = default;

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
	 * @brief 終了処理
	 */
	void OnUninit() override final;
};
