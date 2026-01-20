/*+===================================================================
	File: DepthTexture.hpp
	Summary: DepthTextureクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/19 15:51 初回作成
			26/01/15 18:23 コメント記載
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

/**
 * @brief DepthTextureクラス
 */
class DepthTexture
{
public:
	/**
	 * コンストラクタ
	 */
	DepthTexture();

	/**
	 * デストラクタ
	 */
	~DepthTexture();

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

private:
	/**
	 * @brief レンダリングタイプ
	 */
	enum RenderType : size_t
	{
		SkeletalMesh,	// スケルタルメッシュ
		Mesh,			// メッシュ
		Sprite,			// スプライト
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

	RenderTarget* m_pRT{};							// 深度レンダーターゲット
	DepthStencil* m_pDSV{};							// 深度ステンシル
	RootSignature* m_pRootsignatures[Num]{};		// ルートシグネチャ配列
	PipelineState* m_pPSOs[Num]{};					// パイプラインステートオブジェクト

	std::vector<Render::RenderItem> m_RenderItems{};	// レンダーアイテム配列
};
