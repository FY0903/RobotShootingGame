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

int main()
{
#if defined(_DEBUG) || defined(DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//_CrtSetBreakAlloc(396); // メモリリークの検出を有効化（必要に応じて行番号を変更）

	// アプリケーションを実行
	App app(960, 540, DXGI_FORMAT_R10G10B10A2_UNORM);
	app.Run();

	return 0;
}