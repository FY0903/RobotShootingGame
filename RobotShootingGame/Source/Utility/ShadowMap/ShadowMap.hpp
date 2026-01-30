/*+===================================================================
	File: ShadowMap.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2026/01/19 3:25:58 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Engine/Engine.hpp"
#include "System/Render/Render.hpp"
#include "Utility/RootSignature/RootSignature.hpp"
#include "Utility/PipelineState/PipelineState.hpp"
#include "Utility/ConstantBuffer/ConstantBuffer.hpp"
#include "Utility/RenderTarget/RenderTarget.hpp"
#include "Utility/DepthStencil/DepthStencil.hpp"
#include "Utility/Compornent/Light/Light.hpp"

/**
 * @brief ShadowMapクラス
 */
class ShadowMap
{
public:
	enum Quality : uint32_t
	{
		LOW = 512,
		MIDDLE = 1024,
		HIGH = 2048,
		ULTRA = 4096,
	};

	/**
	 * コンストラクタ
	 * @param pLight ライトコンポーネントへのポインタ
	 * @param quality シャドウマップの品質（解像度）
	 */
	ShadowMap(Light* pLight, Quality quality);

	/**
	 * デストラクタ
	 */
	~ShadowMap();

	/**
	 * @brief 描画処理
	 */
	void Draw();

	/**
	 * @brief 現在の RenderTarget オブジェクトへのポインタを取得します。
	 * @return RenderTarget へのポインタ。
	 */
	RenderTarget* GetRenderTarget() const { return m_pRT; }

	/**
	 * @brief レンダーアイテムを設定します。
	 * @param items 設定するレンダーアイテムの配列。
	 */
	inline void SetRenderItems(const std::vector<Render::RenderItem>& items) { m_RenderItems = items; }

	/**
	 * @brief ライトコンポーネントを取得します。
	 * @return ライトコンポーネントへのポインタ (Light*)。
	 */
	Light* GetLight() const { return m_pLight; }

private:
	/**
	 * @brief レンダリングタイプ
	 */
	enum RenderType : size_t
	{
		SkeletalMesh,	// スケルタルメッシュ
		Mesh,			// メッシュ
		Sprite,			// スプライト
		DebugSprite,	// デバッグスプライト
		Num,
	};

	/**
	 * @brief ルートシグネチャの作成
	 */
	void CreateRootSignature();

	/**
	 * @brief パイプラインステートオブジェクトの作成
	 */
	void CreatePSO();

	/**
	 * @brief レンダーターゲットの設定
	 */
	void SetRenderTarget();

	/**
	 * @brief レンダーアイテムの描画
	 */
	void DrawRenderItems();

	/**
	 * @brief GPUの待機処理
	 */
	void WaitGPU();

	Light* m_pLight{};	// ライトコンポーネント
	std::vector<Render::RenderItem> m_RenderItems{};	// レンダーアイテム配列

	D3D12_VIEWPORT m_viewport{};						// ビューポート
	D3D12_RECT m_scissor{};								// シザー矩形
	RenderTarget* m_pRT{};								// レンダーターゲット
	DepthStencil* m_pDSV{};								// 深度ステンシル
	RootSignature* m_pRootsignatures[Num]{};			// ルートシグネチャ配列
	PipelineState* m_pPSOs[Num]{};						// パイプラインステートオブジェクト

	ConstantBuffer* m_pWVPCBs[FRAME_BUFFER_COUNT]{};		// WVP行列用定数バッファ
};
