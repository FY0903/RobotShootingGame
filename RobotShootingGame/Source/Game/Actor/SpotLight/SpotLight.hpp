/*+===================================================================
	File: SpotLight.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/27 18:49:31 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Game/Actor/Actor.hpp"

/**
 * @brief SpotLightクラス
 */
class SpotLight : public Actor
{
public:
	/**
	 * コンストラクタ
	 */
	SpotLight() = default;

	/**
	 * デストラクタ
	 */
	~SpotLight() = default;

private:
	void OnInit() override final;
	void OnUpdate() override final;
	void OnUninit() override final;
};
