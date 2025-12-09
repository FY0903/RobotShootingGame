/*+===================================================================
	File: App.hpp
	Summary: ウィンドウの作成をするヘッダファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/19 13:15 初回作成
			  /07/19 18:56 D3D12の初期化を追加
			  /07/20 22:53 描画処理を追加
			  /07/21 00:19 リファクタリング
			  /07/22 05:03 描画初期化処理を追加
			  /07/22 16:48 深度ステンシルビューの追加
			  /07/23 15:45 テクスチャ読み込み追加
			  /07/24 13:03 メッシュ読み込み追加
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Window/Window.hpp"
#include "System/Engine/Engine.hpp"

#include "Game/Scene/SceneBase.hpp"

class App
{
public:
	/**
	 * @brief コンストラクタ
	 */
	App() = default;

	/**
	 * @brief デストラクタ
	 */
	~App() = default;

	void Init(uint32_t width, uint32_t height, HINSTANCE hInstance, int nCmdShow);

	void UnInit();

	/**
	 * @brief アプリケーションを実行
	 */
	void Run();

private:
	SceneBase* m_pCurrentScene{};

	float m_fpsTimer{};
	uint32_t m_fpsFrames{};
	float m_fps{};
};
