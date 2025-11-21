/*+===================================================================
	File: SceneGame.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/21 17:30:12 初回作成
	（これ以降下に更新日時と更新内容を書く）
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

	void Init() override final;
};
