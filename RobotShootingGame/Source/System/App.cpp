/*+===================================================================
	File: App.cpp
	Summary: ウィンドウの作成をするソースファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/19 13:15 初回作成
			  /07/19 18:56 D3D12の初期化を追加
			  /07/20 22:53 描画処理の追加
			  /07/21 00:18 リファクタリング
			  /07/22 05:03 描画初期化処理を追加
			  /07/22 16:48 深度ステンシルビューの追加
===================================================================+*/

// ==============================
//	include
// ==============================
#include "App.hpp"
#include <cassert>

// ==============================
//	constexpr
// ==============================
constexpr const char* ClassName = "RobotShootingGameClass";

// ==============================
//	structs
// ==============================
struct Vertex
{
	DirectX::XMFLOAT3 Position; // 頂点の位置
	DirectX::XMFLOAT4 Color;    // 頂点の色
};

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

	// D3Dの初期化
	if (!InitD3D()) return false;

	if (!OnInit()) return false;

	return true;	// 正常終了
}

void App::TermApp()
{
	OnTerm();

	// D3Dの終了処理
	TermD3D();

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
		else
		{
			Render();
		}
	}
}

bool App::InitD3D()
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

	hr = m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_pQueue.GetAddressOf()));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "コマンドキューの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
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
	ComPtr<IDXGISwapChain> pSwapChain = nullptr;
	hr = pFactory->CreateSwapChain(
		m_pQueue.Get(),				// コマンドキュー
		&swapDesc,					// スワップチェーンの設定
		pSwapChain.GetAddressOf());				// スワップチェーンのポインタ
	if (FAILED(hr))
	{
		return false; // エラー終了
	}

	// IDXGISwapChain3を取得
	hr = pSwapChain.As(&m_pSwapChain); // IDXGISwapChain3のスマートポインタにキャスト
	if (FAILED(hr))
	{
		MessageBox(nullptr, "IDXGISwapChain3の取得に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// バックバッファ番号を取得
	m_unFrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	// 不要になったので解放
	pFactory.Reset(); // DXGIファクトリーのスマートポインタをリセット
	pSwapChain.Reset(); // IDXGISwapChainのスマートポインタをリセット

	// ==============================
	//	コマンドアロケーターの生成
	// ==============================
	for (uint32_t i = 0; i < FrameCount; ++i)
	{
		hr = m_pDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,						// コマンドリストのタイプ
			IID_PPV_ARGS(m_pCmdAllocater[i].GetAddressOf()));	// コマンドアロケーターのポインタ
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
		0,											// コマンドリストのフラグ
		D3D12_COMMAND_LIST_TYPE_DIRECT,				// コマンドリストのタイプ
		m_pCmdAllocater[m_unFrameIndex].Get(),		// コマンドアロケーター
		nullptr,									// パイプラインステートオブジェクト（nullptrならデフォルト）
		IID_PPV_ARGS(m_pCmdList.GetAddressOf()));	// コマンドリストのポインタ
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

	hr = m_pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_pHeapRTV.GetAddressOf()));
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
		hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(m_pColorBuffer[i].GetAddressOf()));
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
			m_pColorBuffer[i].Get(),	// バックバッファのリソース
			&rtvDesc,					// レンダーターゲットビューの設定
			rtvHandle);					// レンダーターゲットビューのハンドル

		m_hRTV[i] = rtvHandle; // レンダーターゲットビューのハンドルを保存
		rtvHandle.ptr += incrementSize; // 次のハンドルへ移動
	}

	// ==============================
	//	深度ステンシルビューの生成
	// ==============================
	D3D12_HEAP_PROPERTIES prop{};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT; // ヒープのタイプ（デフォルト）
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN; // CPUページプロパティ
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN; // メモリプールの設定
	prop.CreationNodeMask = 1; // 作成ノードマスク
	prop.VisibleNodeMask = 1; // 可視ノードマスク

	D3D12_RESOURCE_DESC depthDesc{};
	depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // リソースの次元（2Dテクスチャ）
	depthDesc.Alignment = 0; // アライメント（0ならデフォルト）
	depthDesc.Width = m_unWidth; // 幅
	depthDesc.Height = m_unHeight; // 高さ
	depthDesc.DepthOrArraySize = 1; // 深度またはアレイサイズ
	depthDesc.MipLevels = 1; // ミップレベル
	depthDesc.Format = DXGI_FORMAT_D32_FLOAT; // フォーマット（深度ステンシル用）
	depthDesc.SampleDesc.Count = 1; // マルチサンプリングのサンプル数
	depthDesc.SampleDesc.Quality = 0; // マルチサンプリングの品質
	depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // テクスチャのレイアウト（未知）
	depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // リソースフラグ（深度ステンシルを許可）

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = DXGI_FORMAT_D32_FLOAT; // 深度ステンシルのフォーマット
	clearValue.DepthStencil.Depth = 1.0f; // 深度のクリア値
	clearValue.DepthStencil.Stencil = 0; // ステンシルのクリア値

	hr = m_pDevice->CreateCommittedResource(
		&prop,								// ヒーププロパティ
		D3D12_HEAP_FLAG_NONE,				// ヒープフラグ（なし）
		&depthDesc,							// リソースの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	// 初期状態（深度書き込み）
		&clearValue,						// クリア値
		IID_PPV_ARGS(m_pDepthBuffer.GetAddressOf())); // 深度バッファのポインタ
	if (FAILED(hr))
	{
		MessageBox(nullptr, "深度ステンシルビューの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// ディスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1; // 深度ステンシルビューの数
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; // 深度ステンシルビュー用
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; // フラグ（なし）
	dsvHeapDesc.NodeMask = 0; // ノードマスク（単一ノード）

	hr = m_pDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_pHeapDSV.GetAddressOf()));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "深度ステンシルビューのディスクリプタヒープの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_pHeapDSV->GetCPUDescriptorHandleForHeapStart(); // 深度ステンシルビューのハンドルを取得
	incrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV); // 深度ステンシルビューのインクリメントサイズを取得

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // 深度ステンシルビューのフォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // ビューの次元（2Dテクスチャ）
	dsvDesc.Texture2D.MipSlice = 0; // ミップスライス
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE; // フラグ（なし）

	// 深度ステンシルビューの生成
	m_pDevice->CreateDepthStencilView(
		m_pDepthBuffer.Get(),				// 深度バッファのリソース
		&dsvDesc,							// 深度ステンシルビューの設定
		dsvHandle);							// 深度ステンシルビューのハンドル

	m_hDSV = dsvHandle; // 深度ステンシルビューのハンドルを保存

	// ==============================
	//	フェンスの生成
	// ==============================
	// フェンスカウンターをリセット
	for (uint32_t i = 0; i < FrameCount; ++i) m_unFenceCounter[i] = 0;
	
	// フェンスの生成
	hr = m_pDevice->CreateFence(
		m_unFenceCounter[m_unFrameIndex],		// 初期値
		D3D12_FENCE_FLAG_NONE,					// フェンスのフラグ（なし）
		IID_PPV_ARGS(m_pFence.GetAddressOf())); // フェンスのポインタ
	if (FAILED(hr))
	{
		MessageBox(nullptr, "フェンスの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	m_unFenceCounter[m_unFrameIndex]++; // フレームカウンターをインクリメント

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
	// GPU処理の完了を待機
	WaitGPU();

	// イベント破棄
	if (m_hFenceEvent)
	{
		CloseHandle(m_hFenceEvent);
		m_hFenceEvent = nullptr;
	}

	// フェンス破棄
	m_pFence.Reset(); // フェンスのスマートポインタをリセット

	// レンダーターゲットビューの破棄
	m_pHeapRTV.Reset(); // レンダーターゲットビューのスマートポインタをリセット
	for (uint32_t i = 0; i < FrameCount; ++i) m_pColorBuffer[i].Reset(); // バックバッファのリソースを解放

	// コマンドリストの破棄
	m_pCmdList.Reset();

	// コマンドアロケーターの破棄
	for (uint32_t i = 0; i < FrameCount; ++i) m_pCmdAllocater[i].Reset(); // コマンドアロケーターを解放

	// スワップチェーンの破棄
	m_pSwapChain.Reset();

	// コマンドキューの破棄
	m_pQueue.Reset();

	// デバイスの破棄
	m_pDevice.Reset();
}

void App::Render()
{
	// ==============================
	//	更新処理
	// ==============================
	m_fRotateAngle += 0.025f;
	m_CBV[m_unFrameIndex * 2 + 0].pBuffer->World =
		DirectX::XMMatrixRotationZ(m_fRotateAngle + DirectX::XMConvertToRadians(45.0f));
	m_CBV[m_unFrameIndex * 2 + 1].pBuffer->World =
		DirectX::XMMatrixRotationY(m_fRotateAngle) * DirectX::XMMatrixScaling(2.0f, 0.5f, 1.0f);

	// コマンドの記録を開始
	m_pCmdAllocater[m_unFrameIndex]->Reset(); // コマンドアロケーターをリセット
	m_pCmdList->Reset(m_pCmdAllocater[m_unFrameIndex].Get(), nullptr); // コマンドリストをリセット

	// リソースバリアの設定
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; // リソースバリアのタイプ
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE; // フラグ（なし）
	barrier.Transition.pResource = m_pColorBuffer[m_unFrameIndex].Get(); // 対象のリソース
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // 前の状態
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 後の状態
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; // 全サブリソース

	// リソースバリア
	m_pCmdList->ResourceBarrier(1, &barrier);

	// レンダーターゲットの設定
	m_pCmdList->OMSetRenderTargets(
		1,								// レンダーターゲットビューの数
		&m_hRTV[m_unFrameIndex],		// レンダーターゲットビューのハンドル
		FALSE,							// レンダーターゲットビューをクリアしない
		&m_hDSV);						// 深度ステンシルビューのハンドル

	// クリアカラーの設定
	float clearColor[4] = { 0.25f, 0.25f, 0.25f, 1.0f };

	// レンダーターゲットビューをクリア
	m_pCmdList->ClearRenderTargetView(
		m_hRTV[m_unFrameIndex],		// レンダーターゲットビューのハンドル
		clearColor,					// クリアカラー
		0,							// 深度ステンシルビューの数（なし）
		nullptr);					// 深度ステンシルビューのハンドル（なし）

	// 深度ステンシルビューをクリア
	m_pCmdList->ClearDepthStencilView(
		m_hDSV,						// 深度ステンシルビューのハンドル
		D3D12_CLEAR_FLAG_DEPTH,		// クリアフラグ（深度のみ）
		1.0f,						// 深度のクリア値
		0,							// ステンシルのクリア値
		0,							// 深度ステンシルビューの数（なし）
		nullptr);					// 深度ステンシルビューのハンドル（なし）

	// ===============================
	//	描画処理
	// ===============================
	m_pCmdList->SetGraphicsRootSignature(m_pRootSignature.Get()); // ルートシグネチャの設定
	m_pCmdList->SetDescriptorHeaps(1, m_pHeapCBV.GetAddressOf()); // ディスクリプタヒープの設定
	m_pCmdList->SetPipelineState(m_pPSO.Get()); // パイプラインステートオブジェクトの設定

	m_pCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // プリミティブトポロジの設定
	m_pCmdList->IASetVertexBuffers(0, 1, &m_VBV); // 頂点バッファの設定
	m_pCmdList->IASetIndexBuffer(&m_IBV); // インデックスバッファの設定
	m_pCmdList->RSSetViewports(1, &m_Viewport); // ビューポートの設定
	m_pCmdList->RSSetScissorRects(1, &m_Scissor); // シザーレクトの設定

	// 手前側の三角形を描画
	m_pCmdList->SetGraphicsRootConstantBufferView(
		0, // ルートパラメータのインデックス
		m_CBV[m_unFrameIndex * 2 + 0].Desc.BufferLocation); // 定数バッファビューの設定

	m_pCmdList->DrawIndexedInstanced(
		6,	// インデックスの数
		1,  // インスタンスの数
		0,  // インデックスの開始位置
		0,  // 頂点の開始位置
		0); // インスタンスの開始位置

	// 奥側の三角形を描画
	m_pCmdList->SetGraphicsRootConstantBufferView(
		0, // ルートパラメータのインデックス
		m_CBV[m_unFrameIndex * 2 + 1].Desc.BufferLocation); // 定数バッファビューの設定

	m_pCmdList->DrawIndexedInstanced(
		6,	// インデックスの数
		1,  // インスタンスの数
		0,  // インデックスの開始位置
		0,  // 頂点の開始位置
		0); // インスタンスの開始位置

	// リソースバリアの設定
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; // リソースバリアのタイプ
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE; // フラグ（なし）
	barrier.Transition.pResource = m_pColorBuffer[m_unFrameIndex].Get(); // 対象のリソース
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 前の状態
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // 後の状態
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; // 全サブリソース

	// リソースバリア
	m_pCmdList->ResourceBarrier(1, &barrier);

	// コマンドの記録を終了
	m_pCmdList->Close();

	// コマンドを実行
	ID3D12CommandList* ppCommandLists[] = { m_pCmdList.Get()};
	m_pQueue->ExecuteCommandLists(1, ppCommandLists);

	// 画面に表示
	Present(1);
}

void App::WaitGPU()
{
	assert(m_pQueue != nullptr);
	assert(m_pFence != nullptr);
	assert(m_hFenceEvent != nullptr);

	// シグナル処理
	m_pQueue->Signal(m_pFence.Get(), m_unFenceCounter[m_unFrameIndex]); // コマンドキューをシグナル

	// 完了時にイベントを設定する
	m_pFence->SetEventOnCompletion(m_unFenceCounter[m_unFrameIndex], m_hFenceEvent);

	// 待機処理
	WaitForSingleObjectEx(m_hFenceEvent, INFINITE, FALSE); // フェンスイベントがシグナルされるまで待機

	// カウンターを増やす
	m_unFenceCounter[m_unFrameIndex]++; // 次のフレームのフェンスカウンターを増やす
}

void App::Present(uint32_t interval)
{
	// 画面に表示
	m_pSwapChain->Present(interval, 0); // スワップチェーンをプレゼント

	// シグナル処理
	const uint64_t currentValue = m_unFenceCounter[m_unFrameIndex];
	m_pQueue->Signal(m_pFence.Get(), currentValue); // コマンドキューをシグナル

	// バックバッファ番号を更新
	m_unFrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	// 次のフレームの描画準備がまだであれば待機する
	if (m_pFence->GetCompletedValue() < m_unFenceCounter[m_unFrameIndex])
	{
		m_pFence->SetEventOnCompletion(m_unFenceCounter[m_unFrameIndex], m_hFenceEvent);
		WaitForSingleObjectEx(m_hFenceEvent, INFINITE, FALSE); // フェンスイベントがシグナルされるまで待機
	}

	// 次のフレームのフェンスカウンターを増やす
	m_unFenceCounter[m_unFrameIndex] = currentValue + 1;
}

bool App::OnInit()
{
	// ==============================
	//	頂点バッファの作成
	// ==============================
	// 頂点データ
	Vertex vertices[] = {
		{DirectX::XMFLOAT3(-1.0f,  1.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
		{DirectX::XMFLOAT3( 1.0f,  1.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
		{DirectX::XMFLOAT3( 1.0f, -1.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
		{DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)},
	};

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES prop{};
	prop.Type = D3D12_HEAP_TYPE_UPLOAD;	// ヒープのタイプ（アップロード用）
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// CPUページプロパティ（不明）
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// メモリプールの優先度（不明）
	prop.CreationNodeMask = 1;	// 作成ノードマスク（単一ノード）
	prop.VisibleNodeMask = 1;	// 可視ノードマスク（単一ノード）

	// リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;	// リソースの次元（バッファ）
	resourceDesc.Alignment = 0;	// アライメント（0ならデフォルト）
	resourceDesc.Width = sizeof(vertices);	// リソースの幅（頂点データのサイズ）
	resourceDesc.Height = 1;	// 高さ（1なら1Dバッファ）
	resourceDesc.DepthOrArraySize = 1;	// 深度または配列サイズ（1なら1Dバッファ）
	resourceDesc.MipLevels = 1;	// ミップレベル（1ならミップマップなし）
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;	// フォーマット（バッファなので不明）
	resourceDesc.SampleDesc.Count = 1;	// サンプル数（1ならマルチサンプリングなし）
	resourceDesc.SampleDesc.Quality = 0;	// サンプル品質（0ならデフォルト）
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;	// レイアウト（行メジャー）
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;	// リソースフラグ（なし）

	// リソースを生成
	HRESULT hr = m_pDevice->CreateCommittedResource(
		&prop,								// ヒーププロパティ
		D3D12_HEAP_FLAG_NONE,				// ヒープフラグ（なし）
		&resourceDesc,						// リソースの設定
		D3D12_RESOURCE_STATE_GENERIC_READ,	// 初期状態（汎用読み取り）
		nullptr,							// クリア値（なし）
		IID_PPV_ARGS(m_pVB.GetAddressOf())); // リソースのポインタ
	if (FAILED(hr))
	{
		MessageBox(nullptr, "頂点バッファの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// マッピングする
	void* ptr = nullptr;
	hr = m_pVB->Map(0, nullptr, &ptr); // リソースをマッピングしてポインタを取得
	if (FAILED(hr) || !ptr)
	{
		MessageBox(nullptr, "頂点バッファのマッピングに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// 頂点データーをマッピング先に設定
	memcpy(ptr, vertices, sizeof(vertices)); // 頂点データをコピー

	// マッピング解除
	m_pVB->Unmap(0, nullptr); // マッピングを解除

	// 頂点バッファビューの設定
	m_VBV.BufferLocation = m_pVB->GetGPUVirtualAddress(); // GPU仮想アドレスを設定
	m_VBV.SizeInBytes = static_cast<UINT>(sizeof(vertices)); // サイズを設定
	m_VBV.StrideInBytes = static_cast <UINT>(sizeof(Vertex)); // ストライドを設定（頂点のサイズ）

	// ==============================
	//	インデックスバッファの生成
	// ==============================
	uint32_t indices[] = {
		0, 1, 2, // 三角形1
		0, 2, 3  // 三角形2
	};

	// ヒーププロパティ
	prop.Type = D3D12_HEAP_TYPE_UPLOAD;	// ヒープのタイプ（アップロード用）
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// CPUページプロパティ（不明）
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// メモリプールの優先度（不明）
	prop.CreationNodeMask = 1;	// 作成ノードマスク（単一ノード）
	prop.VisibleNodeMask = 1;	// 可視ノードマスク（単一ノード）

	// リソースの設定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;	// リソースの次元（バッファ）
	resourceDesc.Alignment = 0;	// アライメント（0ならデフォルト）
	resourceDesc.Width = sizeof(indices);	// リソースの幅（インデックスデータのサイズ）
	resourceDesc.Height = 1;	// 高さ（1なら1Dバッファ）
	resourceDesc.DepthOrArraySize = 1;	// 深度または配列サイズ（1なら1Dバッファ）
	resourceDesc.MipLevels = 1;	// ミップレベル（1ならミップマップなし）
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;	// フォーマット（バッファなので不明）
	resourceDesc.SampleDesc.Count = 1;	// サンプル数（1ならマルチサンプリングなし）
	resourceDesc.SampleDesc.Quality = 0;	// サンプル品質（0ならデフォルト）
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;	// レイアウト（行メジャー）
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;	// リソースフラグ（なし）
	
	// リソースを生成
	hr = m_pDevice->CreateCommittedResource(
		&prop,								// ヒーププロパティ
		D3D12_HEAP_FLAG_NONE,				// ヒープフラグ（なし）
		&resourceDesc,						// リソースの設定
		D3D12_RESOURCE_STATE_GENERIC_READ,	// 初期状態（汎用読み取り）
		nullptr,							// クリア値（なし）
		IID_PPV_ARGS(m_pIB.GetAddressOf())); // リソースのポインタ
	if (FAILED(hr))
	{
		MessageBox(nullptr, "インデックスバッファの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// マッピングする
	ptr = nullptr;
	hr = m_pIB->Map(0, nullptr, &ptr); // リソースをマッピングしてポインタを取得
	if (FAILED(hr) || !ptr)
	{
		MessageBox(nullptr, "インデックスバッファのマッピングに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// インデックスデータをマッピング先に設定
	memcpy(ptr, indices, sizeof(indices)); // インデックスデータをコピー

	// マッピング解除
	m_pIB->Unmap(0, nullptr); // マッピングを解除

	// インデックスバッファビューの設定
	m_IBV.BufferLocation = m_pIB->GetGPUVirtualAddress(); // GPU仮想アドレスを設定
	m_IBV.Format = DXGI_FORMAT_R32_UINT; // インデックスフォーマットを設定（32ビット整数）
	m_IBV.SizeInBytes = static_cast<UINT>(sizeof(indices)); // サイズを設定

	// ==============================
	//	定数バッファ用ディスクリプタヒープの作成
	// ==============================
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; // ヒープのタイプ（CBV/SRV/UAV用）
	heapDesc.NumDescriptors = 2 * FrameCount; // ディスクリプタの数（フレーム数分）
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // シェーダーから可視化可能
	heapDesc.NodeMask = 0; // ノードマスク（単一ノード）

	hr = m_pDevice->CreateDescriptorHeap(
		&heapDesc,									// ヒープの設定
		IID_PPV_ARGS(m_pHeapCBV.GetAddressOf()));	// ヒープのポインタ
	if (FAILED(hr))
	{
		MessageBox(nullptr, "定数バッファ用ディスクリプタヒープの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// ===============================
	//	定数バッファの作成
	// ===============================
	// ヒーププロパティ
	prop.Type = D3D12_HEAP_TYPE_UPLOAD;	// ヒープのタイプ（アップロード用）
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// CPUページプロパティ（不明）
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// メモリプールの優先度（不明）
	prop.CreationNodeMask = 1;	// 作成ノードマスク（単一ノード）
	prop.VisibleNodeMask = 1;	// 可視ノードマスク（単一ノード）

	// リソースの設定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;	// リソースの次元（バッファ）
	resourceDesc.Alignment = 0;	// アライメント（0ならデフォルト）
	resourceDesc.Width = sizeof(Transform);	// リソースの幅（Transform構造体のサイズ）
	resourceDesc.Height = 1;	// 高さ（1なら1Dバッファ）
	resourceDesc.DepthOrArraySize = 1;	// 深度または配列サイズ（1なら1Dバッファ）
	resourceDesc.MipLevels = 1;	// ミップレベル（1ならミップマップなし）
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;	// フォーマット（バッファなので不明）
	resourceDesc.SampleDesc.Count = 1;	// サンプル数（1ならマルチサンプリングなし）
	resourceDesc.SampleDesc.Quality = 0;	// サンプル品質（0ならデフォルト）
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;	// レイアウト（行メジャー）
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;	// リソースフラグ（なし）

	UINT incrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV); // ディスクリプタのインクリメントサイズ
	
	for (uint32_t i = 0; i < FrameCount * 2; ++i)
	{
		// リソースを生成
		hr = m_pDevice->CreateCommittedResource(
			&prop,								// ヒーププロパティ
			D3D12_HEAP_FLAG_NONE,				// ヒープフラグ（なし）
			&resourceDesc,								// リソースの設定
			D3D12_RESOURCE_STATE_GENERIC_READ,	// 初期状態（汎用読み取り）
			nullptr,							// クリア値（なし）
			IID_PPV_ARGS(m_pCB[i].GetAddressOf())); // リソースのポインタ
		if (FAILED(hr))
		{
			MessageBox(nullptr, "定数バッファの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return false; // エラー終了
		}

		auto address = m_pCB[i]->GetGPUVirtualAddress(); // GPU仮想アドレスを取得
		auto handleCPU = m_pHeapCBV->GetCPUDescriptorHandleForHeapStart(); // CPUディスクリプタハンドルを取得
		auto handleGPU = m_pHeapCBV->GetGPUDescriptorHandleForHeapStart(); // GPUディスクリプタハンドルを取得

		handleCPU.ptr += incrementSize * i; // CPUディスクリプタハンドルをフレーム数分ずらす
		handleGPU.ptr += incrementSize * i; // GPUディスクリプタハンドルをフレーム数分ずらす

		// 定数バッファビューの設定.
		m_CBV[i].HandleCPU = handleCPU;
		m_CBV[i].HandleGPU = handleGPU;
		m_CBV[i].Desc.BufferLocation = address;
		m_CBV[i].Desc.SizeInBytes = sizeof(Transform);

		m_pDevice->CreateConstantBufferView(
			&m_CBV[i].Desc,	// 定数バッファビューの設定
			handleCPU);		// CPUディスクリプタハンドル

		// マッピング
		hr = m_pCB[i]->Map(0, nullptr, reinterpret_cast<void**>(&m_CBV[i].pBuffer)); // リソースをマッピングしてポインタを取得
		if (FAILED(hr) || !m_CBV[i].pBuffer)
		{
			MessageBox(nullptr, "定数バッファのマッピングに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return false; // エラー終了
		}

		auto eyePos = DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f); // 視点の位置を設定
		auto targetPos = DirectX::XMVectorZero(); // 注視点の位置を設定
		auto upward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // 上方向を設定

		auto fovY = DirectX::XMConvertToRadians(37.5f); // 垂直視野角を設定
		auto aspect = static_cast<float>(m_unWidth) / static_cast<float>(m_unHeight); // アスペクト比を計算

		// 変換行列を計算
		m_CBV[i].pBuffer->World = DirectX::XMMatrixIdentity(); // ワールド行列を単位行列に設定
		m_CBV[i].pBuffer->View = DirectX::XMMatrixLookAtLH(eyePos, targetPos, upward); // ビュー行列を計算
		m_CBV[i].pBuffer->Proj = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, 1.0f, 1000.0f);
	}

	// ==============================
	//	ルートシグネチャの生成
	// ==============================
	auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;	// ルートシグネチャのフラグ（入力アセンブラー入力レイアウトを許可）
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;	// ドメインシェーダーのルートアクセスを拒否
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;	// ハルシェーダーのルートアクセスを拒否
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;	// ジオメトリシェーダーのルートアクセスを拒否

	// ルートパラメーターの設定
	D3D12_ROOT_PARAMETER param{};
	param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // パラメーターのタイプ（CBV）
	param.Descriptor.ShaderRegister = 0; // シェーダーレジスタ
	param.Descriptor.RegisterSpace = 0; // レジスタスペース
	param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // シェーダーの可視性（頂点シェーダー）

	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootDesc{};
	rootDesc.NumParameters = 1; // パラメーターの数
	rootDesc.NumStaticSamplers = 0; // 静的サンプラーの数
	rootDesc.pParameters = &param; // ルートパラメーターの配列
	rootDesc.pStaticSamplers = nullptr; // 静的サンプラーの配列（なし）
	rootDesc.Flags = flag; // ルートシグネチャのフラグ

	ComPtr<ID3DBlob> pBlob = nullptr; // ルートシグネチャのバイナリデータ
	ComPtr<ID3DBlob> pErrorBlob = nullptr; // エラーメッセージ用のバイナリデータ

	// シリアライズ
	hr = D3D12SerializeRootSignature(
		&rootDesc,								// ルートシグネチャの設定
		D3D_ROOT_SIGNATURE_VERSION_1_0,			// バージョン
		pBlob.GetAddressOf(),					// シリアライズされたバイナリデータ
		pErrorBlob.GetAddressOf());				// エラーメッセージ
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			MessageBoxA(nullptr, static_cast<const char*>(pErrorBlob->GetBufferPointer()), "ルートシグネチャのシリアライズエラー", MB_OK | MB_ICONERROR);
		}
		else
		{
			MessageBox(nullptr, "ルートシグネチャのシリアライズに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		}
		return false; // エラー終了
	}

	// ルートシグネチャを生成
	hr = m_pDevice->CreateRootSignature(
		0,									// ノードマスク（単一ノード）
		pBlob->GetBufferPointer(),			// シリアライズされたバイナリデータ
		pBlob->GetBufferSize(),			// バッファのサイズ
		IID_PPV_ARGS(m_pRootSignature.GetAddressOf())); // ルートシグネチャのポインタ
	if (FAILED(hr))
	{
		MessageBox(nullptr, "ルートシグネチャの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// ==============================
	//	パイプラインステートの生成
	// ==============================
	D3D12_INPUT_ELEMENT_DESC elements[2];
	elements[0].SemanticName = "POSITION"; // セマンティック名（位置）
	elements[0].SemanticIndex = 0; // セマンティックインデックス
	elements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT; // フォーマット（3D位置）
	elements[0].InputSlot = 0; // 入力スロット
	elements[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT; // アライメントオフセット（自動計算）
	elements[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA; // 入力クラス（頂点データ）
	elements[0].InstanceDataStepRate = 0; // インスタンスデータステップレート（なし）

	elements[1].SemanticName = "COLOR"; // セマンティック名（色）
	elements[1].SemanticIndex = 0; // セマンティックインデックス
	elements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // フォーマット（RGBA色）
	elements[1].InputSlot = 0; // 入力スロット
	elements[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT; // アライメントオフセット（自動計算）
	elements[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA; // 入力クラス（頂点データ）
	elements[1].InstanceDataStepRate = 0; // インスタンスデータステップレート（なし）

	// ラスタライザーステートの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID; // 塗りつぶしモード（ソリッド）
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE; // カリングモード（なし）
	rasterizerDesc.FrontCounterClockwise = FALSE; // フロントカウンタークロックワイズ（時計回り）
	rasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS; // 深度バイアス（デフォルト）
	rasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP; // 深度バイアスクランプ（デフォルト）
	rasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS; // スロープスケールド深度バイアス（デフォルト）
	rasterizerDesc.DepthClipEnable = FALSE; // 深度クリップを無効化
	rasterizerDesc.MultisampleEnable = FALSE; // マルチサンプリングを無効化
	rasterizerDesc.AntialiasedLineEnable = FALSE; // アンチエイリアスラインを無効化
	rasterizerDesc.ForcedSampleCount = 0; // 強制サンプル数（なし）
	rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF; // 保守的ラスタライゼーションをオフ

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blendDescRT = {
		FALSE, // ブレンドを無効化
		FALSE, // アルファブレンドを無効化
		D3D12_BLEND_ONE, // ソースブレンド（1）
		D3D12_BLEND_ZERO, // デスティネーションブレンド（0）
		D3D12_BLEND_OP_ADD, // ブレンドオペレーション（加算）
		D3D12_BLEND_ONE, // ソースアルファブレンド（1）
		D3D12_BLEND_ZERO, // デスティネーションアルファブレンド（0）
		D3D12_BLEND_OP_ADD, // アルファブレンドオペレーション（加算）
		D3D12_LOGIC_OP_NOOP, // ロジックオペレーション（なし）
		D3D12_COLOR_WRITE_ENABLE_ALL // カラー書き込みを全て有効化
	};

	// ブレンドステートの設定
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.AlphaToCoverageEnable = FALSE; // アルファカバレッジを無効化
	blendDesc.IndependentBlendEnable = FALSE; // 独立ブレンドを無効化
	for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		blendDesc.RenderTarget[i] = blendDescRT; // レンダーターゲットのブレンド設定を適用
	}

	// 深度ステンシルステートの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = TRUE; // 深度ステンシルを有効化
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL; // 深度書き込みマスクを全て有効化
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; // 深度比較関数（小なりイコール）
	depthStencilDesc.StencilEnable = FALSE; // ステンシルを無効化

	ComPtr<ID3DBlob> pVSBlob = nullptr; // 頂点シェーダーのバイナリデータ
	ComPtr<ID3DBlob> pPSBlob = nullptr; // ピクセルシェーダーのバイナリデータ

	// 頂点シェーダー読み込み
	hr = D3DReadFileToBlob(L"SimpleVS.cso", pVSBlob.GetAddressOf()); // 頂点シェーダーのバイナリデータを読み込み
	if (FAILED(hr))
	{
		MessageBox(nullptr, "頂点シェーダーの読み込みに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false;
	}

	// ピクセルシェーダー読み込み
	hr = D3DReadFileToBlob(L"SimplePS.cso", pPSBlob.GetAddressOf()); // ピクセルシェーダーのバイナリデータを読み込み
	if (FAILED(hr))
	{
		MessageBox(nullptr, "ピクセルシェーダーの読み込みに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false;
	}

	// パイプラインステートの設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.InputLayout = { elements, _countof(elements) }; // 入力レイアウトを設定
	psoDesc.pRootSignature = m_pRootSignature.Get(); // ルートシグネチャを設定
	psoDesc.VS = { pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize() }; // 頂点シェーダーを設定
	psoDesc.PS = { pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize() }; // ピクセルシェーダーを設定
	psoDesc.RasterizerState = rasterizerDesc; // ラスタライザーステートを設定
	psoDesc.BlendState = blendDesc; // ブレンドステートを設定
	psoDesc.DepthStencilState = depthStencilDesc; // 深度ステンシルステートを設定
	psoDesc.SampleMask = UINT_MAX; // サンプルマスクを全て有効化
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // プリミティブトポロジタイプを三角形に設定
	psoDesc.NumRenderTargets = 1; // レンダーターゲットの数を1に設定
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // レンダーターゲットのフォーマットを設定
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT; // 深度ステンシルのフォーマットを設定
	psoDesc.SampleDesc.Count = 1; // サンプル数を1に設定
	psoDesc.SampleDesc.Quality = 0; // サンプル品質を0に設定（デフォルト）

	// パイプラインステートを生成
	hr = m_pDevice->CreateGraphicsPipelineState(
		&psoDesc,								// パイプラインステートの設定
		IID_PPV_ARGS(m_pPSO.GetAddressOf()));	// パイプラインステートのポインタ
	if (FAILED(hr))
	{
		MessageBox(nullptr, "パイプラインステートの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false;
	}

	// ==============================
	//	ビューポートとシザ―矩形の設定
	// ==============================
	m_Viewport.TopLeftX = 0.0f; // ビューポートの左上X座標
	m_Viewport.TopLeftY = 0.0f; // ビューポートの左上Y座標
	m_Viewport.Width = static_cast<float>(m_unWidth); // ビューポートの幅
	m_Viewport.Height = static_cast<float>(m_unHeight); // ビューポートの高さ
	m_Viewport.MinDepth = 0.0f; // ビューポートの最小深度
	m_Viewport.MaxDepth = 1.0f; // ビューポートの最大深度

	m_Scissor.left = 0; // シザ―矩形の左端
	m_Scissor.right = m_unWidth; // シザ―矩形の右端
	m_Scissor.top = 0; // シザ―矩形の上端
	m_Scissor.bottom = m_unHeight; // シザ―矩形の下端

	return true; // 初期化成功
}

void App::OnTerm()
{
	for (uint32_t i = 0; i < FrameCount; ++i)
	{
		if (m_pCB[i].Get())
		{
			m_pCB[i]->Unmap(0, nullptr); // 定数バッファのマッピングを解除
			memset(m_CBV[i].pBuffer, 0, sizeof(m_CBV[i])); // 定数バッファの内容をクリア
		}
		m_pCB[i].Reset(); // 定数バッファのリセット
	}

	m_pVB.Reset(); // 頂点バッファのリセット
	m_pPSO.Reset(); // パイプラインステートのリセット
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
