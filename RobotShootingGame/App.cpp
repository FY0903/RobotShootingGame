/*+===================================================================
	File: App.cpp
	Summary: ウィンドウの作成をするソースファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/19 13:15 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "App.hpp"

// ==============================
//	constexpr
// ==============================
constexpr const char* ClassName = "RobotShootingGameClass";

App::App(uint32_t width, uint32_t height)
{
	m_unWidth = width;
	m_unHeight = height;
}

void App::Run()
{
	if (InitApp()) MainLoop();

	TermApp();
}

bool App::InitApp()
{
	// ウィンドウの初期化
	if (!InitWnd()) return false;

	return true;	// 正常終了
}

void App::TermApp()
{
	// ウィンドウの終了処理
	TermWnd();
}

bool App::InitWnd()
{
	// インスタンスハンドルを取得
	HINSTANCE hIst = GetModuleHandle(nullptr);
	if (!hIst) return false;

	// ウィンドウの設定
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(WNDCLASSEX);							// 構造体のサイズ
	wc.style = CS_HREDRAW | CS_VREDRAW;						// ウィンドウのスタイル
	wc.lpfnWndProc = WndProc;								// ウィンドウプロシージャのポインタ
	wc.hIcon = LoadIcon(hIst, IDI_APPLICATION);				// アイコンのハンドル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);			// カーソルのハンドル
	wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);	// 背景ブラシのハンドル
	wc.lpszMenuName = nullptr;								// メニューの名前（nullptrならメニューなし）
	wc.lpszClassName = ClassName;							// クラス名
	wc.hIconSm = LoadIcon(hIst, IDI_APPLICATION);			// 小さいアイコンのハンドル

	// ウィンドウの登録
	if (!RegisterClassEx(&wc))
	{
		MessageBox(nullptr, "ウィンドウの登録に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false;
	}

	// インスタンスハンドル設定
	m_hInst = hIst;

	// ウィンドウサイズを調整
	RECT rect{};
	rect.right = static_cast<LONG>(m_unWidth);
	rect.bottom = static_cast<LONG>(m_unHeight);

	// ウィンドウサイズを調整
	long style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	AdjustWindowRect(&rect, style, FALSE);

	// ウィンドウを生成
	m_hWnd = CreateWindowEx(
		0,								// 拡張スタイル（なし）
		ClassName,						// クラス名
		TEXT("Robot Shooting Game"),	// ウィンドウのタイトル
		style,							// ウィンドウのスタイル
		CW_USEDEFAULT,					// X座標（デフォルト）
		CW_USEDEFAULT,					// Y座標（デフォルト）
		rect.right - rect.left,			// 幅
		rect.bottom - rect.top,			// 高さ
		nullptr,						// 親ウィンドウ（nullptrならトップレベルウィンドウ）
		nullptr,						// メニューハンドル（nullptrならメニューなし）
		m_hInst,						// インスタンスハンドル
		nullptr);						// 追加のパラメータ（nullptrならなし）

	if (!m_hWnd)
	{
		MessageBox(nullptr, "ウィンドウの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false;
	}

	// ウィンドウを表示
	ShowWindow(m_hWnd, SW_SHOWNORMAL);

	// ウィンドウの更新
	UpdateWindow(m_hWnd);

	// ウィンドウにフォーカスを設定
	SetFocus(m_hWnd);

	return true;	// 正常終了
}

void App::TermWnd()
{
	// ウィンドウの登録を解除
	if (m_hInst) UnregisterClass(ClassName, m_hInst);

	m_hInst = nullptr;
	m_hWnd = nullptr;
}

void App::MainLoop()
{
	MSG msg{};
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

LRESULT App::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0); // アプリケーションの終了
		return 0;
	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wp, lp);
}
