/*+===================================================================
	File: DirectX.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/08/23 11:45 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "DirectX.hpp"

// ==============================
//	define
// ==============================

// ==============================
//	構造体定義
// ==============================

// ==============================
//	列挙型定義
// ==============================

// ==============================
//	プロトタイプ宣言
// ==============================

// ==============================
//	定数定義
// ==============================

// ==============================
//	グローバル変数宣言
// ==============================

HRESULT DirectX::Init(uint32_t width, uint32_t height, HWND wnd)
{
		// ==============================
	//	デバックレイヤーの有効化
	// ==============================
#if defined(_DEBUG) || defined(DEBUG)
	{
		ComPtr<ID3D12Debug> debug;
		HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));

		if (SUCCEEDED(hr))
		{
			debug->EnableDebugLayer(); // デバッグレイヤーを有効化
		}
	}
#endif

	// ==============================
	//	デバイスの生成
	// ==============================
	HRESULT hr = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_11_0,						// 使用する機能レベル
		IID_PPV_ARGS(m_pDevice.GetAddressOf()));	// デバイスのポインタ
	
	if (FAILED(hr))
	{
		MessageBox(nullptr, "Direct3D 12デバイスの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

	// ==============================
	//	コマンドキューの生成
	// ==============================
	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // コマンドリストのタイプ
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; // 優先度
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; // フラグ（なし）
	queueDesc.NodeMask = 0; // ノードマスク（単一ノード）

	hr = m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_pQueue.GetAddressOf()));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "コマンドキューの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

	// ==============================
	//	スワップチェーンの生成
	// ==============================
	// DXGIファクトリーの生成
	ComPtr<IDXGIFactory4> pFactory = nullptr;
	hr = CreateDXGIFactory1(IID_PPV_ARGS(pFactory.GetAddressOf()));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "DXGIファクトリーの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

	// スワップチェーンの設定
	DXGI_SWAP_CHAIN_DESC swapDesc{};
	swapDesc.BufferDesc.Width = width;		// スワップチェーンの幅
	swapDesc.BufferDesc.Height = height;	// スワップチェーンの高さ
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;	// リフレッシュレートの分子（60Hz）
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;	// リフレッシュレートの分母
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // スキャンラインの順序
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // スケーリングの指定
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // バッファのフォーマット
	swapDesc.SampleDesc.Count = 1; // マルチサンプリングのサンプル数
	swapDesc.SampleDesc.Quality = 0; // マルチサンプリングの品質
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // バッファの使用法
	swapDesc.BufferCount = m_unFrameCount; // バッファの数
	swapDesc.OutputWindow = wnd; // 出力ウィンドウ
	swapDesc.Windowed = TRUE; // ウィンドウモード
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // スワップ効果
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // モードスイッチを許可

	// スワップチェーンの生成
	ComPtr<IDXGISwapChain> pSwapChain = nullptr;
	hr = pFactory->CreateSwapChain(
		m_pQueue.Get(),				// コマンドキュー
		&swapDesc,					// スワップチェーンの設定
		pSwapChain.GetAddressOf());				// スワップチェーンのポインタ
	if (FAILED(hr))
	{
		return E_FAIL; // エラー終了
	}

	// IDXGISwapChain3を取得
	hr = pSwapChain.As(&m_pSwapChain); // IDXGISwapChain3のスマートポインタにキャスト
	if (FAILED(hr))
	{
		MessageBox(nullptr, "IDXGISwapChain3の取得に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

	// バックバッファ番号を取得
	m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	// 不要になったので解放
	pFactory.Reset(); // DXGIファクトリーのスマートポインタをリセット
	pSwapChain.Reset(); // IDXGISwapChainのスマートポインタをリセット

	// ==============================
	//	ディスクリプタプールの生成
	// ==============================

	// ==============================
	//	コマンドリストの生成
	// ==============================
	if (!m_commandList.Init(
		m_pDevice.Get(),					// デバイス
		D3D12_COMMAND_LIST_TYPE_DIRECT,		// コマンドリストのタイプ
		FrameCount))						// コマンドアロケータの数
	{
		MessageBox(nullptr, "コマンドリストの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

	// ===============================
	//	レンダーターゲットビューの生成
	// ===============================
	for (uint32_t i = 0; i < FrameCount; ++i)
	{
		if (!m_colorTarget[i].InitFromBackBuffer(
			m_pDevice.Get(),			// デバイス
			m_pPools[POOL_TYPE_RTV],	// レンダーターゲットビュー用のディスクリプタプール
			i,							// バックバッファのインデックス
			m_pSwapChain.Get()))		// スワップチェーン
		{
			MessageBox(nullptr, "バックバッファのレンダーターゲットビューの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return E_FAIL; // エラー終了
		}
	}

	// ==============================
	//	深度ステンシルビューの生成
	// ==============================
	if (!m_depthTarget.Init(
		m_pDevice.Get(),			// デバイス
		m_pPools[POOL_TYPE_DSV],	// 深度ステンシルビュー用のディスクリプタプール
		m_unWidth,					// 幅
		m_unHeight,					// 高さ
		DXGI_FORMAT_D32_FLOAT))		// 深度ステンシルフォーマット
	{
		MessageBox(nullptr, "深度ステンシルビューの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

	// ==============================
	//	フェンスの生成
	// ==============================
	if (!m_fence.Init(m_pDevice.Get())) // フェンスの初期化
	{
		MessageBox(nullptr, "フェンスの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

	// ==============================
	//	ビューポートの設定
	// ==============================
	m_Viewport.TopLeftX = 0.0f;							// ビューポートの左上X座標
	m_Viewport.TopLeftY = 0.0f;							// ビューポートの左上Y座標
	m_Viewport.Width = static_cast<float>(m_unWidth);	// ビューポートの幅
	m_Viewport.Height = static_cast<float>(m_unHeight);	// ビューポートの高さ
	m_Viewport.MinDepth = 0.0f;							// ビューポートの最小深度
	m_Viewport.MaxDepth = 1.0f;							// ビューポートの最大深度

	// ==============================
	//	シザー矩形の設定
	// ==============================
	m_scissor.left = 0;				// シザー矩形の左端
	m_scissor.right = m_unWidth;	// シザー矩形の右端
	m_scissor.top = 0;				// シザー矩形の上端
	m_scissor.bottom = m_unHeight;	// シザー矩形の下端

	return true; // 正常終了
}

void DirectX::Update()
{
}

void DirectX::Draw()
{
}

void DirectX::UnInit()
{
}
