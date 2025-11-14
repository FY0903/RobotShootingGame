/*+===================================================================
	File: MeshRenderer.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/14 17:22:58 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/Compornent/Component.hpp"

/**
 * @brief MeshRendererクラス
 */
class MeshRenderer : public Component
{
public:
	using Component::Component;

	void Init() override final;
	void Update() override final;
	void Draw() override final;
	void Uninit() override final;
};
