/*+===================================================================
	File: DirectionalLight.hpp
	Summary: 太陽光源を表すクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/18 15:43 初回作成
			26/01/14 17:54 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Game/Actor/Actor.hpp"

/**
 * @brief DirectionalLightクラス
 */
class DirectionalLight : public Actor
{
public:
	/**
	 * コンストラクタ
	 */
	DirectionalLight() = default;

	/**
	 * デストラクタ
	 */
	~DirectionalLight() = default;

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
