/*+===================================================================
	File: Input.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/04 15:04:07 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Input.hpp"

namespace Input
{
	// ==============================
	//	グローバル変数宣言
	// ==============================
	namespace
	{
		BYTE  g_keyTable[256];		// キーボードの入力情報
		BYTE  g_oldTable[256];		// 1フレーム前のキーボードの入力情報
		POINT g_MousePos;			// マウス座標
		POINT g_MouseSavePos;		// マウスの保存用座標
		POINTS g_MouseRelativePos;	// マウスポインタのウィンドウ上の相対座標
		short g_MouseWheelData;		// マウスホイールの入力情報
		bool g_MiddleButton;		// マウスの中ボタンの入力情報
	}

	HRESULT Init()
	{	
		// 最初の入力
		if (GetKeyboardState(g_keyTable))
		{
			return S_OK;
		}
		else
		{
			MessageBox(nullptr, "キーボードの初期化に失敗しました", "エラー", MB_OK);
			return E_FAIL;
		}
	}

	void Update()
	{
		// 古い入力を更新
		memcpy_s(g_oldTable, sizeof(g_oldTable), g_keyTable, sizeof(g_keyTable));

		// 現在の入力を取得
		if (!GetKeyboardState(g_keyTable))
			OutputDebugStringA("error : キーボードの更新に失敗しました\n");
		if (!GetCursorPos(&g_MousePos))
			OutputDebugStringA("error : マウス座標の取得に失敗しました\n");
	}

	void UpdateRelativeMousePos(LPARAM In_Lparam)
	{
		// マウスのウィンドウ上の相対座標を取得
		g_MouseRelativePos = MAKEPOINTS(In_Lparam);
	}

	void UpdateMouseWheel(short wheelDelta)
	{
		g_MouseWheelData = wheelDelta;
	}

	void UpdateMiddleButton(bool pressed)
	{
		g_MiddleButton = pressed;
	}

	void EndUpdateInput()
	{
		g_MouseWheelData = 0; // マウスホイールの入力情報をリセット

		// マウス座標の保存
		g_MouseSavePos = g_MousePos;
	}

	bool IsKeyPress(BYTE key)
	{
		return g_keyTable[key] & 0x80;
	}

	bool IsKeyTrigger(BYTE key)
	{
		return (g_keyTable[key] ^ g_oldTable[key]) & g_keyTable[key] & 0x80;
	}

	bool IsKeyRelease(BYTE key)
	{
		return (g_keyTable[key] ^ g_oldTable[key]) & g_oldTable[key] & 0x80;
	}

	bool IsKeyRepeat(BYTE key)
	{
		return IsKeyPress(key) && !IsKeyTrigger(key);
	}

	bool IsAnyKeyPress()
	{
		for (int key = 0; key < 256; ++key)
		{
			if (key >= 166 && 245 >= key || key == VK_LWIN || key == VK_RWIN);	// OEMキーとWindowsキーは無視
			else if (GetAsyncKeyState(key) & 0x8000)
				return true;
		}

		return false;
	}

	const POINT& GetMousePos()
	{
		return g_MousePos;
	}

	POINT GetMouseDifferencePos()
	{
		// 差分格納用
		POINT MouseDifferencePos;

		// 差分の計算
		MouseDifferencePos.x = g_MousePos.x - g_MouseSavePos.x;
		MouseDifferencePos.y = g_MousePos.y - g_MouseSavePos.y;

		return MouseDifferencePos;
	}

	POINTS GetMouseRelativePos()
	{
		return g_MouseRelativePos;
	}

	int GetMouseWheelInput()
	{
		return g_MouseWheelData;
	}
	bool IsMiddleButtonPress()
	{
		return g_MiddleButton;
	}
}
