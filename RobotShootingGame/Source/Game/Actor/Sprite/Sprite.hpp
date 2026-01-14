/*+===================================================================
	File: Sprite.hpp
	Summary: スプライトを表すクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/24 08:33 初回作成
			26/01/14 18:49 コメント記載
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

	std::array<ConstantBuffer*, FRAME_BUFFER_COUNT> m_pTimeCB{}; // 時間用定数バッファ
};
