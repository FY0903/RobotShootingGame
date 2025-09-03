/*+===================================================================
	File: App.cpp
	Summary: ウィンドウの作成をするソースファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/19 13:15 初回作成
			  /07/19 18:56 D3D12の初期化を追加
			  /07/20 22:53 描画処理の追加
			  /07/21 00:18 リファクタリング
			  /07/22 05:03 描画初期化処理を追加
			  /07/22 16:48 深度ステンシルビューの追加
			  /07/23 15:45 テクスチャ読み込み追加
			  /07/24 13:03 メッシュ読み込み追加
===================================================================+*/

// ==============================
//	include
// ==============================
#include "App.hpp"

// ==============================
//	lib
// ==============================
#pragma comment(lib, "winmm.lib")

App::App(uint32_t width, uint32_t height)
{
	// ウィンドウの初期化
	if (FAILED(Window::GetInstance().Init(width, height)))
	{
		MessageBox(nullptr, "ウィンドウの初期化に失敗しました", "エラー", MB_OK);
		exit(-1);
	}

	// Engineの初期化
	Engine::GetInstance().Init(Window::GetInstance().GetHandle());

	// Sceneの初期化
	m_Scene.Init();
}

App::~App()
{
	// Sceneの終了処理
	m_Scene.UnInit();
}

void App::Run()
{
	MSG msg{};
	// ウィンドウの×ボタンが押されるまでループ
	while (msg.message != WM_QUIT)
	{
		// メッセージがあるか確認
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			// メッセージがあれば翻訳して送る
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_Scene.Update();
			Engine::GetInstance().BeginDraw();
			m_Scene.Draw();
			Engine::GetInstance().EndDraw();
		}
	}
}