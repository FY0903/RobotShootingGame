/*+===================================================================
	File: DirectX.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/08/23 11:45 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <Windows.h>
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

// ==============================
//	Linker
// ==============================
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;

/**
 * @brief DirectXクラス
 */
class DirectX
{
public:
	/**
	 * コンストラクタ
	 */
	DirectX() = default;

	/**
	 * デストラクタ
	 */
	~DirectX() = default;

	HRESULT Init(uint32_t width, uint32_t height, HWND wnd);
	void Update();
	void Draw();
	void UnInit();

private:
	HRESULT CreateDevice();							
	HRESULT CreateCommandQueue();
	HRESULT CreateSwapChain();
	HRESULT CreateCommandList();
	HRESULT CreateFence();
	HRESULT CreateViewPort();
	HRESULT CreateScissorRect();

	ComPtr<ID3D12Device> m_pDevice{};		// Direct3D 12デバイス
	ComPtr<ID3D12CommandQueue> m_pQueue{};	// コマンドキュー
	ComPtr<IDXGISwapChain3> m_pSwapChain{};	// スワップチェーン
	uint32_t m_frameIndex{};				// 現在のフレーム番号
	D3D12_VIEWPORT m_Viewport{};			// ビューポート
	D3D12_RECT m_scissor{};					// シザー矩形

	HWND m_hWnd{};			// ウィンドウハンドル
	uint32_t m_unWidth{};	// ウィンドウの幅
	uint32_t m_unHeight{};	// ウィンドウの高さ

	uint32_t m_unFrameCount = 2;
};
