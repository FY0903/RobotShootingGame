/*+===================================================================
	File: Sepia.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2026/01/13 16:41:42 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/RenderPass/RenderPass.hpp"

/**
 * @brief Sepiaクラス
 */
class Sepia : public RenderPass
{
public:
	using RenderPass::RenderPass;

	/**
	 * コンストラクタ
	 */
	Sepia() = default;

	/**
	 * デストラクタ
	 */
	~Sepia() = default;

	void Init() override final;

private:
	void CreateRootSignature() override final;
	void CreatePSO() override final;
};
