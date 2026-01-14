/*+===================================================================
	File: Engine.hpp
	Summary: D3D12エンジンクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/08/23 11:45 初回作成
			26/01/14 20:37 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>

#include "Utility/RenderTarget/RenderTarget.hpp"

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
	/**
	 * @brief 初期化を実行します。
	 * @param wnd ウィンドウハンドル。
	 * @return 成功した場合はS_OK、失敗した場合はエラーコード。
	 */
	HRESULT Init(HWND wnd);

	/**
	 * @brief 描画処理
	 */
	void Draw();

	/**
	 * @brief デバイスを取得します。
	 * @return ID3D12Device ポインタ。
	 */
	ID3D12Device* GetDevice() const { return m_pDevice.Get(); }

	/**
	 * @brief コマンドリストを取得します。
	 * @return ID3D12GraphicsCommandList へのポインタ。
	 */
	ID3D12GraphicsCommandList* GetCommandList() const { return m_pCommandList.Get(); }

	/**
	 * @brief 現在のバックバッファのインデックスを取得します。
	 * @return 現在のバックバッファのインデックス。
	 */
	UINT GetCurrentBackBufferIndex() const { return m_currentBackBufferIndex; }

	/**
	 * @brief レンダーターゲットを設定します。
	 * @param pRenderTarget 設定するレンダーターゲットへのポインタ。
	 */
	void SetRenderTarget(RenderTarget* pRenderTarget) { m_pOffScreenRTV = pRenderTarget; }

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

	/**
	 * @brief 描画処理を開始します。
	 */
	void BeginDraw();

	/**
	 * @brief 描画処理を終了します。
	 */
	void EndDraw();

	/**
	 * @brief バックバッファをレンダーターゲットとして設定します。
	 */
	void SetBackBufferRenderTarget();

	/**
	 * @brief デバイスを作成します。
	 * @return 操作の成功または失敗を示すHRESULT値。
	 */
	HRESULT CreateDevice();

	/**
	 * @brief コマンドキューを作成します。
	 * @return 成功した場合はS_OK、失敗した場合はエラーコードを返します。
	 */
	HRESULT CreateCommandQueue();

	/**
	 * @brief スワップチェーンを作成します。
	 * @return 操作の成功または失敗を示すHRESULT値。
	 */
	HRESULT CreateSwapChain();

	/**
	 * @brief コマンドリストを作成します。
	 * @return 成功した場合はS_OK、失敗した場合はエラーコードを返します。
	 */
	HRESULT CreateCommandList();

	/**
	 * @brief フェンスを作成します。
	 * @return 成功した場合はS_OK、失敗した場合はエラーコードを返します。
	 */
	HRESULT CreateFence();

	/**
	 * @brief ビューポートを作成します。
	 */
	void CreateViewPort();

	/**
	 * @brief シザー矩形を作成します。
	 */
	void CreateScissorRect();

	/**
	 * @brief レンダーターゲットビューを作成します。
	 * @return 操作の成功または失敗を示すHRESULT値。
	 */
	HRESULT CreateRenderTargetView();

	/**
	 * @brief 深度ステンシルビューを作成します。
	 * @return 成功した場合は S_OK を返し、失敗した場合はエラーコードを返します。
	 */
	HRESULT CreateDepthStencilView();

	/**
	 * @brief レンダリングが完了するまで待機します。
	 */
	void WaitRender();

	uint32_t m_frameCount = FRAME_BUFFER_COUNT;			// フレームバッファの数
	UINT m_currentBackBufferIndex{};					// 現在のバックバッファのインデックス

	ComPtr<ID3D12Device> m_pDevice{};					// Direct3D 12デバイス
	ComPtr<ID3D12CommandQueue> m_pQueue{};				// コマンドキュー
	ComPtr<IDXGISwapChain3> m_pSwapChain{};				// スワップチェーン
	ComPtr<ID3D12CommandAllocator> m_pAllocator[2]{};	// コマンドアロケーター
	ComPtr<ID3D12GraphicsCommandList> m_pCommandList{}; // コマンドリスト
	HANDLE m_fenceEvent{};								// フェンスイベントハンドル
	ComPtr<ID3D12Fence> m_pFence{};						// フェンス
	UINT64 m_fenceValue[2]{};							// フェンスの値
	D3D12_VIEWPORT m_viewport{};						// ビューポート
	D3D12_RECT m_scissor{};								// シザー矩形

	UINT m_RTVDescriptorSize{};						// RTVディスクリプタサイズ
	ComPtr<ID3D12DescriptorHeap> m_pRTVHeap{};		// RTVディスクリプタヒープ
	ComPtr<ID3D12Resource> m_pRenderTargets[2]{};	// レンダーターゲット

	UINT m_DSVDescriptorSize{};						// DSVディスクリプタサイズ
	ComPtr<ID3D12DescriptorHeap> m_pDsvHeap{};		// DSVディスクリプタヒープ
	ComPtr<ID3D12Resource> m_pDepthStencil{};		// 深度ステンシルバッファ

	ComPtr<ID3D12Resource> m_pCurrentRenderTarget{}; // 現在のレンダーターゲット

	HWND m_hWnd{};			// ウィンドウハンドル

	RenderTarget* m_pOffScreenRTV{}; // オフスクリーンレンダーターゲット
};
