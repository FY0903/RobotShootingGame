/*+===================================================================
	File: Skybox.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/18 1:22:25 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Game/Actor/Actor.hpp"

/**
 * @brief Objectクラス
 */
class Skybox : public Actor
{
public:
	/**
	 * コンストラクタ
	 */
	Skybox() = default;

	/**
	 * デストラクタ
	 */
	~Skybox() = default;

private:
	void OnInit() override final;
	void OnUpdate() override final;
	void OnUninit() override final;
};
