/*+===================================================================
	File: Window.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/08/23 11:52:17 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <Windows.h>
#include <cstdint>

#include "../../Utility/Singleton/Singleton.hpp"

/**
 * @brief Windowクラス
 */
class Window : public Singleton<Window>
{
public:
	HRESULT Init(uint32_t width, uint32_t height);

	HWND GetHandle() const { return m_hWnd; }
	uint32_t GetWidth() const { return m_unWidth; }
	uint32_t GetHeight() const { return m_unHeight; }

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

	HINSTANCE m_hInst{};	// インスタンスハンドル
	HWND m_hWnd{};			// ウィンドウハンドル
	uint32_t m_unWidth{};	// ウィンドウの幅
	uint32_t m_unHeight{};	// ウィンドウの高さ
};
