/*+===================================================================
	File: main.cpp
	Summary: エントリーポイント
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/19 13:15 初回作成
			  /07/21 00:20 デバック処理追加
===================================================================+*/


#if defined(_DEBUG) || defined(DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

// ==============================
//	include
// ==============================
#include "App.hpp"
#include "../Utility/Singleton/Singleton.hpp"

// ==============================
//	constexpr
// ==============================
constexpr uint32_t WINDOW_WIDTH = 960;	// ウィンドウの幅
constexpr uint32_t WINDOW_HEIGHT = 540;	// ウィンドウの高さ

int main()
{
#if defined(_DEBUG) || defined(DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// _CrtSetBreakAlloc(13134);
#endif

	// アプリケーションを実行
	App app(WINDOW_WIDTH, WINDOW_HEIGHT);
	app.Run();

	// シングルトンの終了処理
	SingletonFinalizer::Finalize();

	return 0;
}