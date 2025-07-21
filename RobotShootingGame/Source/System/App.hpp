/*+===================================================================
	File: App.hpp
	Summary: ウィンドウの作成をするヘッダファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/19 13:15 初回作成
			  /07/19 18:56 D3D12の初期化を追加
			  /07/20 22:53 描画処理を追加
			  /07/21 00:19 リファクタリング
			  /07/22 05:03 描画初期化処理を追加
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
#pragma comment(lib, "d3dcompiler.lib")

// ==============================
//	Type Definitions
// ==============================
template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

// ==============================
//	structs
// ==============================
struct alignas(256) Transform	// 定数バッファの構造体(256バイト境界でアライメント)
{
	DirectX::XMMATRIX World;	// ワールド行列
	DirectX::XMMATRIX View;		// ビュー行列
	DirectX::XMMATRIX Proj;		// プロジェクション行列
};

template<typename T>
struct ConstantBufferView
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC Desc;	// 定数バッファビューの説明
	D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU;	// CPUディスクリプタハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU;	// GPUディスクリプタハンドル
	T* pBuffer;								// 定数バッファのポインタ
};
 
/**
 * @brief Appクラス
 */
class App
{
public:
	/**
	 * @brief 指定された幅と高さでアプリケーションを初期化します。
	 * @param width アプリケーションウィンドウの幅（ピクセル単位）。
	 * @param height アプリケーションウィンドウの高さ（ピクセル単位）。
	 */
	App(uint32_t width, uint32_t height);

	/**
	 * @brief デストラクタ
	 */
	~App() = default;

	/**
	 * @brief アプリケーションを実行します。
	 */
	void Run();

private:
	bool InitApp();
	void TermApp();
	bool InitWnd();
	void TermWnd();
	void MainLoop();

	bool InitD3D();
	void TermD3D();
	void Render();
	void WaitGPU();
	void Present(uint32_t interval);
	bool OnInit();
	void OnTerm();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

	static const uint32_t FrameCount = 2;	// フレームバッファの数

	HINSTANCE m_hInst{};	// インスタンスハンドル
	HWND m_hWnd{};			// ウィンドウハンドル
	uint32_t m_unWidth{};	// ウィンドウの幅
	uint32_t m_unHeight{};	// ウィンドウの高さ

	ComPtr<ID3D12Device> m_pDevice{};								// Direct3D 12デバイス
	ComPtr<ID3D12CommandQueue> m_pQueue{};							// コマンドキュー
	ComPtr<IDXGISwapChain3> m_pSwapChain{};							// スワップチェーン
	ComPtr<ID3D12Resource> m_pColorBuffer[FrameCount]{};			// カラーバッファ
	ComPtr<ID3D12CommandAllocator> m_pCmdAllocater[FrameCount]{};	// コマンドアロケータ
	ComPtr<ID3D12GraphicsCommandList> m_pCmdList{};					// コマンドリスト
	ComPtr<ID3D12DescriptorHeap> m_pHeapRTV{};						// レンダーターゲットビューのヒープ(ディスクリプタヒープ)
	ComPtr<ID3D12Fence> m_pFence{};									// フェンス
	ComPtr<ID3D12DescriptorHeap> m_pHeapCBV{};						// 定数バッファビューのヒープ(ディスクリプタヒープ)
	ComPtr<ID3D12Resource> m_pVB{};									// 頂点バッファ
	ComPtr<ID3D12Resource> m_pCB[FrameCount]{};						// 定数バッファ(フレームごとに1つ)
	ComPtr<ID3D12RootSignature> m_pRootSignature{};					// ルートシグネチャ
	ComPtr<ID3D12PipelineState> m_pPSO{};							// パイプラインステートオブジェクト

	HANDLE m_hFenceEvent{};											// フェンスイベントハンドル
	uint64_t m_unFenceCounter[FrameCount]{};						// フェンスカウンター(フレームごとに1つ)
	uint32_t m_unFrameIndex{};										// 現在のフレームインデックス
	D3D12_CPU_DESCRIPTOR_HANDLE m_hRTV[FrameCount]{};				// レンダーターゲットビューのハンドル(CPUディスクリプタ)
	D3D12_VERTEX_BUFFER_VIEW m_VBV{};								// 頂点バッファビュー
	D3D12_VIEWPORT m_Viewport{};									// ビューポート
	D3D12_RECT m_Scissor{};											// シザーレクト
	ConstantBufferView<Transform> m_CBV[FrameCount]{};				// 定数バッファビュー
	float m_fRotateAngle{};											// 回転角度
};
