/*+===================================================================
	File: DepthTexture.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/19 15:51:14 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Engine/Engine.hpp"
#include "Utility/VertexBuffer/VertexBuffer.hpp"
#include "Utility/IndexBuffer/IndexBuffer.hpp"
#include "Utility/DescriptorHeap/DescriptorHeap.hpp"
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

	void Draw();

	RenderTarget* GetRenderTarget() const { return m_pDepthRT; }

private:
	void SetRenderTarget();
	void UpdateCB();
	void DrawSprite();
	void WaitGPU();

	RenderTarget* m_pDepthRT{};				// 深度レンダーターゲット
	DepthStencil* m_pDepthStencil{};		// 深度ステンシル
	VertexBuffer* m_pVertexBuffer{};		// 頂点バッファ
	ConstantBuffer* m_pWVPCB[FRAME_BUFFER_COUNT]{};	// 定数バッファ
	IndexBuffer* m_pIndexBuffer{};			// インデックスバッファ
	RootSignature* m_pRootSignature{};		// ルートシグネチャ
	PipelineState* m_pPipelineState{};		// パイプラインステート
};
