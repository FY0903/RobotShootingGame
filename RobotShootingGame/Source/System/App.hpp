/*+===================================================================
	File: App.hpp
	Summary: ウィンドウの作成をするヘッダファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/19 13:15 初回作成
			  /07/19 18:56 D3D12の初期化を追加
			  /07/20 22:53 描画処理を追加
			  /07/21 00:19 リファクタリング
			  /07/22 05:03 描画初期化処理を追加
			  /07/22 16:48 深度ステンシルビューの追加
			  /07/23 15:45 テクスチャ読み込み追加
			  /07/24 13:03 メッシュ読み込み追加
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
#include "../Utility/ComPtr.h"
#include "../Utility/Pool/DescriptorPool/DescriptorPool.hpp"
#include "ColorTarget/ColorTarget.hpp"
#include "DepthTarget/DepthTarget.hpp"
#include "CommandList/CommandList.hpp"
#include "Fence/Fence.hpp"
#include "../Utility/Mesh/Mesh.hpp"
#include "../Utility/Texture/Texture.hpp"

#include "../Utility/ConstantBuffer/ConstantBuffer.hpp"
#include "../Utility/Material/Material.hpp"

// ==============================
//	Linker
// ==============================
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

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
	bool InitD3D();
	void TermD3D();
	void MainLoop();

	void Present(uint32_t interval);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

	bool OnInit();
	void OnTerm();
	void OnRender();
	void OnMsgProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {}

	enum POOL_TYPE
	{
		POOL_TYPE_RES,	// CBV/SRV/UAV用のプール
		POOL_TYPE_SMP,	// サンプラ用のプール
		POOL_TYPE_RTV,	// レンダーターゲットビュー用のプール
		POOL_TYPE_DSV,	// 深度ステンシルビュー用のプール
		POOL_TYPE_COUNT	// プールの数
	};

	static const uint32_t FrameCount = 2;	// フレームバッファの数

	HINSTANCE m_hInst{};	// インスタンスハンドル
	HWND m_hWnd{};			// ウィンドウハンドル
	uint32_t m_unWidth{};	// ウィンドウの幅
	uint32_t m_unHeight{};	// ウィンドウの高さ

	ComPtr<ID3D12Device> m_pDevice{};				// Direct3D 12デバイス
	ComPtr<ID3D12CommandQueue> m_pQueue{};			// コマンドキュー
	ComPtr<IDXGISwapChain3> m_pSwapChain{};			// スワップチェーン
	ColorTarget m_colorTarget[FrameCount]{};		// カラーバッファ
	DepthTarget m_depthTarget{};					// 深度ステンシルバッファ
	DescriptorPool* m_pPools[POOL_TYPE_COUNT]{};	// ディスクリプタプールの配列
	CommandList m_commandList{};					// コマンドリスト
	Fence m_fence{};								// フェンス
	uint32_t m_frameIndex{};						// 現在のフレーム番号
	D3D12_VIEWPORT m_Viewport{};					// ビューポート
	D3D12_RECT m_scissor{};							// シザー矩形

	// ==============================
	//	各シーンに置くやつ(本来は)
	// ==============================
	std::vector<Mesh*> m_pMesh{};				// メッシュの配列
	std::vector<ConstantBuffer*> m_Transform{};	// 変換行列の定数バッファ
	ConstantBuffer* m_pLight{};					// ライトの定数バッファ
	Material m_material{};						// マテリアル
	ComPtr<ID3D12PipelineState> m_pPSO{};		// パイプラインステートオブジェクト
	ComPtr<ID3D12RootSignature> m_pRootSig{};	// ルートシグネチャ
	float m_RotateAngle{};						// 回転角度
};
