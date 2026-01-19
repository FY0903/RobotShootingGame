/*+===================================================================
	File: RenderPass.hpp
	Summary: RenderPassクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/30 01:11 初回作成
			26/01/15 10:21 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Engine/Engine.hpp"
#include "Utility/VertexBuffer/VertexBuffer.hpp"
#include "Utility/IndexBuffer/IndexBuffer.hpp"
#include "Utility/RootSignature/RootSignature.hpp"
#include "Utility/PipelineState/PipelineState.hpp"
#include "Utility/ConstantBuffer/ConstantBuffer.hpp"
#include "Utility/RenderTarget/RenderTarget.hpp"
#include "Utility/DepthStencil/DepthStencil.hpp"

/**
 * @brief RenderPassクラス
 */
class RenderPass
{
public:
	/**
	 * @brief 指定したレンダーターゲットに関連付けられた RenderPass オブジェクトを構築します。
	 * @param rt 使用する RenderTarget へのポインタ。
	 */
	RenderPass(class RenderTarget* rt);

	/**
	 * デストラクタ
	 */
	virtual ~RenderPass();

	virtual void Init() = 0;
	
	/**
	 * @brief 処理を実行します。
	 */
	void Execute();

	/**
	 * @brief 保持している RenderTarget オブジェクトへのポインタを取得します。
	 * @return RenderTarget へのポインタ。
	 */
	RenderTarget* GetRenderTarget() const { return m_pRT; }

protected:
	virtual void CreateRootSignature() = 0;
	virtual void CreatePSO() = 0;
	virtual void UpdateCB() {}

	ConstantBuffer* m_pWVPCB[FRAME_BUFFER_COUNT]{};	// 定数バッファ
	DescriptorHeap* m_pDescriptorHeap{};			// ディスクリプタヒープ
	std::vector<DescriptorHandle*> m_SRVHandles{};	// SRVハンドル配列
	RootSignature* m_pRootSignature{};				// ルートシグネチャ
	PipelineState* m_pPSO{};						// パイプラインステート

private:
	/**
	 * @brief レンダーターゲットの設定
	 */
	void SetRenderTarget();

	/**
	 * @brief スプライトの描画処理
	 */
	void DrawSprite();

	/**
	 * @brief GPUの待機
	 */
	void WaitGPU();

	RenderPass() = delete;

	RenderTarget* m_pRT{};

	VertexBuffer* m_pVB{};	// 頂点バッファ
	IndexBuffer* m_pIB{};	// インデックスバッファ
};
