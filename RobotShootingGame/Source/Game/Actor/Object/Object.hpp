/*+===================================================================
	File: Object.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/24 13:58:08 初回作成
	（これ以降下に更新日時と更新内容を書く）
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
	void OnInit() override final;
	void OnUpdate() override final;
	void OnUninit() override final;

	Model* m_pModel{}; // モデルデータ
	Animation* m_pAnimation{}; // アニメーションデータ
};
