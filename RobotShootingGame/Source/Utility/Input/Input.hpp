/*+===================================================================
	File: Input.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/04 15:04:07 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

#undef max
#undef min

// ==============================
//	include
// ==============================
#include <Windows.h>

namespace Input
{
	HRESULT Init();

	void Update();

	void UpdateRelativeMousePos(LPARAM In_Lparam);

	void EndUpdateInput();

	bool IsKeyPress(BYTE key);

	bool IsKeyTrigger(BYTE key);

	bool IsKeyRelease(BYTE key);

	bool IsKeyRepeat(BYTE key);

	bool IsAnyKeyPress();

	const POINT& GetMousePos();

	POINT GetMouseDifferencePos();

	POINTS GetMouseRelativePos();

	int GetMouseWheelInput();
}
