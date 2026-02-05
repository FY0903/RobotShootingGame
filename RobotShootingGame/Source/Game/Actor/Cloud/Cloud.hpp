/*+===================================================================
	File: Cloud.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2026/02/03 20:45:41 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Game/Actor/Actor.hpp"

/**
 * @brief Cloudクラス
 */
class Cloud : public Actor
{
public:
	/**
	 * コンストラクタ
	 */
	Cloud() = default;

	/**
	 * デストラクタ
	 */
	~Cloud() = default;

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

	std::array<ConstantBuffer*, FRAME_BUFFER_COUNT> m_pLightCB{};	// ライト用定数バッファ
	std::array<ConstantBuffer*, FRAME_BUFFER_COUNT> m_pTimeCB{};	// 時間用定数バッファ
};
