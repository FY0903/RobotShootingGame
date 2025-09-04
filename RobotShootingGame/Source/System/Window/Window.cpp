/*+===================================================================
	File: Window.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/08/23 11:52:17 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Window.hpp"
#include "../../Utility/Input/Input.hpp"

// ==============================
//	constexpr
// ==============================
namespace
{
	constexpr LPCSTR ClassName = "RobotShootingGameWindowClass"; // ウィンドウクラス名
}

HRESULT Window::Init(uint32_t width, uint32_t height, HINSTANCE hInstance, int nCmdShow)
{
	m_unWidth = width;
	m_unHeight = height;

	// インスタンスハンドルを取得
	HINSTANCE hIst = GetModuleHandle(nullptr);
	if (!hIst) return E_FAIL;

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
		return E_FAIL;
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
		this);							// ユーザデータ（thisポインタを渡す）

	if (!m_hWnd)
	{
		MessageBox(nullptr, "ウィンドウの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// ウィンドウを表示
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);
	
	return S_OK;
}

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// ウィンドウプロシージャのポインタを取得
	auto instance = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	
	switch (msg)
	{
	case WM_CREATE:
		{
			// ウィンドウの作成時にユーザデータを設定
			auto pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lp);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));	// ユーザデータにthisポインタを設定
		}
		return 0;

	case WM_MOUSEHWHEEL:
		Input::UpdateMouseWheel(GET_WHEEL_DELTA_WPARAM(wp));
		break;

	case WM_MOUSEMOVE:
		Input::UpdateRelativeMousePos(lp);
		break;

	case WM_DESTROY:
		PostQuitMessage(0); // アプリケーションの終了
		return 0;
	}

	return DefWindowProc(hWnd, msg, wp, lp);
}
