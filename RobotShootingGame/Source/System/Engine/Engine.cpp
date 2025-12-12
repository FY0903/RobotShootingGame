/*+===================================================================
	File: Engine.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/08/23 11:45 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Engine.hpp"
#include "../DirectXTex/d3dx12.h"
#include "System/Window/Window.hpp"
#include "System/Render/Render.hpp"

HRESULT Engine::Init(HWND wnd)
{
	// ==============================
	//	変数初期化
	// ==============================
	m_hWnd = wnd;

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
	//	コマンドリストの生成
	// ==============================
	if (FAILED(CreateCommandList()))
	{
		MessageBox(nullptr, "コマンドリストの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

	// ==============================
	//	フェンスの生成
	// ==============================
	if (FAILED(CreateFence())) // フェンスの初期化
	{
		MessageBox(nullptr, "フェンスの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

	// ===============================
	//	レンダーターゲットビューの生成
	// ===============================
	if (FAILED(CreateRenderTargetView()))
	{
		MessageBox(nullptr, "レンダーターゲットビューの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

	// ==============================
	//	深度ステンシルビューの生成
	// ==============================
	if (FAILED(CreateDepthStencilView()))
	{
		MessageBox(nullptr, "深度ステンシルビューの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

	// ==============================
	//	ビューポートの設定
	// ==============================
	CreateViewPort();

	// ==============================
	//	シザー矩形の設定
	// ==============================
	CreateScissorRect();

	return S_OK; // 正常終了
}

void Engine::BeginDraw()
{
	// コマンドアロケーターのリセット
	m_pAllocator[m_CurrentBackBufferIndex]->Reset();

	// コマンドリストのリセット
	m_pCommandList->Reset(m_pAllocator[m_CurrentBackBufferIndex].Get(), nullptr);

	// ビューポートとシザー矩形の設定
	m_pCommandList->RSSetViewports(1, &m_Viewport); // ビューポートの設定
	m_pCommandList->RSSetScissorRects(1, &m_scissor); // シザー矩形の設定

	// Renderクラスの描画開始処理を呼び出す
	Render::GetInstance().BeginDraw();

#if 0
	// RTVのディスクリプタヒープの開始アドレスを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pRtvHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += m_CurrentBackBufferIndex * m_rtvDescriptorSize;	// 現在のRTVのアドレスを計算
	auto OffScreenColorHandle = m_pOffScreenRTV->GetDescriptorHandle();

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[] = {
		rtvHandle,
		OffScreenColorHandle->HandleCPU
	};
	
	// DSVのディスクリプタヒープの開始アドレスを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_pDsvHeap->GetCPUDescriptorHandleForHeapStart();

	// RTVが使用可能になるまで待つ
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_pCurrentRenderTarget.Get(),
		D3D12_RESOURCE_STATE_PRESENT,			// 変換前のリソース状態
		D3D12_RESOURCE_STATE_RENDER_TARGET);	// 変換後のリソース状態
	CD3DX12_RESOURCE_BARRIER offScreenBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_pOffScreenRTV->Resource(),
		D3D12_RESOURCE_STATE_PRESENT,				// 変換前のリソース状態
		D3D12_RESOURCE_STATE_RENDER_TARGET);		// 変換後のリソース状態

	CD3DX12_RESOURCE_BARRIER barriers[] = { barrier, offScreenBarrier };

	m_pCommandList->ResourceBarrier(_countof(barriers), barriers);

	// RTV設定
	m_pCommandList->OMSetRenderTargets(_countof(rtvHandles), rtvHandles, FALSE, &dsvHandle);

	// RTVをクリア
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f }; // クリアカラー（青色）
	m_pCommandList->ClearRenderTargetView(rtvHandles[0], clearColor, 0, nullptr);
	m_pCommandList->ClearRenderTargetView(rtvHandles[1], m_pOffScreenRTV->GetClearValue().Color, 0, nullptr);

	// DSVをクリア
	m_pCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_pOffScreenRTV->Resource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_pCommandList->ResourceBarrier(1, &barrier);
	auto offScreenHandlePtr = m_pOffScreenRTV->GetDescriptorHandle()->HandleCPU;
	auto dsvHandlePtr = m_pDsvHeap->GetCPUDescriptorHandleForHeapStart();
	m_pCommandList->OMSetRenderTargets(1, &offScreenHandlePtr, FALSE, &dsvHandlePtr);
	
	// RTVをクリア
	m_pCommandList->ClearRenderTargetView(offScreenHandlePtr, m_pOffScreenRTV->GetClearValue().Color, 0, nullptr);
	// DSVをクリア
	m_pCommandList->ClearDepthStencilView(dsvHandlePtr, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
#endif
}

void Engine::Draw()
{
#if 0
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_pOffScreenRTV->Resource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	m_pCommandList->ResourceBarrier(1, &barrier);
#endif
}

void Engine::EndDraw()
{
#if 0
	// バックバッファとオフスクリーンの両方を PRESENT に戻す
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_pCurrentRenderTarget.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);

	CD3DX12_RESOURCE_BARRIER offScreenBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_pOffScreenRTV->Resource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);

	CD3DX12_RESOURCE_BARRIER barriers[] = { barrier, offScreenBarrier };
	m_pCommandList->ResourceBarrier(_countof(barriers), barriers);
#endif
	// Renderクラスの描画終了処理を呼び出す
	Render::GetInstance().EndDraw();

	// ビューポートとシザー矩形の設定
	m_pCommandList->RSSetViewports(1, &m_Viewport); // ビューポートの設定
	m_pCommandList->RSSetScissorRects(1, &m_scissor); // シザー矩形の設定

	// 現在のRTVを更新
	m_pCurrentRenderTarget = m_pRenderTargets[m_CurrentBackBufferIndex].Get();
		
	// RTVのディスクリプタヒープの開始アドレスを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pRtvHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += m_CurrentBackBufferIndex * m_rtvDescriptorSize;	// 現在のRTVのアドレスを計算

	// DSVのディスクリプタヒープの開始アドレスを取得
	//D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_pDsvHeap->GetCPUDescriptorHandleForHeapStart();

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_pCurrentRenderTarget.Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_pCommandList->ResourceBarrier(1, &barrier);

	// RTV設定
	m_pCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	// RTVをクリア
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f }; // クリアカラー（青色）
	m_pCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// バックバッファを画面に描画
	Render::GetInstance().DrawBackBuffer();

	barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_pCurrentRenderTarget.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);

	m_pCommandList->ResourceBarrier(1, &barrier);

	// コマンドリストのクローズ
	m_pCommandList->Close();

	// コマンドリストの実行
	ID3D12CommandList* ppCommandLists[] = { m_pCommandList.Get() };
	m_pQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// スワップチェーンの画面表示
	m_pSwapChain->Present(0, 0);

	// 描画完了を待つ
	WaitRender();

	// 次のバックバッファインデックスを取得
	m_CurrentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
}

Engine::~Engine()
{
#ifdef _DEBUG
	ComPtr<ID3D12DebugDevice> debugDevice;
	HRESULT hr = m_pDevice.As(&debugDevice);	// ID3D12DebugDeviceの取得

	if (SUCCEEDED(hr))
	{
		debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL); // ライブオブジェクトの詳細をレポート
	}

#endif

	if (m_hFenceEvent)
	{
		CloseHandle(m_hFenceEvent);
		m_hFenceEvent = nullptr;
	}

	if (m_pOffScreenRTV)
	{
		delete m_pOffScreenRTV;
		m_pOffScreenRTV = nullptr;
	}
}

HRESULT Engine::CreateDevice()
{
	HRESULT hr = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_11_0,						// 使用する機能レベル
		IID_PPV_ARGS(m_pDevice.GetAddressOf()));	// デバイスのポインタ

	m_pDevice->SetName(L"Device");

	return hr;
}

HRESULT Engine::CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC desc{};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // コマンドリストのタイプ
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; // 優先度
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; // フラグ（なし）
	desc.NodeMask = 0; // ノードマスク（単一ノード）

	HRESULT hr = m_pDevice->CreateCommandQueue(
		&desc,								// コマンドキューの設定
		IID_PPV_ARGS(m_pQueue.GetAddressOf())); // コマンドキューのポインタ

	m_pQueue->SetName(L"CommandQueue");

	return hr;
}

HRESULT Engine::CreateSwapChain()
{
	// DXGIファクトリーの生成
	IDXGIFactory4* pFactory = nullptr;
	HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "DXGIファクトリーの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

	// スワップチェーンの設定
	DXGI_SWAP_CHAIN_DESC desc{};
	desc.BufferDesc.Width = Window::GetInstance().GetWidth();		// スワップチェーンの幅
	desc.BufferDesc.Height = Window::GetInstance().GetHeight();	// スワップチェーンの高さ
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
	IDXGISwapChain* pSwapChain = nullptr;
	hr = pFactory->CreateSwapChain(
		m_pQueue.Get(),				// コマンドキュー
		&desc,					// スワップチェーンの設定
		&pSwapChain);				// スワップチェーンのポインタ
	if (FAILED(hr))
	{
		pFactory->Release();
		return E_FAIL; // エラー終了
	}

	// IDXGISwapChain3を取得
	hr = pSwapChain->QueryInterface(IID_PPV_ARGS(m_pSwapChain.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		pFactory->Release();
		pSwapChain->Release();
		MessageBox(nullptr, "IDXGISwapChain3の取得に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

	// バックバッファ番号を取得
	m_CurrentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	pFactory->Release();
	pSwapChain->Release();
	return S_OK;
}

HRESULT Engine::CreateCommandList()
{
	HRESULT hr;

	// ==============================
	//	コマンドアロケーターの生成
	// ==============================
	for (uint32_t i = 0; i < m_unFrameCount; ++i)
	{
		hr = m_pDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,					// コマンドリストのタイプ
			IID_PPV_ARGS(m_pAllocator[i].GetAddressOf()));	// コマンドアロケーターのポインタ
		if (FAILED(hr))
		{
			MessageBox(nullptr, "コマンドアロケーターの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return E_FAIL; // エラー終了
		}

		m_pAllocator[i]->SetName(L"CommandAllocator");
	}

	// ==============================
	//	コマンドリストの生成
	// ==============================
	hr = m_pDevice->CreateCommandList(
		0,												// ノードマスク（単一ノード）
		D3D12_COMMAND_LIST_TYPE_DIRECT,					// コマンドリストのタイプ
		m_pAllocator[m_CurrentBackBufferIndex].Get(),	// コマンドアロケーター
		nullptr,										// 初期パイプラインステートオブジェクト
		IID_PPV_ARGS(m_pCommandList.GetAddressOf()));	// コマンドリストのポインタ

	if (FAILED(hr))
	{
		MessageBox(nullptr, "コマンドリストの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

	m_pCommandList->SetName(L"CommandList");

	// コマンドリストは生成直後に記録状態になるので、閉じておく
	hr = m_pCommandList->Close();

	return S_OK;
}

HRESULT Engine::CreateFence()
{
	HRESULT hr = m_pDevice->CreateFence(
		0,													// フェンスの初期値
		D3D12_FENCE_FLAG_NONE,								// フラグ（なし）
		IID_PPV_ARGS(m_pFence.ReleaseAndGetAddressOf()));	// フェンスのポインタ

	if (FAILED(hr))
	{
		MessageBox(nullptr, "フェンスの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

	m_pFence->SetName(L"Fence");

	m_fenceValue[m_CurrentBackBufferIndex]++; // フェンスの値をインクリメント

	// ==============================
	//	フェンスイベントの生成
	// ==============================
	m_hFenceEvent = CreateEvent(
		nullptr,	// デフォルトのセキュリティ属性
		FALSE,		// 手動リセットイベントフラグ
		FALSE,		// イベントの初期状態
		nullptr);	// 名前付きオブジェクト
	if (m_hFenceEvent == nullptr)
	{
		MessageBox(nullptr, "フェンスイベントの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

	return S_OK;
}

void Engine::CreateViewPort()
{
	m_Viewport.TopLeftX = 0.0f;													// ビューポートの左上X座標
	m_Viewport.TopLeftY = 0.0f;													// ビューポートの左上Y座標
	m_Viewport.Width = static_cast<float>(Window::GetInstance().GetWidth());	// ビューポートの幅
	m_Viewport.Height = static_cast<float>(Window::GetInstance().GetHeight());	// ビューポートの高さ
	m_Viewport.MinDepth = 0.0f;													// ビューポートの最小深度
	m_Viewport.MaxDepth = 1.0f;													// ビューポートの最大深度
}

void Engine::CreateScissorRect()
{
	m_scissor.left = 0;										// シザー矩形の左端
	m_scissor.right = Window::GetInstance().GetWidth();		// シザー矩形の右端
	m_scissor.top = 0;										// シザー矩形の上端
	m_scissor.bottom = Window::GetInstance().GetHeight();	// シザー矩形の下端
}

HRESULT Engine::CreateRenderTargetView()
{
	// ==============================
	//	RTVディスクリプタヒープの生成
	// ==============================
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.NumDescriptors = m_unFrameCount;			// ディスクリプタ数
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;		// ヒープのタイプ
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// フラグ（なし）
	HRESULT hr = m_pDevice->CreateDescriptorHeap(
		&desc,												// ヒープの設定
		IID_PPV_ARGS(m_pRtvHeap.ReleaseAndGetAddressOf()));	// ヒープのポインタ
	if (FAILED(hr))
	{
		MessageBox(nullptr, "RTVディスクリプタヒープの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

	m_pRtvHeap->SetName(L"RTVHeap");

	m_rtvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV); // RTVディスクリプタサイズを取得

	// ==============================
	//	RTVの生成
	// ==============================
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pRtvHeap->GetCPUDescriptorHandleForHeapStart(); // ヒープの先頭ハンドルを取得
	for (uint32_t i = 0; i < m_unFrameCount; ++i)
	{
		hr = m_pSwapChain->GetBuffer(
			i,															// バックバッファ番号
			IID_PPV_ARGS(m_pRenderTargets[i].ReleaseAndGetAddressOf())); // レンダーターゲットのポインタ
		if (FAILED(hr))
		{
			MessageBox(nullptr, "レンダーターゲットの取得に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return E_FAIL; // エラー終了
		}

		m_pDevice->CreateRenderTargetView(
			m_pRenderTargets[i].Get(),	// レンダーターゲット
			nullptr,					// RTVの設定（デフォルト）
			rtvHandle);

		rtvHandle.ptr += m_rtvDescriptorSize; // 次のハンドルへ
	}

	return S_OK;
}

HRESULT Engine::CreateDepthStencilView()
{
	// ==============================
	//	DSVディスクリプタヒープの生成
	// ==============================
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.NumDescriptors = 1;						// ディスクリプタ数
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;		// ヒープのタイプ
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// フラグ（なし）
	HRESULT hr = m_pDevice->CreateDescriptorHeap(
		&desc,						// ヒープの設定
		IID_PPV_ARGS(&m_pDsvHeap));	// ヒープのポインタ
	if (FAILED(hr))
	{
		MessageBox(nullptr, "DSVディスクリプタヒープの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

	m_pDsvHeap->SetName(L"DSVHeap");

	m_dsvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV); // DSVディスクリプタサイズを取得

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = DXGI_FORMAT_D32_FLOAT;	// フォーマット
	clearValue.DepthStencil.Depth = 1.0f;		// 深度のクリア値
	clearValue.DepthStencil.Stencil = 0;		// ステンシルのクリア値

	// ==============================
	//	DSVの生成
	// ==============================
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT); // ヒープのプロパティ
	CD3DX12_RESOURCE_DESC resc(
		D3D12_RESOURCE_DIMENSION_TEXTURE2D, // リソースの設定
		0,									// アライメント
		Window::GetInstance().GetWidth(),	// 幅
		Window::GetInstance().GetHeight(),	// 高さ
		1,									// 深さ
		1,									// ミップ数
		DXGI_FORMAT_D32_FLOAT,				// フォーマット
		1,									// サンプル数
		0,									// サンプル品質
		D3D12_TEXTURE_LAYOUT_UNKNOWN,		// テクスチャのレイアウト
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);
	hr = m_pDevice->CreateCommittedResource(
		&heapProp,							// ヒープのプロパティ
		D3D12_HEAP_FLAG_NONE,				// ヒープのフラグ
		&resc,								// リソースの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	// リソースの初期状態
		&clearValue,						// 最適化されたクリア値
		IID_PPV_ARGS(m_pDepthStencil.ReleaseAndGetAddressOf())); // 深度ステンシルバッファのポインタ

	m_pDepthStencil->SetName(L"DepthStencilBuffer");

	if (FAILED(hr))
	{
		MessageBox(nullptr, "深度ステンシルバッファの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL; // エラー終了
	}

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_pDsvHeap->GetCPUDescriptorHandleForHeapStart(); // ヒープの先頭ハンドルを取得

	m_pDevice->CreateDepthStencilView(
		m_pDepthStencil.Get(),	// 深度ステンシルバッファ
		nullptr,				// DSVの設定（デフォルト）
		dsvHandle);				// DSVハンドル

	return S_OK;
}

void Engine::WaitRender()
{
	const UINT64 fence = m_fenceValue[m_CurrentBackBufferIndex];
	m_pQueue->Signal(m_pFence.Get(), fence); // コマンドキューにシグナルを送る
	m_fenceValue[m_CurrentBackBufferIndex]++; // フェンスの値をインクリメント

	if (m_pFence->GetCompletedValue() < fence) // GPUが処理を終えていない場合
	{
		// 完了時にイベントを設定
		auto hr = m_pFence->SetEventOnCompletion(fence, m_hFenceEvent); // フェンスの値に達したらイベントをセット
		if (FAILED(hr))
		{
			MessageBox(nullptr, "フェンスのイベント設定に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return; // エラー終了
		}

		// 待機処理
		if (WaitForSingleObjectEx(m_hFenceEvent, INFINITE, FALSE) != WAIT_OBJECT_0)
		{
			MessageBox(nullptr, "フェンスのイベント待機に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return; // エラー終了
		}
	}
}
