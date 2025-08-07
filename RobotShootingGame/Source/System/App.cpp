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

	struct alignas(256) CbMesh
	{
		DirectX::SimpleMath::Matrix World;
	};

	struct alignas(256) CbTransform
	{
		DirectX::SimpleMath::Matrix View;	// ビュー行列
		DirectX::SimpleMath::Matrix Proj;	// 投影行列
	};

	struct alignas(256) CbLight
	{
		DirectX::SimpleMath::Vector3 LightPosition;	// ライトの位置
		float LightInvSqrRadius;	// ライトの逆二乗半径
		DirectX::SimpleMath::Vector3 LightColor;	// ライトの色
		float LightIntensity;		// ライトの強度
	};

	struct alignas(256) CbCamera
	{
		DirectX::SimpleMath::Vector3 CameraPosition;	// カメラの位置
	};
	
	struct alignas(256) CbMaterial
	{
		DirectX::SimpleMath::Vector3 BaseColor;	// ベースカラー
		float Alpha;			// アルファ値
		float Roughness;		// ラフネス値
		float Metallic;			// メタリック値
	};

	UINT16 inline GetChromaticityCoord(double value)
	{
		return static_cast<UINT16>(value * 50000);
	}

	inline int ComputeIntersectionArea(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2)
	{
		return std::max(0, std::min(ax2, bx2) - std::max(ax1, bx1)) * std::max(0, std::min(ay2, by2) - std::max(ay1, by1));
	}

	CbLight ComputePointLight(const DirectX::SimpleMath::Vector3& pos, float radius, const DirectX::SimpleMath::Vector3& color, float intensity)
	{
		CbLight result{};
		result.LightPosition = pos;
		result.LightInvSqrRadius = 1.0f / (radius * radius);
		result.LightColor = color;
		result.LightIntensity = intensity;
		return result;
	}

	DirectX::SimpleMath::Vector3 CalocLightColor(float time)
	{
		auto c = fmodf(time, 3.0f);
		auto result = DirectX::SimpleMath::Vector3(0.25f, 0.25f, 0.25f);

		if (c < 1.0f)
		{
			result.x += 1.0f - c;
			result.y += c;
		}
		else if (c < 2.0f)
		{
			c -= 1.0f;
			result.y += 1.0f - c;
			result.z += c;
		}
		else
		{
			c -= 2.0f;
			result.z += 1.0f - c;
			result.x += c;
		}
		
		return result;
	}
}

App::App(uint32_t width, uint32_t height, DXGI_FORMAT format)
{
	m_unWidth = width;
	m_unHeight = height;
	m_backBufferFormat = format;
	m_tonemapType = TONEMAP_NONE;
	m_colorSpace = COLOR_TYPE_BT709;
	m_BaseLuminance = 100.0f;	// 基準輝度値
	m_MaxLuminance = 100.0f;	// 最大輝度値
	m_Exposure = 1.0f;		// 露出値
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
	hr = CreateDXGIFactory2(0, IID_PPV_ARGS(m_pFactory.GetAddressOf()));
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
	swapDesc.BufferDesc.Format = m_backBufferFormat; // バッファのフォーマット
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

	// IDXGISwapChain4を取得
	hr = pSwapChain.As(&m_pSwapChain); // IDXGISwapChain4のスマートポインタにキャスト
	if (FAILED(hr))
	{
		MessageBox(nullptr, "IDXGISwapChain4の取得に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// バックバッファ番号を取得
	m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	// 不要になったので解放
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
	// ===============================
	//	メッシュをロード
	// ===============================
	std::vector<MeshData> meshData;
	std::vector<MaterialData> materialData;

	if (!LoadMesh("Assets/material_test/material_test.obj", meshData, materialData))
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
		sizeof(CbMaterial),			// マテリアルのインデックス
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

	// ハードコーディングする
	m_material.SetTexture(0, TU_BASE_COLOR, L"Assets/material_test/wall_bc.dds", batch);
	m_material.SetTexture(0, TU_METALLIC, L"Assets/material_test/wall_m.dds", batch);
	m_material.SetTexture(0, TU_ROUGHNESS, L"Assets/material_test/wall_r.dds", batch);
	m_material.SetTexture(0, TU_NORMAL, L"Assets/material_test/wall_n.dds", batch);

	m_material.SetTexture(1, TU_BASE_COLOR, L"Assets/material_test/matball_bc.dds", batch);
	m_material.SetTexture(1, TU_METALLIC, L"Assets/material_test/matball_m.dds", batch);
	m_material.SetTexture(1, TU_ROUGHNESS, L"Assets/material_test/matball_r.dds", batch);
	m_material.SetTexture(1, TU_NORMAL, L"Assets/material_test/matball_n.dds", batch);

	// バッチ終了
	auto future = batch.End(m_pQueue.Get()); // リソースアップロードバッチを終了

	// バッチ完了を待機
	future.wait(); // 非同期処理の完了を待機

	// ==============================
	//	ライトバッファの設定
	// ==============================
	for (size_t i = 0; i < FrameCount; ++i)
	{
		if (!m_LightCB[i].Init(
			m_pDevice.Get(),					// デバイス
			m_pPools[POOL_TYPE_RES],			// リソース用のディスクリプタプール
			sizeof(CbLight)))					// 定数バッファのサイズ
		{
			MessageBox(nullptr, "ライト定数バッファの初期化に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return false; // エラー終了
		}

		auto ptr = m_LightCB[i].GetPtr<CbLight>(); // 定数バッファのポインタを取得
		*ptr = ComputePointLight(DirectX::SimpleMath::Vector3(0.0f, 1.0f, 1.5f), 1.0f, DirectX::SimpleMath::Vector3(1.0f, 0.5f, 0.0f), 10.0f);
	}

	// ==============================
	//	カメラバッファの設定
	// ==============================
	for (size_t i = 0; i < FrameCount; ++i)
	{
		if (!m_CameraCB[i].Init(
			m_pDevice.Get(),					// デバイス
			m_pPools[POOL_TYPE_RES],			// リソース用のディスクリプタプール
			sizeof(CbCamera)))					// 定数バッファのサイズ
		{
			MessageBox(nullptr, "カメラ定数バッファの初期化に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return false; // エラー終了
		}
	}

	// ==============================
	//	シーン用カラーターゲットの生成
	// ==============================
	float clearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };

	if (!m_SceneColorTarget.Init(
		m_pDevice.Get(),				// デバイス
		m_pPools[POOL_TYPE_RTV],		// レンダーターゲットビュー用のディスクリプタプール
		m_pPools[POOL_TYPE_RES],		// リソース用のディスクリプタプール
		m_unWidth,						// 幅
		m_unHeight,						// 高さ
		DXGI_FORMAT_R10G10B10A2_UNORM,	// フォーマット
		clearColor						// クリアカラー
	))
	{
		MessageBox(nullptr, "シーン用カラーターゲットの初期化に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// ==============================
	//	シーン用深度ターゲットの生成
	// ==============================
	if (!m_SceneDepthTarget.Init(
		m_pDevice.Get(),			// デバイス
		m_pPools[POOL_TYPE_DSV],	// 深度ステンシルビュー用のディスクリプタプール
		nullptr,					// 深度ステンシルリソース
		m_unWidth,					// 幅
		m_unHeight,					// 高さ
		DXGI_FORMAT_D32_FLOAT,		// 深度ステンシルフォーマット
		1.0f,						// 深度クリア値
		0))							// ステンシルクリア値
	{
		MessageBox(nullptr, "シーン用深度ターゲットの初期化に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// ==============================
	//	シーン用ルートシグネチャの生成
	// ==============================
	RootSignature::Desc desc; // ルートシグネチャの設定
	desc.Begin(8)
		.SetCBV(ShaderStage::VS, 0, 0) // 頂点シェーダーで使用する定数バッファビュー
		.SetCBV(ShaderStage::VS, 1, 1) // 頂点シェーダーで使用する定数バッファビュー
		.SetCBV(ShaderStage::PS, 2, 2) // ピクセルシェーダーで使用する定数バッファビュー
		.SetCBV(ShaderStage::PS, 3, 2) // ピクセルシェーダーで使用する定数バッファビュー
		.SetSRV(ShaderStage::PS, 4, 0) // ピクセルシェーダーで使用するシェーダーリソースビュー
		.SetSRV(ShaderStage::PS, 5, 1) // ピクセルシェーダーで使用するシェーダーリソースビュー
		.SetSRV(ShaderStage::PS, 6, 2) // ピクセルシェーダーで使用するシェーダーリソースビュー
		.SetSRV(ShaderStage::PS, 7, 3) // ピクセルシェーダーで使用するシェーダーリソースビュー
		.AddStaticSmp(ShaderStage::PS, 0, SamplerState::LinearWrap)	// ピクセルシェーダーで使用するスタティックサンプラー
		.AddStaticSmp(ShaderStage::PS, 1, SamplerState::LinearWrap)	// ピクセルシェーダーで使用するスタティックサンプラー
		.AddStaticSmp(ShaderStage::PS, 2, SamplerState::LinearWrap)	// ピクセルシェーダーで使用するスタティックサンプラー
		.AddStaticSmp(ShaderStage::PS, 3, SamplerState::LinearWrap)	// ピクセルシェーダーで使用するスタティックサンプラー
		.AllowIL()	// インライン化を許可
		.End();		// ルートシグネチャの設定を終了

	if (!m_SceneRootSig.Init(
		m_pDevice.Get(),		// デバイス
		desc.GetDesc()))		// ルートシグネチャの設定
	{
		MessageBox(nullptr, "ルートシグネチャの初期化に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// ===============================
	//	シーン用パイプラインステートの生成
	// ===============================
	ComPtr<ID3DBlob> pVSBlob = nullptr; // 頂点シェーダーのバイナリデータ
	ComPtr<ID3DBlob> pPSBlob = nullptr; // ピクセルシェーダーのバイナリデータ

	// 頂点シェーダー読み込み
	HRESULT hr = D3DReadFileToBlob(L"Assets/Shader/BasicVS.cso", pVSBlob.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(nullptr, "頂点シェーダーの読み込みに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// ピクセルシェーダー読み込み
	hr = D3DReadFileToBlob(L"Assets/Shader/BasicPS.cso", pPSBlob.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(nullptr, "ピクセルシェーダーの読み込みに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	D3D12_INPUT_ELEMENT_DESC elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// グラフィックスパイプラインステートを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.InputLayout = { elements , _countof(elements) }; // 入力レイアウト
	psoDesc.pRootSignature = m_pRootSig.Get(); // ルートシグネチャ
	psoDesc.VS = { pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize() }; // 頂点シェーダー
	psoDesc.PS = { pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize() }; // ピクセルシェーダー
	psoDesc.RasterizerState = DirectX::CommonStates::CullNone; // ラスタライザーステート（カリングなし）
	psoDesc.BlendState = DirectX::CommonStates::Opaque; // ブレンドステート（不透明）
	psoDesc.DepthStencilState = DirectX::CommonStates::DepthDefault; // 深度ステンシルステート（デフォルト）
	psoDesc.SampleMask = UINT_MAX; // サンプルマスク（全サンプル）
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // プリミティブトポロジタイプ（三角形）
	psoDesc.NumRenderTargets = 1; // レンダーターゲットの数
	psoDesc.RTVFormats[0] = m_SceneColorTarget.GetRTVDesc().Format; // レンダーターゲットのフォーマット
	psoDesc.DSVFormat = m_SceneDepthTarget.GetDSVDesc().Format; // 深度ステンシルビューのフォーマット
	psoDesc.SampleDesc.Count = 1; // マルチサンプリングのサンプル数
	psoDesc.SampleDesc.Quality = 0; // マルチサンプリングの品質

	// パイプラインステートを生成
	hr = m_pDevice->CreateGraphicsPipelineState(
		&psoDesc,								// パイプラインステートの設定
		IID_PPV_ARGS(m_pScenePSO.GetAddressOf()));	// パイプラインステートのポインタ
	if (FAILED(hr))
	{
		MessageBox(nullptr, "パイプラインステートの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// =============================
	//	トーンマップ用ルートシグネチャの生成
	// =============================
	desc.Begin(2)
		.SetCBV(ShaderStage::PS, 0, 0) // ピクセルシェーダーで使用する定数バッファビュー
		.SetSRV(ShaderStage::PS, 1, 0) // ピクセルシェーダーで使用するシェーダーリソースビュー
		.AddStaticSmp(ShaderStage::PS, 0, SamplerState::LinearWrap) // ピクセルシェーダーで使用するスタティックサンプラー
		.AllowIL() // インライン化を許可
		.End(); // ルートシグネチャの設定を終了

	if (!m_TonemapRootSig.Init(
		m_pDevice.Get(),		// デバイス
		desc.GetDesc()))		// ルートシグネチャの設定
	{
		MessageBox(nullptr, "トーンマップ用ルートシグネチャの初期化に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// ==============================
	//	トーンマップ用パイプラインステートの生成
	// ==============================
	// 頂点シェーダーを読み込み
	hr = D3DReadFileToBlob(L"Assets/Shader/TonemapVS.cso", pPSBlob.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(nullptr, "トーンマップ用頂点シェーダーの読み込みに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// ピクセルシェーダーを読み込み
	hr = D3DReadFileToBlob(L"Assets/Shader/TonemapPS.cso", pVSBlob.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(nullptr, "トーンマップ用ピクセルシェーダーの読み込みに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	D3D12_INPUT_ELEMENT_DESC quadElements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// グラフィックスパイプラインステートを設定
	psoDesc = {};
	psoDesc.InputLayout = { quadElements, _countof(quadElements) }; // 入力レイアウト
	psoDesc.pRootSignature = m_TonemapRootSig.GetPtr(); // トーンマップ用ルートシグネチャ
	psoDesc.VS = { pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize() }; // トーンマップ用頂点シェーダー
	psoDesc.PS = { pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize() }; // トーンマップ用ピクセルシェーダー
	psoDesc.RasterizerState = DirectX::CommonStates::CullNone; // ラスタライザーステート（カリングなし）
	psoDesc.BlendState = DirectX::CommonStates::Opaque; // ブレンドステート（不透明）
	psoDesc.DepthStencilState = DirectX::CommonStates::DepthDefault; // 深度ステンシルステート（デフォルト）
	psoDesc.SampleMask = UINT_MAX; // サンプルマスク（全サンプル）
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // プリミティブトポロジタイプ（三角形）
	psoDesc.NumRenderTargets = 1; // レンダーターゲットの数
	psoDesc.RTVFormats[0] = m_colorTarget[0].GetRTVDesc().Format; // レンダーターゲットのフォーマット
	psoDesc.DSVFormat = m_depthTarget.GetDSVDesc().Format; // 深度ステンシルビューのフォーマット
	psoDesc.SampleDesc.Count = 1; // マルチサンプリングのサンプル数
	psoDesc.SampleDesc.Quality = 0; // マルチサンプリングの品質

	// パイプラインステートを生成
	hr = m_pDevice->CreateGraphicsPipelineState(
		&psoDesc,								// パイプラインステートの設定
		IID_PPV_ARGS(m_pTonemapPSO.GetAddressOf()));	// パイプラインステートのポインタ
	if (FAILED(hr))
	{
		MessageBox(nullptr, "トーンマップ用パイプラインステートの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// =============================
	//	頂点バッファの作成
	// =============================
	struct Vertex
	{
		float px;
		float py; // 頂点位置
		float tx;
		float ty; // テクスチャ座標
	};
	
	if (!m_QuadVB.Init<Vertex>(m_pDevice.Get(), 3))
	{
		MessageBox(nullptr, "四角形の頂点バッファの初期化に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	Vertex* vertexPtr = m_QuadVB.Map<Vertex>(); // 頂点バッファのポインタを取得
	assert(vertexPtr); // ポインタがnullptrでないことを確認
	vertexPtr[0] = { -1.0f, 1.0f, 0.0f, -1.0f }; // 左下の頂点
	vertexPtr[1] = { 3.0f, 1.0f, 2.0f, -1.0f }; // 右下の頂点
	vertexPtr[2] = { -1.0f, -3.0f, 0.0f, 1.0f }; // 左上の頂点
	m_QuadVB.Unmap(); // 頂点バッファのアンマップ

	// ==============================
	//	壁用頂点バッファの生成
	// ==============================
	struct BasicVertex
	{
		DirectX::SimpleMath::Vector3 Position; // 頂点位置
		DirectX::SimpleMath::Vector3 Normal;   // 法線
		DirectX::SimpleMath::Vector3 TexCoord; // テクスチャ座標
		DirectX::SimpleMath::Vector3 Tangent; // 接線
	};

	if (!m_WallVB.Init<BasicVertex>(m_pDevice.Get(), 6))
	{
		MessageBox(nullptr, "壁用頂点バッファの初期化に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false;
	}

	float size = 10.0f;
	BasicVertex* basicVertexPtr = m_WallVB.Map<BasicVertex>(); // 壁用頂点バッファのポインタを取得
	assert(basicVertexPtr); // ポインタがnullptrでないことを確認

	basicVertexPtr[0].Position = { size, -size, 0.0f };
	basicVertexPtr[0].Normal = { 0.0f, 0.0f, 1.0f };
	basicVertexPtr[0].TexCoord = { 0.0f, 1.0f };
	basicVertexPtr[0].Tangent = { 1.0f, 0.0f, 0.0f };

	basicVertexPtr[1].Position = { size, size, 0.0f };
	basicVertexPtr[1].Normal = { 0.0f, 0.0f, 1.0f };
	basicVertexPtr[1].TexCoord = { 1.0f, 1.0f };
	basicVertexPtr[1].Tangent = { 1.0f, 0.0f, 0.0f };

	basicVertexPtr[2].Position = { size, -size, 0.0f };
	basicVertexPtr[2].Normal = { 0.0f, 0.0f, 1.0f };
	basicVertexPtr[2].TexCoord = { 0.0f, 1.0f };
	basicVertexPtr[2].Tangent = { 1.0f, 0.0f, 0.0f };

	basicVertexPtr[3].Position = { -size, size, 0.0f };
	basicVertexPtr[3].Normal = { 0.0f, 0.0f, 1.0f };
	basicVertexPtr[3].TexCoord = { 0.0f, 1.0f };
	basicVertexPtr[3].Tangent = { 1.0f, 0.0f, 0.0f };

	basicVertexPtr[4].Position = { size, -size, 0.0f };
	basicVertexPtr[4].Normal = { 0.0f, 0.0f, 1.0f };
	basicVertexPtr[4].TexCoord = { 1.0f, 0.0f };
	basicVertexPtr[4].Tangent = { 1.0f, 0.0f, 0.0f };

	basicVertexPtr[5].Position = { -size, -size, 0.0f };
	basicVertexPtr[5].Normal = { 0.0f, 0.0f, 1.0f };
	basicVertexPtr[5].TexCoord = { 0.0f, 0.0f };
	basicVertexPtr[5].Tangent = { 1.0f, 0.0f, 0.0f };

	m_WallVB.Unmap(); // 壁用頂点バッファのアンマップ

	for (uint32_t i = 0; i < FrameCount; ++i)
	{
		if (!m_TonemapCB[i].Init(
			m_pDevice.Get(),					// デバイス
			m_pPools[POOL_TYPE_RES],			// リソース用のディスクリプタプール
			sizeof(CbTonemap)))					// 定数バッファのサイズ
		{
			MessageBox(nullptr, "トーンマップ定数バッファの初期化に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return false; // エラー終了
		}
	}
#if 0
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
		L"Assets/hdr014.dds",				// テクスチャファイル名
		false,								// sRGBカラー空間を使用しない
		batch))								// リソースアップロードバッチ
	{
		MessageBox(nullptr, "テクスチャの初期化に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// バッチ終了
	auto future = batch.End(m_pQueue.Get()); // リソースアップロードバッチを終了

	future.wait(); // 非同期処理の完了を待機
#endif
	// ==============================
	//	変換行列用の定数バッファの生成
	// ==============================
	for (size_t i = 0; i < FrameCount; ++i)
	{
		// 定数バッファの初期化
		if (!m_TransformCB[i].Init(
			m_pDevice.Get(),			// デバイス
			m_pPools[POOL_TYPE_RES],	// リソース用のディスクリプタプール
			sizeof(CbTransform)))		// 定数バッファのサイズ
		{
			MessageBox(nullptr, "変換行列用定数バッファの初期化に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
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
		auto ptr = m_TransformCB[i].GetPtr<CbTransform>(); // 定数バッファのポインタを取得
		ptr->View = DirectX::XMMatrixLookAtLH(eyePos, targetPos, upDir); // ビュー行列を設定
		ptr->Proj = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, 1.0f, 100.0f); // プロジェクション行列を設定
	}

	m_RotateAngle = DirectX::XMConvertToRadians(-60.0f); // 初期回転角度を設定

	// ==============================
	//	メッシュ用バッファの生成
	// ==============================
	for (uint32_t i = 0; i < FrameCount; ++i)
	{
		if (!m_MeshCB[i].Init(
			m_pDevice.Get(),					// デバイス
			m_pPools[POOL_TYPE_RES],			// リソース用のディスクリプタプール
			sizeof(CbMesh)))					// 定数バッファのサイズ
		{
			MessageBox(nullptr, "メッシュ用定数バッファの初期化に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return false; // エラー終了
		}

		auto ptr = m_MeshCB[i].GetPtr<CbMesh>(); // 定数バッファのポインタを取得
		ptr->World = DirectX::XMMatrixIdentity(); // ワールド行列を単位行列に設定
	}

	return true; // 正常終了
}

void App::OnTerm()
{
	m_QuadVB.Term(); // 四角形の頂点バッファの解放
	for (uint32_t i = 0; i < FrameCount; ++i)
	{
		m_TonemapCB[i].Term(); // トーンマップ定数バッファの解放
		m_LightCB[i].Term(); // ライト定数バッファの解放
		m_CameraCB[i].Term(); // カメラ定数バッファの解放
		m_TransformCB[i].Term(); // 変換行列用定数バッファの解放
	}

	// メッシュ破棄
	for (auto& mesh : m_pMesh)
	{
		if (mesh)
		{
			mesh->Term(); // メッシュの解放
		}
	}
	m_pMesh.clear(); // メッシュのクリア
	m_pMesh.shrink_to_fit(); // メモリの最適化

	// マテリアル破棄
	m_material.Term();

	for (uint32_t i = 0; i < FrameCount; ++i)
	{
		m_MeshCB[i].Term(); // メッシュ用定数バッファの解放
	}

	m_SceneColorTarget.Term(); // シーン用カラーターゲットの解放
	m_SceneDepthTarget.Term(); // シーン用深度ターゲットの解放

	m_pScenePSO.Reset(); // シーン用パイプラインステートの解放
	m_SceneRootSig.Term(); // シーン用ルートシグネチャの解放

	m_pTonemapPSO.Reset(); // トーンマップ用パイプラインステートの解放
	m_TonemapRootSig.Term(); // トーンマップ用ルートシグネチャの解放
}

void App::OnRender()
{
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
#if 0
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
#endif

	pCmd->SetGraphicsRootSignature(m_pRootSig.Get()); // ルートシグネチャを設定
	pCmd->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps); // ディスクリプタヒープを設定
	pCmd->SetGraphicsRootConstantBufferView(0, m_CB[m_frameIndex].GetAddress());	// トーンマッピング定数バッファを設定
	pCmd->SetGraphicsRootDescriptorTable(1, m_texture.GetHandleGPU());	// テクスチャのハンドルを設定

	pCmd->SetPipelineState(m_pPSO.Get()); // パイプラインステートを設定
	pCmd->RSSetViewports(1, &m_Viewport);	// ビューポートを設定
	pCmd->RSSetScissorRects(1, &m_scissor); // シザー矩形を設定

	pCmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // プリミティブトポロジを設定
	auto vbView = m_quadVB.GetView();	// 頂点バッファビューを取得
	pCmd->IASetVertexBuffers(0, 1, &vbView);	// 頂点バッファを設定
	pCmd->DrawInstanced(3, 1, 0, 0); // 四角形を描画

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

void App::ChangeDisplayMode(bool hdr)
{
	if (hdr)
	{
		if (!IsSupportHDR())
		{
			MessageBox(nullptr, "HDRはサポートされていません。", "エラー", MB_OK | MB_ICONERROR);
			return; // HDRがサポートされていない場合は終了
		}

		HRESULT hr = m_pSwapChain->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);
		if (FAILED(hr))
		{
			MessageBox(nullptr, "HDRモードへの切り替えに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return; // HDRモードへの切り替えに失敗した場合は終了
		}

		DXGI_HDR_METADATA_HDR10 metaData{};
			
		// ITU-R BT.2100の原刺激と白色点を設定
		metaData.RedPrimary[0] = GetChromaticityCoord(0.64);
		metaData.RedPrimary[1] = GetChromaticityCoord(0.33);
		metaData.BluePrimary[0] = GetChromaticityCoord(0.3);
		metaData.BluePrimary[1] = GetChromaticityCoord(0.6);
		metaData.GreenPrimary[0] = GetChromaticityCoord(0.15);
		metaData.GreenPrimary[1] = GetChromaticityCoord(0.06);
		metaData.WhitePoint[0] = GetChromaticityCoord(0.3127);
		metaData.WhitePoint[1] = GetChromaticityCoord(0.329);

		// ディスプレイがサポートすると最大輝度値と最小輝度値を設定
		metaData.MaxMasteringLuminance = static_cast<UINT>(GetMaxLuminance() * 10000);
		metaData.MinMasteringLuminance = static_cast<UINT>(GetMinLuminance() * 0.001);

		// 最大値を100[nit]に設定
		metaData.MaxContentLightLevel = 100;

		hr = m_pSwapChain->SetHDRMetaData(
			DXGI_HDR_METADATA_TYPE_HDR10,			// メタデータのタイプ
			sizeof(DXGI_HDR_METADATA_TYPE_HDR10),	// メタデータのサイズ
			&metaData								// メタデータのポインタ
		);
		if (FAILED(hr))
		{
			MessageBox(nullptr, "HDRメタデータの設定に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return; // HDRメタデータの設定に失敗した場合は終了
		}

		m_BaseLuminance = 100.0f;
		m_MaxLuminance = 100.0f;
	}
}

void App::DrawScene(ID3D12GraphicsCommandList* pCmdList)
{
}

void App::DrawTonemap(ID3D12GraphicsCommandList* pCmdList)
{
}

void App::DrawMesh(ID3D12GraphicsCommandList* pCmdList)
{
}
