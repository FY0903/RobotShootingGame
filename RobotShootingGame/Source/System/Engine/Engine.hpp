/*+===================================================================
	File: Engine.hpp
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
 * @brief Engineクラス
 */
class Engine
{
public:
	/**
	 * コンストラクタ
	 */
	Engine() = default;

	/**
	 * デストラクタ
	 */
	~Engine() = default;

	HRESULT Init(uint32_t width, uint32_t height, HWND wnd);
	void Update();
	void Draw();
	void UnInit();

	void BeginDraw();
	void EndDraw();

private:
	HRESULT CreateDevice();							
	HRESULT CreateCommandQueue();
	HRESULT CreateSwapChain();
	HRESULT CreateCommandList();
	HRESULT CreateFence();
	void CreateViewPort();
	void CreateScissorRect();

	HRESULT CreateRenderTargetView();
	HRESULT CreateDepthStencilView();

	void WaitRender();

	uint32_t m_unFrameCount = 2;
	UINT m_CurrentBackBufferIndex{};

	ComPtr<ID3D12Device> m_pDevice{};					// Direct3D 12デバイス
	ComPtr<ID3D12CommandQueue> m_pQueue{};				// コマンドキュー
	ComPtr<IDXGISwapChain3> m_pSwapChain{};				// スワップチェーン
	ComPtr<ID3D12CommandAllocator> m_pAllocator[2]{};	// コマンドアロケーター
	ComPtr<ID3D12GraphicsCommandList> m_pCommandList{}; // コマンドリスト
	HANDLE m_hFenceEvent{};								// フェンスイベントハンドル
	ComPtr<ID3D12Fence> m_pFence{};						// フェンス
	UINT64 m_fenceValue[2]{};							// フェンスの値
	D3D12_VIEWPORT m_Viewport{};						// ビューポート
	D3D12_RECT m_scissor{};								// シザー矩形

	UINT m_rtvDescriptorSize{};						// RTVディスクリプタサイズ
	ComPtr<ID3D12DescriptorHeap> m_pRtvHeap{};		// RTVディスクリプタヒープ
	ComPtr<ID3D12Resource> m_pRenderTargets[2]{};	// レンダーターゲット

	UINT m_dsvDescriptorSize{};						// DSVディスクリプタサイズ
	ComPtr<ID3D12DescriptorHeap> m_pDsvHeap{};		// DSVディスクリプタヒープ
	ComPtr<ID3D12Resource> m_pDepthStencil{};		// 深度ステンシルバッファ

	ID3D12Resource* m_pCurrentRenderTarget{}; // 現在のレンダーターゲット

	HWND m_hWnd{};			// ウィンドウハンドル
	uint32_t m_unWidth{};	// ウィンドウの幅
	uint32_t m_unHeight{};	// ウィンドウの高さ
};
