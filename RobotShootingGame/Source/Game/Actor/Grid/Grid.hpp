/*+===================================================================
	File: Grid.hpp
	Summary: 座標軸とグリッドを表示するクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/22 16:46 初回作成
			26/01/14 18:29 コメント記載
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
	/**
	 * @brief 初期化処理
	 */
	void OnInit() override final;
	
	/**
	 * @brief 更新処理
	 */
	void OnUpdate() override final;

	/**
	 * @brief 終了処理
	 */
	void OnUninit() override final;
};
