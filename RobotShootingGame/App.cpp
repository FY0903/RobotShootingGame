/*+===================================================================
	File: App.cpp
	Summary: ウィンドウの作成をするソースファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/19 13:15 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "App.hpp"

// ==============================
//	constexpr
// ==============================
constexpr const char* ClassName = "RobotShootingGameClass";

// ==============================
//	template
// ==============================
template <typename T>
inline void SafeRelease(T*& p)
{
	if (p)
	{
		p->Release();
		p = nullptr;
	}
}

App::App(uint32_t width, uint32_t height)
{
	m_unWidth = width;
	m_unHeight = height;
}

void App::Run()
{
	if (InitApp()) MainLoop();

	TermApp();
}

bool App::InitApp()
{
	// ウィンドウの初期化
	if (!InitWnd()) return false;

	return true;	// 正常終了
}

void App::TermApp()
{
	// ウィンドウの終了処理
	TermWnd();
}

bool App::InitWnd()
{
	// インスタンスハンドルを取得
	HINSTANCE hIst = GetModuleHandle(nullptr);
	if (!hIst) return false;

	// ウィンドウの設定
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(WNDCLASSEX);							// 構造体のサイズ
	wc.style = CS_HREDRAW | CS_VREDRAW;						// ウィンドウのスタイル
	wc.lpfnWndProc = WndProc;								// ウィンドウプロシージャのポインタ
	wc.hIcon = LoadIcon(hIst, IDI_APPLICATION);				// アイコンのハンドル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);			// カーソルのハンドル
	wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);	// 背景ブラシのハンドル
	wc.lpszMenuName = nullptr;								// メニューの名前（nullptrならメニューなし）
	wc.lpszClassName = ClassName;							// クラス名
	wc.hIconSm = LoadIcon(hIst, IDI_APPLICATION);			// 小さいアイコンのハンドル

	// ウィンドウの登録
	if (!RegisterClassEx(&wc))
	{
		MessageBox(nullptr, "ウィンドウの登録に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false;
	}

	// インスタンスハンドル設定
	m_hInst = hIst;

	// ウィンドウサイズを調整
	RECT rect{};
	rect.right = static_cast<LONG>(m_unWidth);
	rect.bottom = static_cast<LONG>(m_unHeight);

	// ウィンドウサイズを調整
	long style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	AdjustWindowRect(&rect, style, FALSE);

	// ウィンドウを生成
	m_hWnd = CreateWindowEx(
		0,								// 拡張スタイル（なし）
		ClassName,						// クラス名
		TEXT("Robot Shooting Game"),	// ウィンドウのタイトル
		style,							// ウィンドウのスタイル
		CW_USEDEFAULT,					// X座標（デフォルト）
		CW_USEDEFAULT,					// Y座標（デフォルト）
		rect.right - rect.left,			// 幅
		rect.bottom - rect.top,			// 高さ
		nullptr,						// 親ウィンドウ（nullptrならトップレベルウィンドウ）
		nullptr,						// メニューハンドル（nullptrならメニューなし）
		m_hInst,						// インスタンスハンドル
		nullptr);						// 追加のパラメータ（nullptrならなし）

	if (!m_hWnd)
	{
		MessageBox(nullptr, "ウィンドウの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false;
	}

	// ウィンドウを表示
	ShowWindow(m_hWnd, SW_SHOWNORMAL);

	// ウィンドウの更新
	UpdateWindow(m_hWnd);

	// ウィンドウにフォーカスを設定
	SetFocus(m_hWnd);

	return true;	// 正常終了
}

void App::TermWnd()
{
	// ウィンドウの登録を解除
	if (m_hInst) UnregisterClass(ClassName, m_hInst);

	m_hInst = nullptr;
	m_hWnd = nullptr;
}

void App::MainLoop()
{
	MSG msg{};
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

bool App::InitD3D()
{
	// ==============================
	//	デバイスの生成
	// ==============================
	HRESULT hr = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_11_0,		// 使用する機能レベル
		IID_PPV_ARGS(&m_pDevice));	// デバイスのポインタ
	
	if (FAILED(hr))
	{
		MessageBox(nullptr, "Direct3D 12デバイスの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// ==============================
	//	コマンドキューの生成
	// ==============================
	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // コマンドリストのタイプ
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; // 優先度
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; // フラグ（なし）
	queueDesc.NodeMask = 0; // ノードマスク（単一ノード）

	hr = m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pQueue));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "コマンドキューの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// ==============================
	//	スワップチェーンの生成
	// ==============================
	// DXGIファクトリーの生成
	IDXGIFactory4* pFactory = nullptr;
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "DXGIファクトリーの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// スワップチェーンの設定
	DXGI_SWAP_CHAIN_DESC swapDesc{};
	swapDesc.BufferDesc.Width = m_unWidth;		// スワップチェーンの幅
	swapDesc.BufferDesc.Height = m_unHeight;	// スワップチェーンの高さ
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;	// リフレッシュレートの分子（60Hz）
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;	// リフレッシュレートの分母
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // スキャンラインの順序
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // スケーリングの指定
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // バッファのフォーマット
	swapDesc.SampleDesc.Count = 1; // マルチサンプリングのサンプル数
	swapDesc.SampleDesc.Quality = 0; // マルチサンプリングの品質
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // バッファの使用法
	swapDesc.BufferCount = FrameCount; // バッファの数
	swapDesc.OutputWindow = m_hWnd; // 出力ウィンドウ
	swapDesc.Windowed = TRUE; // ウィンドウモード
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // スワップ効果
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // モードスイッチを許可

	// スワップチェーンの生成
	IDXGISwapChain* pSwapChain = nullptr;
	hr = pFactory->CreateSwapChain(
		m_pQueue,					// コマンドキュー
		&swapDesc,					// スワップチェーンの設定
		&pSwapChain);				// スワップチェーンのポインタ
	if (FAILED(hr))
	{
		SafeRelease(pFactory);
		return false; // エラー終了
	}

	// IDXGISwapChain3を取得
	hr = pSwapChain->QueryInterface(IID_PPV_ARGS(&m_pSwapChain));
	if (FAILED(hr))
	{
		SafeRelease(pSwapChain);
		SafeRelease(pFactory);
		MessageBox(nullptr, "IDXGISwapChain3の取得に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// バックバッファ番号を取得
	m_unFrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	// スワップチェーンのリソースを解放
	SafeRelease(pFactory);
	SafeRelease(pSwapChain);

	// ==============================
	//	コマンドアロケーターの生成
	// ==============================
	for (uint32_t i = 0; i < FrameCount; ++i)
	{
		hr = m_pDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT, // コマンドリストのタイプ
			IID_PPV_ARGS(&m_pCmdAllocater[i])); // コマンドアロケーターのポインタ
		if (FAILED(hr))
		{
			MessageBox(nullptr, "コマンドアロケーターの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return false; // エラー終了
		}
	}

	// ==============================
	//	コマンドリストの生成
	// ==============================
	hr = m_pDevice->CreateCommandList(
		0,								// コマンドリストのフラグ
		D3D12_COMMAND_LIST_TYPE_DIRECT, // コマンドリストのタイプ
		m_pCmdAllocater[m_unFrameIndex], // コマンドアロケーター
		nullptr,						// パイプラインステートオブジェクト（nullptrならデフォルト）
		IID_PPV_ARGS(&m_pCmdList));	// コマンドリストのポインタ
	if (FAILED(hr))
	{
		MessageBox(nullptr, "コマンドリストの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// ===============================
	//	レンダーターゲットビューの生成
	// ===============================
	// ディスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.NumDescriptors = FrameCount; // フレーム数分のディスクリプタを確保
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // レンダーターゲットビュー用
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; // フラグ（なし）
	rtvHeapDesc.NodeMask = 0; // ノードマスク（単一ノード）

	hr = m_pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_pHeapRTV));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "レンダーターゲットビューの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pHeapRTV->GetCPUDescriptorHandleForHeapStart();
	UINT incrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for (uint32_t i = 0; i < FrameCount; ++i)
	{
		// バックバッファのリソースを取得
		hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pColorBuffer[i]));
		if (FAILED(hr))
		{
			MessageBox(nullptr, "バックバッファの取得に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return false; // エラー終了
		}

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // レンダーターゲットのフォーマット
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // ビューの次元
		rtvDesc.Texture2D.MipSlice = 0; // ミップスライス
		rtvDesc.Texture2D.PlaneSlice = 0; // プレーンスライス

		// レンダーターゲットビューの生成
		m_pDevice->CreateRenderTargetView(
			m_pColorBuffer[i],			// バックバッファのリソース
			&rtvDesc,					// レンダーターゲットビューの設定
			rtvHandle);				// レンダーターゲットビューのハンドル

		m_hRTV[i] = rtvHandle; // レンダーターゲットビューのハンドルを保存
		rtvHandle.ptr += incrementSize; // 次のハンドルへ移動
	}

	// ==============================
	//	フェンスの生成
	// ==============================
	// フェンスカウンターをリセット
	for (uint32_t i = 0; i < FrameCount; ++i) m_unFrameCounter[i] = 0;
	
	// フェンスの生成
	hr = m_pDevice->CreateFence(
		m_unFrameCounter[m_unFrameIndex], // 初期値
		D3D12_FENCE_FLAG_NONE, // フェンスのフラグ（なし）
		IID_PPV_ARGS(&m_pFence)); // フェンスのポインタ
	if (FAILED(hr))
	{
		MessageBox(nullptr, "フェンスの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	m_unFrameCounter[m_unFrameIndex]++; // フレームカウンターをインクリメント

	// イベントの生成
	m_hFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!m_hFenceEvent)
	{
		MessageBox(nullptr, "フェンスイベントの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// コマンドリストを閉じる
	m_pCmdList->Close();

	return true; // 正常終了
}

void App::TermD3D()
{
}

void App::Render()
{
}

void App::WaitGPU()
{
}

void App::Present(uint32_t interval)
{
}

LRESULT App::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0); // アプリケーションの終了
		return 0;
	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wp, lp);
}
