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

#include "Utility/ComPtr.h"
#include "Utility/Singleton/Singleton.hpp"
#include "Utility/ConstantBuffer/ConstantBuffer.hpp"

// ==============================
//	Linker
// ==============================
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

// ==============================
//	constexpr
// ==============================
constexpr size_t FRAME_BUFFER_COUNT = 2; // フレームバッファの数

/**
 * @brief Engineクラス
 */
class Engine : public Singleton<Engine>
{
public:
	HRESULT Init(HWND wnd);

	void BeginDraw();
	void EndDraw();

	ID3D12Device* GetDevice() const { return m_pDevice.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return m_pCommandList.Get(); }
	UINT GetCurrentBackBufferIndex() const { return m_CurrentBackBufferIndex; }

	void SetViewProjCB(const ConstantBuffer* ConstantBuffer);

private:
	friend class Singleton<Engine>;
	/**
	 * @brief コンストラクタ
	 */
	Engine() = default;

	/**
	 * @brief デストラクタ
	 */
	~Engine();

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

	uint32_t m_unFrameCount = FRAME_BUFFER_COUNT;
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

	ComPtr<ID3D12Resource> m_pCurrentRenderTarget{}; // 現在のレンダーターゲット

	HWND m_hWnd{};			// ウィンドウハンドル

	ConstantBuffer* m_pViewProjCB[FRAME_BUFFER_COUNT]{}; // ビュー・プロジェクション行列用定数バッファ
};
