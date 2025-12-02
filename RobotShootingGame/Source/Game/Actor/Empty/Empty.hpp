/*+===================================================================
	File: Empty.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/30 0:39:09 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Game/Actor/Actor.hpp"

/**
 * @brief Emptyクラス
 */
class Empty : public Actor
{
public:
	/**
	 * コンストラクタ
	 */
	Empty() = default;

	/**
	 * デストラクタ
	 */
	~Empty() = default;

private:
	void OnInit() override final;
	void OnUpdate() override final;
	void OnFixedUpdate() override final;
	void OnUninit() override final;
};
