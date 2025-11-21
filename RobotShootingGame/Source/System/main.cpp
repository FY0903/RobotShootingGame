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
#include "main.hpp"
#include "App.hpp"
#include "../Utility/Singleton/Singleton.hpp"
#include "../System/Engine/Engine.hpp"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
#if defined(_DEBUG) || defined(DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(207);
#endif

	App App;

	// アプリケーションを実行
	App.Init(WINDOW_WIDTH, WINDOW_HEIGHT, hInstance, nCmdShow);
	App.Run();
	App.UnInit();

	// シングルトンの終了処理
	SingletonFinalizer::Finalize();

	return 0;
}