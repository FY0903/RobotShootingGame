/*+===================================================================
	File: Grid.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/22 16:46:33 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Game/Actor/Actor.hpp"

/**
 * @brief Gridクラス
 */
class Grid : public Actor
{
public:
	/**
	 * コンストラクタ
	 */
	Grid() = default;

	/**
	 * デストラクタ
	 */
	~Grid() = default;

private:
	void OnInit() override final;
	void OnUninit() override final;
};
