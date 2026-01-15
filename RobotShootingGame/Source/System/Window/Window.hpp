/*+===================================================================
	File: Window.hpp
	Summary: Win32ウィンドウクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/08/23 11:52 初回作成
			26/01/15 11:48 コメント記載
===================================================================+*/
#pragma once

/**
 * @brief Windowクラス
 */
class Window : public Singleton<Window>
{
public:
	/**
	 * @brief 指定された幅と高さでアプリケーションやウィンドウの初期化を行います。
	 * @param width 初期化するウィンドウまたは描画領域の幅（ピクセル単位）。
	 * @param height 初期化するウィンドウまたは描画領域の高さ（ピクセル単位）。
	 * @param hInstance アプリケーションのインスタンスハンドル（HINSTANCE）。ウィンドウクラスの登録やウィンドウ作成に使用されます。
	 * @param nCmdShow ウィンドウの表示方法を指定するフラグ（通常は WinMain に渡される nCmdShow を使用）。ShowWindow に渡される値と同様に扱われます。
	 * @return HRESULT 型の結果コード。成功時は S_OK（または成功を示すコード）を返し、失敗時はエラーを示す HRESULT を返します。
	 */
	HRESULT Init(uint32_t width, uint32_t height, HINSTANCE hInstance, int nCmdShow);

	/**
	 * @brief 関連付けられたウィンドウハンドル (HWND) を取得します。
	 * @return このオブジェクトに関連付けられた HWND を返します。
	 */
	HWND GetHandle() const { return m_hWnd; }

	/**
	 * @brief ウィンドウの幅を取得します。
	 * @return ウィンドウの幅（ピクセル単位）。
	 */
	uint32_t GetWidth() const { return m_unWidth; }

	/**
	 * @brief ウィンドウの高さを取得します。
	 * @return ウィンドウの高さ（ピクセル単位）。
	 */
	uint32_t GetHeight() const { return m_unHeight; }

private:
	friend class Singleton<Window>;
	/**
	 * @brief コンストラクタ
	 */
	Window() = default;
	/**
	 * @brief デストラクタ
	 */
	~Window() = default;

	/**
	 * @brief ウィンドウプロシージャ
	 * @param hWnd ウィンドウハンドル
	 * @param msg メッセージコード
	 * @param wp メッセージの追加情報（WPARAM）
	 * @param lp メッセージの追加情報（LPARAM）
	 * @return LRESULT 型の結果コード
	 */
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

	HINSTANCE m_hInst{};	// インスタンスハンドル
	HWND m_hWnd{};			// ウィンドウハンドル
	uint32_t m_unWidth{};	// ウィンドウの幅
	uint32_t m_unHeight{};	// ウィンドウの高さ
};
