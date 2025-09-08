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
#include "../Utility/Input/Input.hpp"
#include "../Utility/Sound/Sound.hpp"

// ==============================
//	lib
// ==============================
#pragma comment(lib, "winmm.lib")

void App::Init(uint32_t width, uint32_t height, HINSTANCE hInstance, int nCmdShow)
{
	// ウィンドウの初期化
	if (FAILED(Window::GetInstance().Init(width, height, hInstance, nCmdShow)))
	{
		MessageBox(nullptr, "ウィンドウの初期化に失敗しました", "エラー", MB_OK);
		exit(-1);
	}

	// Engineの初期化
	Engine::GetInstance().Init(Window::GetInstance().GetHandle());

	// Inputの初期化
	Input::Init();

	// Soundの初期化
	if (FAILED(Sound::GetInstance().Init()))
	{
		MessageBox(nullptr, "サウンドの初期化に失敗しました", "エラー", MB_OK);
		exit(-1);
	}

	// Sceneの初期化
	m_Scene.Init();
}

void App::UnInit()
{
	// Sceneの終了処理
	m_Scene.UnInit();

	// Soundの終了処理
	Sound::GetInstance().Uninit();
}

void App::Run()
{
#ifdef _DEBUG
	DWORD fpsCount = 0;    // FPSカウント
	DWORD FPS = 0;        // 直近のFPS
	DWORD fpsTime = timeGetTime(); // FPS計測用の時間
#endif // _DEBUG

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
			// キー入力の更新
			Input::Update();

			// シーンの更新と描画
			m_Scene.Update();
			Engine::GetInstance().BeginDraw();
			m_Scene.Draw();
			Engine::GetInstance().EndDraw();

			Input::EndUpdateInput();

#ifdef _DEBUG

			++fpsCount;// 処理回数をカウント

			// ------------------------------
			//    FPSの計測
			// ------------------------------
			if (timeGetTime() - fpsTime >= 1000)    // 1000ms経過したら
			{
				// 整数型から文字列へ変換
				std::string mes;
				mes = "fps:" + std::to_string(fpsCount);

				SetWindowText(Window::GetInstance().GetHandle(), mes.c_str());    // FPSの表示

				// 次の計測の準備
				fpsCount = 0;
				fpsTime = timeGetTime();
			}
#endif // _DEBUG
		}
	}
}