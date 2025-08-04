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
			  /07/23 15:45 テクスチャ読み込み追加
			  /07/24 13:03 メッシュ読み込み追加
===================================================================+*/

// ==============================
//	include
// ==============================
#include "App.hpp"
#include <algorithm>
#undef max
#undef min

#include "CommonStates.h"
#include "DirectXHelpers.h"
#include "SimpleMath.h"

// ==============================
//	constexpr
// ==============================
constexpr const char* ClassName = "RobotShootingGameClass";

// ==============================
//	定数定義
// ==============================
namespace {
#if 0
	struct Transform
	{
		DirectX::XMMATRIX World{};	// ワールド変換行列
		DirectX::XMMATRIX View{};	// ビュー変換行列
		DirectX::XMMATRIX Proj{};	// 投影変換行列
	};

	struct LightBuffer
	{
		DirectX::SimpleMath::Vector4 LightPosition;		// ライトの位置
		DirectX::SimpleMath::Color LightColor;			// ライトの色
		DirectX::SimpleMath::Vector4 CameraPosition;	// カメラの位置
	};

	struct MaterialBuffer
	{
		DirectX::SimpleMath::Vector3 BaseColor;	// ベースカラー
		float Alpha;							// 透過度
		float Roughness;						// 粗さ
		float Metallic;							// 金属度
	};
#endif
	enum COLOR_TYPE
	{
		COLOR_TYPE_BT709,
		COLOR_TYPE_BT2100_PQ,
	};

	enum TONEMAP_TYPE
	{
		TONEMAP_NONE,
		TONEMAP_REINHARD,
		TONEMAP_GT,
	};

	struct alignas(256) CbTonemap
	{
		int Type;				// トーンマッピングの種類
		int ColorSpace;			// 出力色空間
		float BaseLuminance;	// 基準輝度値
		float MaxLuminance;		// 最大輝度値
	};

	inline int ComputeIntersectionArea(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2)
	{
		return std::max(0, std::min(ax2, bx2) - std::max(ax1, bx1)) * std::max(0, std::min(ay2, by2) - std::max(ay1, by1));
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

	// D3D12の初期化
	if (!InitD3D()) return false;

	// アプリケーション固有の初期化
	if (!OnInit()) return false;

	// ウィンドウを表示
	ShowWindow(m_hWnd, SW_SHOWNORMAL);

	// ウィンドウの更新
	UpdateWindow(m_hWnd);

	// フォーカスをウィンドウに設定
	SetFocus(m_hWnd);

	return true;	// 正常終了
}

void App::TermApp()
{
	// アプリケーション固有の終了処理
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
		this);							// ユーザデータ（thisポインタを渡す）

	if (!m_hWnd)
	{
		MessageBox(nullptr, "ウィンドウの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false;
	}

	return true;	// 正常終了
}

void App::TermWnd()
{
	// ウィンドウの登録を解除
	if (m_hInst) UnregisterClass(ClassName, m_hInst);

	m_hInst = nullptr;
	m_hWnd = nullptr;
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
	hr = CreateDXGIFactory1(IID_PPV_ARGS(m_pFactory.GetAddressOf()));
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
	hr = m_pFactory->CreateSwapChain(
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
	m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	// 不要になったので解放
	pFactory.Reset(); // DXGIファクトリーのスマートポインタをリセット
	pSwapChain.Reset(); // IDXGISwapChainのスマートポインタをリセット

	// ==============================
	//	ディスクリプタプールの生成
	// ==============================
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};

	rtvHeapDesc.NodeMask = 1; // ノードマスク（単一ノード）
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;	// ディスクリプタヒープのタイプ
	rtvHeapDesc.NumDescriptors = 512; // ディスクリプタの数
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // シェーダーからアクセス可能なフラグ
	if (!DescriptorPool::Create(
		m_pDevice.Get(),			// デバイス
		&rtvHeapDesc,				// ディスクリプタヒープの設定
		&m_pPools[POOL_TYPE_RES]))	// リソース用のディスクリプタプールを生成
	{
		MessageBox(nullptr, "CBV/SRV/UAV用のディスクリプタプールの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;	// ディスクリプタヒープのタイプをサンプラ用に変更
	rtvHeapDesc.NumDescriptors = 256; // サンプラの数を256に設定
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // シェーダーからアクセス可能なフラグ
	if (!DescriptorPool::Create(
		m_pDevice.Get(),			// デバイス
		&rtvHeapDesc,				// ディスクリプタヒープの設定
		&m_pPools[POOL_TYPE_SMP]))	// サンプラ用のディスクリプタプールを生成
	{
		MessageBox(nullptr, "サンプラ用のディスクリプタプールの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	// ディスクリプタヒープのタイプをレンダーターゲットビュー用に変更
	rtvHeapDesc.NumDescriptors = 512; // レンダーターゲットビューの数を512に設定
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; // フラグ（なし）
	if (!DescriptorPool::Create(
		m_pDevice.Get(),			// デバイス
		&rtvHeapDesc,				// ディスクリプタヒープの設定
		&m_pPools[POOL_TYPE_RTV]))	// レンダーターゲットビュー用のディスクリプタプールを生成
	{
		MessageBox(nullptr, "レンダーターゲットビュー用のディスクリプタプールの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;	// ディスクリプタヒープのタイプを深度ステンシルビュー用に変更
	rtvHeapDesc.NumDescriptors = 512; // 深度ステンシルビューの数を512に設定
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; // フラグ（なし）
	if (!DescriptorPool::Create(
		m_pDevice.Get(),			// デバイス
		&rtvHeapDesc,				// ディスクリプタヒープの設定
		&m_pPools[POOL_TYPE_DSV]))	// 深度ステンシルビュー用のディスクリプタプールを生成
	{
		MessageBox(nullptr, "深度ステンシルビュー用のディスクリプタプールの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// ==============================
	//	コマンドリストの生成
	// ==============================
	if (!m_commandList.Init(
		m_pDevice.Get(),					// デバイス
		D3D12_COMMAND_LIST_TYPE_DIRECT,		// コマンドリストのタイプ
		FrameCount))						// コマンドアロケータの数
	{
		MessageBox(nullptr, "コマンドリストの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// ===============================
	//	レンダーターゲットビューの生成
	// ===============================
	for (uint32_t i = 0; i < FrameCount; ++i)
	{
		if (!m_colorTarget[i].InitFromBackBuffer(
			m_pDevice.Get(),			// デバイス
			m_pPools[POOL_TYPE_RTV],	// レンダーターゲットビュー用のディスクリプタプール
			true,						// sRGBフォーマットを使用するかどうか
			i,							// バックバッファのインデックス
			m_pSwapChain.Get()))		// スワップチェーン
		{
			MessageBox(nullptr, "バックバッファのレンダーターゲットビューの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return false; // エラー終了
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
		return false; // エラー終了
	}

	// ==============================
	//	フェンスの生成
	// ==============================
	if (!m_fence.Init(m_pDevice.Get())) // フェンスの初期化
	{
		MessageBox(nullptr, "フェンスの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
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

void App::TermD3D()
{
	// GPU処理の完了を待機
	m_fence.Sync(m_pQueue.Get()); // フェンスを使用してGPU処理の完了を待機

	// フェンス破棄
	m_fence.Term(); // フェンスの終了処理

	// レンダーターゲットビューの破棄
	for (uint32_t i = 0; i < FrameCount; ++i)
	{
		m_colorTarget[i].Term(); // 各バックバッファのレンダーターゲットビューを終了
	}

	// 深度ステンシルビューの破棄
	m_depthTarget.Term();

	// コマンドリストの破棄
	m_commandList.Reset();

	for (uint32_t i = 0; i < POOL_TYPE_COUNT; ++i)
	{
		if (m_pPools[i])
		{
			m_pPools[i]->Release(); // 各ディスクリプタプールを終了
			m_pPools[i] = nullptr; // ポインタをnullptrに設定
		}
	}

	// スワップチェーンの破棄
	m_pSwapChain.Reset();

	// コマンドキューの破棄
	m_pQueue.Reset();

	// デバイスの破棄
	m_pDevice.Reset();
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
			OnRender();
		}
	}
}

void App::CheckSupportHDR()
{
	// 何も作られてない場合は処理しない
	if (!m_pSwapChain || !m_pFactory || !m_pDevice) return;

	HRESULT hr = S_OK;

	// ウィンドウ領域を取得
	RECT rect;
	GetWindowRect(m_hWnd, &rect); // ウィンドウの矩形を取得

	if (!m_pFactory->IsCurrent())
	{
		m_pFactory.Reset(); // DXGIファクトリーをリセット
		hr = CreateDXGIFactory2(
			0, // フラグ（なし）
			IID_PPV_ARGS(m_pFactory.GetAddressOf()) // DXGIファクトリーのポインタを取得
		);

		if (FAILED(hr))
		{
			MessageBox(nullptr, "DXGIファクトリーの再生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return; // エラー終了
		}
	}

	ComPtr<IDXGIAdapter1> pAdapter;
	hr = m_pFactory->EnumAdapters1(0, pAdapter.GetAddressOf()); // アダプターを取得
	if (FAILED(hr))
	{
		MessageBox(nullptr, "アダプターの取得に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return; // エラー終了
	}

	UINT i = 0;
	ComPtr<IDXGIOutput> currentOutput;
	ComPtr<IDXGIOutput> bestOutput;
	int bestIntersectArea = -1;

	// 各ディスプレイを調べる
	while (pAdapter->EnumOutputs(i, &currentOutput) != DXGI_ERROR_NOT_FOUND)
	{
		auto ax1 = rect.left;	// ウィンドウの左端
		auto ay1 = rect.top;	// ウィンドウの上端
		auto ax2 = rect.right;	// ウィンドウの右端
		auto ay2 = rect.bottom;	// ウィンドウの下端

		// ディスプレイの設定を取得
		DXGI_OUTPUT_DESC desc;
		hr = currentOutput->GetDesc(&desc);
		if (FAILED(hr))
		{
			MessageBox(nullptr, "ディスプレイの設定取得に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return; // エラー終了
		}

		auto bx1 = desc.DesktopCoordinates.left;	// ディスプレイの左端
		auto by1 = desc.DesktopCoordinates.top;	// ディスプレイの上端
		auto bx2 = desc.DesktopCoordinates.right;	// ディスプレイの右端
		auto by2 = desc.DesktopCoordinates.bottom;	// ディスプレイの下端

		// 領域が一致するかどうか調べる
		int intersectArea = ComputeIntersectionArea(
			ax1, ay1, ax2, ay2,	// ウィンドウの座標
			bx1, by1, bx2, by2);	// ディスプレイの座標
		if (intersectArea > bestIntersectArea)
		{
			bestOutput = currentOutput;			// 最適なディスプレイを更新
			bestIntersectArea = intersectArea;	// 最も大きい領域を更新
		}

		++i; // 次のディスプレイへ
	}

	// 一番適しているディスプレイ
	ComPtr<IDXGIOutput6> pOutput6;
	hr = bestOutput.As(&pOutput6); // IDXGIOutput6にキャスト
	if (FAILED(hr))
	{
		MessageBox(nullptr, "IDXGIOutput6の取得に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return; // エラー終了
	}

	// 出力設定を取得
	DXGI_OUTPUT_DESC1 desc1;
	hr = pOutput6->GetDesc1(&desc1); // 出力の設定を取得
	if (FAILED(hr))
	{
		MessageBox(nullptr, "出力設定の取得に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return; // エラー終了
	}

	// 色空間がITU-R BT.2100 PQをサポートしているか確認
	m_supportHDR = (desc1.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);
	m_maxLuminance = desc1.MaxLuminance; // 最大輝度を取得
	m_minLuminance = desc1.MinLuminance; // 最小輝度を取得
}

void App::Present(uint32_t interval)
{
	// 画面に表示
	m_pSwapChain->Present(interval, 0); // スワップチェーンをプレゼント

	// 完了待ち
	m_fence.Wait(m_pQueue.Get(), INFINITE);
	
	// フレーム番号を更新
	m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex(); // 現在のバックバッファのインデックスを取得
}

LRESULT App::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// ウィンドウプロシージャのポインタを取得
	auto instance = reinterpret_cast<App*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (msg)
	{
	case WM_CREATE:
	{
		// ウィンドウの作成時にユーザデータを設定
		auto pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lp);
		auto pApp = reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams);	// ユーザデータからthisポインタを取得
		SetWindowLongPtr(hWnd, GWLP_USERDATA, pApp);	// ユーザデータにthisポインタを設定
	}
		break;
	case WM_DESTROY:
		PostQuitMessage(0); // アプリケーションの終了
		break;
	case WM_MOVE:
		instance->CheckSupportHDR();
		break;
	case WM_DISPLAYCHANGE:
		instance->CheckSupportHDR();
		break;
	default:
		break;
	}

	if (instance)
	{
		// アプリケーションのメッセージ処理
		instance->OnMsgProc(hWnd, msg, wp, lp);
	}

	return DefWindowProc(hWnd, msg, wp, lp);
}

bool App::OnInit()
{
#if 0
	// ===============================
	//	メッシュをロード
	// ===============================
	std::vector<MeshData> meshData;
	std::vector<MaterialData> materialData;

	if (!LoadMesh("Assets/teapot/teapot.obj", meshData, materialData))
	{
		MessageBox(nullptr, "メッシュのロードに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// メモリを予約
	m_pMesh.reserve(meshData.size()); // メッシュの数だけメモリを予約

	// メッシュを初期化
	for (size_t i = 0; i < meshData.size(); ++i)
	{
		// メッシュ生成
		Mesh* mesh = new (std::nothrow) Mesh();

		if (!mesh)
		{
			MessageBox(nullptr, "メモリの確保に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return false; // エラー終了
		}

		// 初期化処理
		if (!mesh->Init(m_pDevice.Get(), meshData[i]))
		{
			delete mesh; // メモリ解放
			MessageBox(nullptr, "メッシュの初期化に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return false; // エラー終了
		}

		// 成功したら登録
		m_pMesh.push_back(mesh); // メッシュをベクターに追加
	}

	// メモリ最適化
	m_pMesh.shrink_to_fit();

	// マテリアル初期化
	if (!m_material.Init(
		m_pDevice.Get(),			// デバイス
		m_pPools[POOL_TYPE_RES],	// リソース用のディスクリプタプール
		sizeof(MaterialBuffer),		// マテリアルのインデックス
		materialData.size()			// マテリアルの数
	))
	{
		MessageBox(nullptr, "マテリアルの初期化に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// リソースバッチを用意
	DirectX::ResourceUploadBatch batch(m_pDevice.Get());

	// バッチ開始
	batch.Begin();

	// テクスチャ設定
	for (size_t i = 0; i < materialData.size(); ++i)
	{
		auto ptr = m_material.GetBufferPtr<MaterialBuffer>(i); // マテリアルのバッファポインタを取得
		ptr->BaseColor = materialData[i].Diffuse; // ベースカラーを設定
		ptr->Alpha = materialData[i].Alpha; // 透過度を設定
		ptr->Roughness = 0.2f; // 粗さを設定（固定値）
		ptr->Metallic = 0.5f; // 金属度を設定（固定値）

		m_material.SetTexture(
			i,							// マテリアルのインデックス
			TU_DIFFUSE,					// テクスチャユニット
			materialData[i].DiffuseMap,	// 拡散反射マップのファイル名
			batch);						// リソースアップロードバッチ
	}

	// バッチ終了
	auto future = batch.End(m_pQueue.Get()); // リソースアップロードバッチを終了

	// バッチ完了を待機
	future.wait(); // 非同期処理の完了を待機

	// ==============================
	//	ライトバッファの設定
	// ==============================
	auto pCB = new (std::nothrow) ConstantBuffer();
	if (!pCB)
	{
		MessageBox(nullptr, "ライトバッファのメモリ確保に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	if (!pCB->Init(
		m_pDevice.Get(),			// デバイス
		m_pPools[POOL_TYPE_RES],	// リソース用のディスクリプタプール
		sizeof(LightBuffer)			// バッファサイズ
	))
	{
		delete pCB; // メモリ解放
		MessageBox(nullptr, "ライトバッファの初期化に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	auto ptr = pCB->GetPtr<LightBuffer>(); // ライトバッファのポインタを取得
	ptr->LightPosition = DirectX::SimpleMath::Vector4(0.0f, 50.0f, 100.0f, 1.0f); // ライトの位置を設定
	ptr->LightColor = DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 0.0f); // ライトの色を設定
	ptr->CameraPosition = DirectX::SimpleMath::Vector4(0.0f, 1.0f, 2.0f, 1.0f); // カメラの位置を設定

	m_pLight = pCB; // ライトバッファのポインタを保存
#endif
	// ==============================
	//	ルートシグネチャの生成
	// ==============================
	auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 入力アセンブラ入力レイアウトを許可
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS; // ドメインシェーダーのルートアクセスを拒否
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS; // ハルシェーダーのルートアクセスを拒否
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS; // ジオメトリシェーダーのルートアクセスを拒否

	// ディスクリプタレンジを設定
	D3D12_DESCRIPTOR_RANGE range{};
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // シェーダーリソースビューのタイプ
	range.NumDescriptors = 1; // ディスクリプタの数
	range.BaseShaderRegister = 0; // ベースシェーダーレジスタ
	range.RegisterSpace = 0; // レジスタスペース
	range.OffsetInDescriptorsFromTableStart = 0; // テーブルの開始からのオフセット

	// ルートパラメーターの設定
	D3D12_ROOT_PARAMETER param[4]{};
	param[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // 定数バッファビュー
	param[0].Descriptor.ShaderRegister = 0; // シェーダーレジスタ
	param[0].Descriptor.RegisterSpace = 0; // レジスタスペース
	param[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // シェーダーの可視性（頂点シェーダー）

	param[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // 定数バッファビュー
	param[1].Descriptor.ShaderRegister = 1; // シェーダーレジスタ
	param[1].Descriptor.RegisterSpace = 0; // レジスタスペース
	param[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // シェーダーの可視性（ピクセルシェーダー）

	param[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // 定数バッファビュー
	param[2].Descriptor.ShaderRegister = 2; // シェーダーレジスタ
	param[2].Descriptor.RegisterSpace = 0; // レジスタスペース
	param[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // シェーダーの可視性（ピクセルシェーダー）

	param[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // ディスクリプタテーブル
	param[3].DescriptorTable.NumDescriptorRanges = 1; // ディスクリプタレンジの数
	param[3].DescriptorTable.pDescriptorRanges = &range; // ディスクリプタレンジのポインタ
	param[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // シェーダーの可視性（ピクセルシェーダー）

	// スタティックサンプラーの設定
	auto sampler = DirectX::CommonStates::StaticLinearWrap(0, D3D12_SHADER_VISIBILITY_PIXEL); // ピクセルシェーダーで使用するスタティックサンプラー

	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc{};
	rootSigDesc.NumParameters = _countof(param); // ルートパラメーターの数
	rootSigDesc.NumStaticSamplers = 1; // スタティックサンプラーの数
	rootSigDesc.pParameters = param; // ルートパラメーターのポインタ
	rootSigDesc.pStaticSamplers = &sampler; // スタティックサンプラーのポインタ
	rootSigDesc.Flags = flag; // ルートシグネチャのフラグ

	ComPtr<ID3DBlob> pBlob; // ルートシグネチャのバイナリデータ
	ComPtr<ID3DBlob> pErrorBlob; // エラーメッセージ用のバイナリデータ

	// ルートシグネチャをシリアライズ
	HRESULT hr = D3D12SerializeRootSignature(
		&rootSigDesc,						// ルートシグネチャの設定
		D3D_ROOT_SIGNATURE_VERSION_1,		// ルートシグネチャのバージョン
		pBlob.GetAddressOf(),				// シリアライズされたルートシグネチャのポインタ
		pErrorBlob.GetAddressOf());			// エラーメッセージのポインタ
	if (FAILED(hr))
	{
		// エラーメッセージが存在する場合は表示
		if (pErrorBlob)
		{
			MessageBoxA(nullptr, static_cast<const char*>(pErrorBlob->GetBufferPointer()), "ルートシグネチャのシリアライズに失敗", MB_OK | MB_ICONERROR);
		}
		else
		{
			MessageBox(nullptr, "ルートシグネチャのシリアライズに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		}
		return false; // エラー終了
	}

	// ルートシグネチャを生成
	hr = m_pDevice->CreateRootSignature(
		0,											// ノードマスク（単一ノード）
		pBlob->GetBufferPointer(),					// ルートシグネチャのバッファポインタ
		pBlob->GetBufferSize(),						// ルートシグネチャのバッファサイズ
		IID_PPV_ARGS(m_pRootSig.GetAddressOf()));	// ルートシグネチャのポインタ
	if (FAILED(hr))
	{
		MessageBox(nullptr, "ルートシグネチャの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// ===============================
	//	パイプラインステートの生成
	// ===============================
	ComPtr<ID3DBlob> pVSBlob = nullptr; // 頂点シェーダーのバイナリデータ
	ComPtr<ID3DBlob> pPSBlob = nullptr; // ピクセルシェーダーのバイナリデータ

	// 頂点シェーダー読み込み
	hr = D3DReadFileToBlob(L"Assets/Shader/LambertVS.cso", pVSBlob.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(nullptr, "頂点シェーダーの読み込みに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// ピクセルシェーダー読み込み
	hr = D3DReadFileToBlob(L"Assets/Shader/GGXPS.cso", pPSBlob.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(nullptr, "ピクセルシェーダーの読み込みに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	D3D12_INPUT_ELEMENT_DESC elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// グラフィックスパイプラインステートを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.InputLayout = { elements , 2 }; // 入力レイアウト
	psoDesc.pRootSignature = m_pRootSig.Get(); // ルートシグネチャ
	psoDesc.VS = { pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize() }; // 頂点シェーダー
	psoDesc.PS = { pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize() }; // ピクセルシェーダー
	psoDesc.RasterizerState = DirectX::CommonStates::CullNone; // ラスタライザーステート（カリングなし）
	psoDesc.BlendState = DirectX::CommonStates::Opaque; // ブレンドステート（不透明）
	psoDesc.DepthStencilState = DirectX::CommonStates::DepthDefault; // 深度ステンシルステート（デフォルト）
	psoDesc.SampleMask = UINT_MAX; // サンプルマスク（全サンプル）
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // プリミティブトポロジタイプ（三角形）
	psoDesc.NumRenderTargets = 1; // レンダーターゲットの数
	psoDesc.RTVFormats[0] = m_colorTarget[0].GetViewDesc().Format; // レンダーターゲットのフォーマット
	psoDesc.DSVFormat = m_depthTarget.GetViewDesc().Format; // 深度ステンシルビューのフォーマット
	psoDesc.SampleDesc.Count = 1; // マルチサンプリングのサンプル数
	psoDesc.SampleDesc.Quality = 0; // マルチサンプリングの品質

	// パイプラインステートを生成
	hr = m_pDevice->CreateGraphicsPipelineState(
		&psoDesc,								// パイプラインステートの設定
		IID_PPV_ARGS(m_pPSO.GetAddressOf()));	// パイプラインステートのポインタ
	if (FAILED(hr))
	{
		MessageBox(nullptr, "パイプラインステートの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// =============================
	//	頂点バッファの作成
	// =============================
	struct Vertex
	{
		float px, py; // 頂点位置
		float tx, ty; // テクスチャ座標
	};

	if (!m_quadVB.Init<Vertex>(m_pDevice.Get(), 3))
	{
		MessageBox(nullptr, "四角形の頂点バッファの初期化に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	auto ptr = m_quadVB.Map<Vertex>(); // 頂点バッファのポインタを取得
	assert(ptr); // ポインタがnullptrでないことを確認
	ptr[0] = { -1.0f, 1.0f, 0.0f, -1.0f };
	ptr[1] = { 3.0f, 1.0f, 2.0f, -1.0f };
	ptr[2] = { -1.0f, -3.0f, 0.0f, 1.0f };
	m_quadVB.Unmap(); // 頂点バッファのアンマップ

	for (uint32_t i = 0; i < FrameCount; ++i)
	{
		if (!m_CB[i].Init(m_pDevice.Get(), m_pPools[POOL_TYPE_RES], sizeof(CbTonemap)))
		{
			MessageBox(nullptr, "定数バッファの初期化に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return false; // エラー終了
		}
	}

	// ==============================
	//	テクスチャロード
	// ==============================
	DirectX::ResourceUploadBatch batch(m_pDevice.Get()); // リソースアップロードバッチの生成

	// バッチ開始
	batch.Begin();

	// テクスチャ初期化
	if (!m_texture.Init(
		m_pDevice.Get(),					// デバイス
		m_pPools[POOL_TYPE_RES],			// リソース用のディスクリプタプール
		L"Assets/Texture/hdr014.hdr",		// テクスチャファイル名
		batch))								// リソースアップロードバッチ
	{
		MessageBox(nullptr, "テクスチャの初期化に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// バッチ終了
	auto future = batch.End(m_pQueue.Get()); // リソースアップロードバッチを終了

	future.wait(); // 非同期処理の完了を待機

#if 0
	// ==============================
	//	変換行列用の定数バッファの生成
	// ==============================
	m_Transform.reserve(FrameCount); // 変換行列の数だけメモリを予約

	for (size_t i = 0; i < FrameCount; ++i)
	{
		auto pCB = new (std::nothrow) ConstantBuffer();
		if (!pCB)
		{
			MessageBox(nullptr, "メモリの確保に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return false; // エラー終了
		}

		// 定数バッファ初期化
		if (!pCB->Init(
			m_pDevice.Get(),			// デバイス
			m_pPools[POOL_TYPE_RES],	// リソース用のディスクリプタプール
			sizeof(Transform) * 2		// 定数バッファのサイズ
		))
		{
			delete pCB; // メモリ解放
			MessageBox(nullptr, "定数バッファの初期化に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return false; // エラー終了
		}

		// カメラ設定
		auto eyePos = DirectX::XMVectorSet(0.0f, 1.0f, 2.0f, 0.0f); // カメラの位置
		auto targetPos = DirectX::XMVectorZero(); // カメラの注視点
		auto upDir = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // カメラの上方向

		// 垂直画角とアスペクト比の設定
		auto fovY = DirectX::XMConvertToRadians(37.5f); // 垂直画角
		auto aspect = static_cast<float>(m_unWidth) / static_cast<float>(m_unHeight); // アスペクト比

		// 変換行列を設定
		auto ptr = pCB->GetPtr<Transform>(); // 定数バッファのポインタを取得
		ptr->World = DirectX::XMMatrixIdentity(); // ワールド行列を単位行列に設定
		ptr->View = DirectX::XMMatrixLookAtLH(eyePos, targetPos, upDir); // ビュー行列を設定
		ptr->Proj = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, 1.0f, 100.0f); // プロジェクション行列を設定

		m_Transform.push_back(pCB); // 変換行列をリストに追加
	}

	m_RotateAngle = DirectX::XMConvertToRadians(-60.0f); // 初期回転角度を設定
#endif
	return true; // 正常終了
}

void App::OnTerm()
{
	// メッシュ破棄
	for (auto& mesh : m_pMesh)
	{
		if (mesh)
		{
			mesh->Term(); // メッシュの終了処理
			delete mesh; // メモリ解放
			mesh = nullptr; // ポインタをnullptrに設定
		}
	}
	m_pMesh.clear(); // メッシュのリストをクリア
	m_pMesh.shrink_to_fit(); // メモリを最適化

	// マテリアル破棄
	m_material.Term(); // マテリアルの終了処理

	// ライトバッファ破棄
	m_pLight->Term(); // ライトバッファの終了処理
	delete m_pLight; // メモリ解放

	// 変換行列用の定数バッファ破棄
	for (auto& pCB : m_Transform)
	{
		if (pCB)
		{
			pCB->Term(); // 定数バッファの終了処理
			delete pCB; // メモリ解放
			pCB = nullptr; // ポインタをnullptrに設定
		}
	}
	m_Transform.clear(); // 変換行列のリストをクリア
	m_Transform.shrink_to_fit(); // メモリを最適化
}

void App::OnRender()
{
	// ===============================
	//	更新処理
	// ===============================
	m_RotateAngle += 0.025f; // 回転角度を更新

	auto pTransform = m_Transform[m_frameIndex]->GetPtr<Transform>(); // 現在の変換行列を取得
	pTransform->World = DirectX::XMMatrixRotationY(m_RotateAngle); // ワールド行列を回転行列に設定

	// コマンドリストの記録を開始
	auto pCmd = m_commandList.Reset();

	// 書き込み用リソースバリア設定
	DirectX::TransitionResource(
		pCmd,										// コマンドリスト
		m_colorTarget[m_frameIndex].GetResource(),	// レンダーターゲットビュー
		D3D12_RESOURCE_STATE_PRESENT,				// レンダーターゲット状態
		D3D12_RESOURCE_STATE_RENDER_TARGET);		// レンダーターゲット状態に変更

	// ディスクリプタ取得
	auto handleRTV = m_colorTarget[m_frameIndex].GetHandleRTV(); // レンダーターゲットビューのハンドルを取得
	auto handleDSV = m_depthTarget.GetHandleDSV(); // 深度ステンシルビューのハンドルを取得

	// レンダーターゲットを設定
	pCmd->OMSetRenderTargets(1, &handleRTV->HandleCPU, FALSE, &handleDSV->HandleCPU); // レンダーターゲットと深度ステンシルビューを設定

	// クリアカラー
	float clearColor[4] = { 0.25f, 0.25f, 0.25f, 1.0f }; // クリアカラーを黒に設定

	// レンダーターゲットをクリア
	pCmd->ClearRenderTargetView(handleRTV->HandleCPU, clearColor, 0, nullptr); // レンダーターゲットビューをクリア

	// 深度ステンシルビューをクリア
	pCmd->ClearDepthStencilView(handleDSV->HandleCPU, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr); // 深度ステンシルビューをクリア

	// 描画処理
	ID3D12DescriptorHeap* ppHeaps[] = { m_pPools[POOL_TYPE_RES]->GetHeap() }; // リソース用のディスクリプタヒープを取得

	pCmd->SetGraphicsRootSignature(m_pRootSig.Get()); // ルートシグネチャを設定
	pCmd->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps); // ディスクリプタヒープを設定
	pCmd->SetGraphicsRootConstantBufferView(0, m_Transform[m_frameIndex]->GetAddress()); // 定数バッファビューを設定
	pCmd->SetGraphicsRootConstantBufferView(1, m_pLight->GetAddress()); // マテリアル定数バッファビューを設定
	pCmd->SetPipelineState(m_pPSO.Get()); // パイプラインステートを設定
	pCmd->RSSetViewports(1, &m_Viewport); // ビューポートを設定
	pCmd->RSSetScissorRects(1, &m_scissor); // シザー矩形を設定

	for (size_t i = 0; i < m_pMesh.size(); ++i)
	{
		// マテリアルIDを取得
		auto id = m_pMesh[i]->GetMaterialId();

		// 定数バッファを設定
		pCmd->SetGraphicsRootConstantBufferView(
			2,									// ルートパラメーターのインデックス
			m_material.GetBufferAddress(i));	// マテリアルの定数バッファのアドレスを設定

		// テクスチャを設定
		pCmd->SetGraphicsRootDescriptorTable(
			3,												// ルートパラメーターのインデックス
			m_material.GetTextureHandle(id, TU_DIFFUSE));	// テクスチャのハンドルを設定

		// メッシュを描画
		m_pMesh[i]->Draw(pCmd);
	}

	// 表示用リソースバリア設定
	DirectX::TransitionResource(
		pCmd,										// コマンドリスト
		m_colorTarget[m_frameIndex].GetResource(),	// レンダーターゲットビュー
		D3D12_RESOURCE_STATE_RENDER_TARGET,			// レンダーターゲット状態
		D3D12_RESOURCE_STATE_PRESENT);				// プレゼント状態に変更

	// コマンドリストの記録を終了
	pCmd->Close(); // コマンドリストをクローズ

	// コマンドリストを実行
	ID3D12CommandList* pCommandLists[] = { pCmd }; // コマンドリストの配列を作成
	m_pQueue->ExecuteCommandLists(_countof(pCommandLists), pCommandLists); // コマンドリストを実行

	// 画面に表示
	Present(1); // スワップチェーンをプレゼント
}

void App::OnMsgProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// キーボードの処理
	if ((msg == WM_KEYDOWN) || (msg == WM_SYSKEYDOWN) || (msg == WM_KEYUP) || (msg == WM_SYSKEYUP))
	{
		DWORD mask = (1 << 29);	// 29ビット目のマスク（キーボードの状態を取得するためのマスク）

		auto isKeyDown = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN); // キーが押されたかどうかを判定
		auto isAltDown = ((lp & mask) != 0);	// Altキーが押されているかどうかを判定
		auto keyCode = static_cast<uint32_t>(wp); // キーコードを取得

		if (isKeyDown)
		{
			switch (keyCode)
			{
			case VK_ESCAPE:
				PostQuitMessage(0); // ESCキーでアプリケーションを終了
				break;
			case 'H':
				ChangeDisplayMode(true);
				break;
			case 'S':
				ChangeDisplayMode(false);
				break;
			case 'N':
				m_tonemapType = TONEMAP_NONE; // トーンマッピングなし
				break;
			case 'R':
				m_tonemapType = TONEMAP_REINHARD; // Reinhardトーンマッピング
				break;
			case 'G':
				m_tonemapType = TONEMAP_GT; // Gammaトーンマッピング
				break;
			}
		}
	}
}
