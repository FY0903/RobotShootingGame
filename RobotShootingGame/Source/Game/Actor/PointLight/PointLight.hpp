/*+===================================================================
	File: PointLight.hpp
	Summary: 点光源を表すクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/27 14:43 初回作成
			26/01/14 18:39 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Game/Actor/Actor.hpp"

/**
 * @brief PointLightクラス
 */
class PointLight : public Actor
{
public:
	/**
	 * コンストラクタ
	 */
	PointLight() = default;

	/**
	 * デストラクタ
	 */
	~PointLight() = default;

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
