/*+===================================================================
	File: App.hpp
	Summary: ウィンドウの作成をするヘッダファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/19 13:15 初回作成
			  /07/19 18:56 D3D12の初期化を追加
			  /07/20 22:53 描画処理を追加
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <Windows.h>
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_4.h>

// ==============================
//	Linker
// ==============================
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

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

	bool InitD3D();
	void TermD3D();
	void Render();
	void WaitGPU();
	void Present(uint32_t interval);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

	static const uint32_t FrameCount = 2;	// フレームバッファの数

	HINSTANCE m_hInst{};	// インスタンスハンドル
	HWND m_hWnd{};			// ウィンドウハンドル
	uint32_t m_unWidth{};	// ウィンドウの幅
	uint32_t m_unHeight{};	// ウィンドウの高さ

	ID3D12Device* m_pDevice{};								// Direct3D 12デバイス
	ID3D12CommandQueue* m_pQueue{};							// コマンドキュー
	IDXGISwapChain3* m_pSwapChain{};						// スワップチェーン
	ID3D12Resource* m_pColorBuffer[FrameCount]{};			// カラーバッファ
	ID3D12CommandAllocator* m_pCmdAllocater[FrameCount]{};	// コマンドアロケータ
	ID3D12GraphicsCommandList* m_pCmdList{};				// コマンドリスト
	ID3D12DescriptorHeap* m_pHeapRTV{};						// レンダーターゲットビューのヒープ(ディスクリプタヒープ)
	ID3D12Fence* m_pFence{};								// フェンス
	HANDLE m_hFenceEvent{};									// フェンスイベントハンドル
	uint64_t m_unFenceCounter[FrameCount]{};				// フェンスカウンター(フレームごとに1つ)
	uint32_t m_unFrameIndex{};								// 現在のフレームインデックス
	D3D12_CPU_DESCRIPTOR_HANDLE m_hRTV[FrameCount]{};		// レンダーターゲットビューのハンドル(CPUディスクリプタ)
};
