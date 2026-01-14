/*+===================================================================
	File: Empty.hpp
	Summary: 空のアクターを表すクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/30 00:39 初回作成
			26/01/14 17:57 コメント記載
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
	/**
	 * @brief 初期化処理
	 */
	void OnInit() override final;
	
	/**
	 * @brief 更新処理
	 */
	void OnUpdate() override final;

	/**
	 * @brief 固定更新処理
	 */
	void OnFixedUpdate() override final;

	/**
	 * @brief 終了処理
	 */
	void OnUninit() override final;
};
