/*+===================================================================
	File: Title.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/18 0:23:52 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Game/Actor/Actor.hpp"
#include "Utility/Texture/Texture.hpp"
#include "Utility/RenderTarget/RenderTarget.hpp"
#include "Utility/ConstantBuffer/ConstantBuffer.hpp"

/**
 * @brief Spriteクラス
 */
class Title : public Actor
{
public:
	/**
	 * コンストラクタ
	 */
	Title() = default;

	/**
	 * デストラクタ
	 */
	~Title() = default;

private:
	void OnInit() override final;
	void OnUpdate() override final;
	void OnUninit() override final;
};
