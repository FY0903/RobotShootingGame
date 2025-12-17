/*+===================================================================
	File: TitleText.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/18 2:42:57 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Game/Actor/Actor.hpp"
#include "Utility/Texture/Texture.hpp"
#include "Utility/ConstantBuffer/ConstantBuffer.hpp"

/**
 * @brief Spriteクラス
 */
class TitleText : public Actor
{
public:
	/**
	 * コンストラクタ
	 */
	TitleText() = default;

	/**
	 * デストラクタ
	 */
	~TitleText() = default;

private:
	void OnInit() override final;
	void OnUpdate() override final;
	void OnUninit() override final;

	std::vector<ConstantBuffer*> m_pPSTimeCB{}; // 時間用定数バッファ
};
