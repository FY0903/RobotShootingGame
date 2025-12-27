/*+===================================================================
	File: PointLight.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/27 14:43:37 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Game/Actor/Actor.hpp"

/**
 * @brief PointLightクラス
 */
class PointLight : public Actor
{
public:
	/**
	 * コンストラクタ
	 */
	PointLight() = default;

	/**
	 * デストラクタ
	 */
	~PointLight() = default;

private:
	void OnInit() override final;
	void OnUpdate() override final;
	void OnUninit() override final;
};
