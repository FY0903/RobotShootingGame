/*+===================================================================
	File: MainCamera.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/21 18:12:36 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Game/Actor/Actor.hpp"

/**
 * @brief MainCameraクラス
 */
class MainCamera : public Actor
{
public:
	/**
	 * コンストラクタ
	 */
	MainCamera() = default;

	/**
	 * デストラクタ
	 */
	~MainCamera() = default;

	void Init() override final;
	void Uninit() override final;
};
