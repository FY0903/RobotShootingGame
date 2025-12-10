/*+===================================================================
	File: Render.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/09 19:24:38 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/RenderTarget/RenderTarget.hpp"

/**
 * @brief Renderクラス
 */
class Render : public Singleton<Render>
{
public:
	enum DefaultRenderTarget : size_t
	{
		DEFAULT_RENDER_TARGET_0,
		DEFAULT_RENDER_TARGET_1,
		DEFAULT_RENDER_TARGET_2,
		RENDER_DEFAULT_TARGET_NUM
	};

	void Init();

	void Uninit();

private:
	friend class Singleton<Render>;

	/**
	 * コンストラクタ
	 */
	Render();

	/**
	 * デストラクタ
	 */
	~Render();

	std::array<RenderTarget*, RENDER_DEFAULT_TARGET_NUM> m_pDefaultRenderTargets{}; // デフォルトのレンダーターゲット

	float clearColor[4]{};
};
