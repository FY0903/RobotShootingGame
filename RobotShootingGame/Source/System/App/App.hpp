/*+===================================================================
	File: App.hpp
	Summary: ウィンドウの作成をするヘッダファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/19 13:15 初回作成
			  /07/19 18:56 D3D12の初期化を追加
			  /07/20 22:53 描画処理を追加
			  /07/21 00:19 リファクタリング
			  /07/22 05:03 描画初期化処理を追加
			  /07/22 16:48 深度ステンシルビューの追加
			  /07/23 15:45 テクスチャ読み込み追加
			  /07/24 13:03 メッシュ読み込み追加
			26/01/14 19:13 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Game/Scene/SceneBase.hpp"

class App
{
public:
	/**
	 * @brief コンストラクタ
	 */
	App() = default;

	/**
	 * @brief デストラクタ
	 */
	~App() = default;

	/**
	 * @brief 指定された幅、高さ、およびインスタンスパラメータで初期化を行います。
	 * @param width 幅（ピクセル単位）。
	 * @param height 高さ（ピクセル単位）。
	 * @param hInstance アプリケーションインスタンスのハンドル。
	 * @param nCmdShow ウィンドウの表示方法を指定するフラグ。
	 */
	void Init(uint32_t width, uint32_t height, HINSTANCE hInstance, int nCmdShow);

	/**
	 * @brief アプリケーションの終了処理
	 */
	void UnInit();

	/**
	 * @brief アプリケーションを実行
	 */
	void Run();

private:
	SceneBase* m_pCurrentScene{};	// 現在のシーン

	float m_fpsTimer{};				// FPS計測用タイマー
	uint32_t m_fpsFrames{};			// FPS計測用フレーム数
	float m_fps{};					// 現在のFPS
};
