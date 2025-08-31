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
	//	変数初期化
	// ==============================
	m_hWnd = wnd;
	m_unWidth = width;
	m_unHeight = height;

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
	if (FAILED(CreateDevice()))
	{
		MessageBox(nullptr, "Direct3D 12デバイスの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

	// ==============================
	//	コマンドキューの生成
	// ==============================
	if (FAILED(CreateCommandQueue()))
	{
		MessageBox(nullptr, "コマンドキューの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

	// ==============================
	//	スワップチェーンの生成
	// ==============================
	if (FAILED(CreateSwapChain()))
	{
		MessageBox(nullptr, "DXGIファクトリーの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

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

HRESULT DirectX::CreateDevice()
{
	HRESULT hr = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_11_0,						// 使用する機能レベル
		IID_PPV_ARGS(m_pDevice.GetAddressOf()));	// デバイスのポインタ

	return hr;
}

HRESULT DirectX::CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC desc{};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // コマンドリストのタイプ
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; // 優先度
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; // フラグ（なし）
	desc.NodeMask = 0; // ノードマスク（単一ノード）

	HRESULT hr = m_pDevice->CreateCommandQueue(
		&desc,								// コマンドキューの設定
		IID_PPV_ARGS(m_pQueue.GetAddressOf())); // コマンドキューのポインタ

	return hr;
}

HRESULT DirectX::CreateSwapChain()
{
	// DXGIファクトリーの生成
	ComPtr<IDXGIFactory4> pFactory = nullptr;
	HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(pFactory.GetAddressOf()));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "DXGIファクトリーの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

	// スワップチェーンの設定
	DXGI_SWAP_CHAIN_DESC desc{};
	desc.BufferDesc.Width = m_unWidth;		// スワップチェーンの幅
	desc.BufferDesc.Height = m_unHeight;	// スワップチェーンの高さ
	desc.BufferDesc.RefreshRate.Numerator = 60;	// リフレッシュレートの分子（60Hz）
	desc.BufferDesc.RefreshRate.Denominator = 1;	// リフレッシュレートの分母
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // スキャンラインの順序
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // スケーリングの指定
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // バッファのフォーマット
	desc.SampleDesc.Count = 1; // マルチサンプリングのサンプル数
	desc.SampleDesc.Quality = 0; // マルチサンプリングの品質
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // バッファの使用法
	desc.BufferCount = m_unFrameCount; // バッファの数
	desc.OutputWindow = m_hWnd; // 出力ウィンドウ
	desc.Windowed = TRUE; // ウィンドウモード
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // スワップ効果
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // モードスイッチを許可

	// スワップチェーンの生成
	ComPtr<IDXGISwapChain> pSwapChain = nullptr;
	hr = pFactory->CreateSwapChain(
		m_pQueue.Get(),				// コマンドキュー
		&desc,					// スワップチェーンの設定
		pSwapChain.GetAddressOf());				// スワップチェーンのポインタ
	if (FAILED(hr))
	{
		pFactory->Release();
		return E_FAIL; // エラー終了
	}

	// IDXGISwapChain3を取得
	hr = pSwapChain.As(&m_pSwapChain); // IDXGISwapChain3のスマートポインタにキャスト
	if (FAILED(hr))
	{
		MessageBox(nullptr, "IDXGISwapChain3の取得に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}
}

HRESULT DirectX::CreateCommandList()
{
	return E_NOTIMPL;
}

HRESULT DirectX::CreateFence()
{
	return E_NOTIMPL;
}

HRESULT DirectX::CreateViewPort()
{
	return E_NOTIMPL;
}

HRESULT DirectX::CreateScissorRect()
{
	return E_NOTIMPL;
}
