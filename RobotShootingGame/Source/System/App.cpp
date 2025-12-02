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
#include "Utility/Input/Input.hpp"
#include "Utility/Sound/Sound.hpp"
#include "Utility/Time/Time.hpp"
#include "Game/Scene/SceneGame/SceneGame.hpp"

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

	// Timeの初期化
	Time::GetInstance().Init();

	// MaterialManagerの初期化
	MaterialManager::GetInstance().Init();

	// Soundの初期化
	if (FAILED(Sound::GetInstance().Init()))
	{
		MessageBox(nullptr, "サウンドの初期化に失敗しました", "エラー", MB_OK);
		exit(-1);
	}

	// Sceneの初期化
	m_pCurrentScene = new SceneGame();
	m_pCurrentScene->Init();
}

void App::UnInit()
{
	// Sceneの終了処理
	m_pCurrentScene->Uninit();
	delete m_pCurrentScene;
	m_pCurrentScene = nullptr;

	// Soundの終了処理
	Sound::GetInstance().Uninit();
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
			// キー入力の更新
			Input::Update();

			// Timeの更新
			Time::GetInstance().Update();

			// シーンの更新と描画
			m_pCurrentScene->Update();
			Engine::GetInstance().BeginDraw();
			m_pCurrentScene->Draw();
			Engine::GetInstance().EndDraw();

			Input::EndUpdateInput();

#ifdef _DEBUG
			const float deltaTime = Time::GetInstance().GetDeltaTime();
			m_fpsTimer += deltaTime;
			++m_fpsFrames;

			if (m_fpsTimer >= 1.0f)
			{
				m_fps = static_cast<float>(m_fpsFrames) / m_fpsTimer;
				m_fpsTimer = 0.0f;
				m_fpsFrames = 0;

				// ウィンドウタイトルにFPSを表示
				std::string title = "Robot Shooting Game - FPS: " + std::to_string(static_cast<int>(m_fps));
				SetWindowText(Window::GetInstance().GetHandle(), title.c_str());
			}

#endif // _DEBUG
		}
	}
}