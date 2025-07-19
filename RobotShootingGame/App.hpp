/*+===================================================================
	File: App.hpp
	Summary: ウィンドウの作成をするヘッダファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/19 13:15 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <Windows.h>
#include <cstdint>

/**
 * @brief Appクラス
 */
class App
{
public:
	/**
	 * @brief 指定された幅と高さでアプリケーションを初期化します。
	 * @param width アプリケーションウィンドウの幅（ピクセル単位）。
	 * @param height アプリケーションウィンドウの高さ（ピクセル単位）。
	 */
	App(uint32_t width, uint32_t height);

	/**
	 * @brief デストラクタ
	 */
	~App() = default;

	/**
	 * @brief アプリケーションを実行します。
	 */
	void Run();

private:
	bool InitApp();
	void TermApp();
	bool InitWnd();
	void TermWnd();
	void MainLoop();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

	HINSTANCE m_hInst{};	// インスタンスハンドル
	HWND m_hWnd{};			// ウィンドウハンドル
	uint32_t m_unWidth{};	// ウィンドウの幅
	uint32_t m_unHeight{};	// ウィンドウの高さ
};
