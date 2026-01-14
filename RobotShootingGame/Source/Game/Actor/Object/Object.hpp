/*+===================================================================
	File: Object.hpp
	Summary: オブジェクトクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/24 13:58 初回作成
			26/01/14 18:36 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Game/Actor/Actor.hpp"
#include "Utility/Model/Model.hpp"
#include "Utility/Animation/Animation.hpp"

/**
 * @brief Objectクラス
 */
class Object : public Actor
{
public:
	/**
	 * コンストラクタ
	 */
	Object() = default;

	/**
	 * デストラクタ
	 */
	~Object() = default;

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

	std::vector<Animation*> m_pAnimations{}; // アニメーションデータ
};
