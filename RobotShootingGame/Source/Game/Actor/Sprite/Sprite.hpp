/*+===================================================================
	File: Sprite.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/24 8:33:45 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Game/Actor/Actor.hpp"
#include "Utility/Texture/Texture.hpp"

/**
 * @brief Spriteクラス
 */
class Sprite : public Actor
{
public:
	/**
	 * コンストラクタ
	 */
	Sprite() = default;

	/**
	 * デストラクタ
	 */
	~Sprite() = default;

private:
	void OnInit() override final;
	void OnUpdate() override final;
	void OnUninit() override final;
};
