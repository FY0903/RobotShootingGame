/*+===================================================================
	File: RenderPass.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/30 1:11:18 初回作成
	（これ以降下に更新日時と更新内容を書く）
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
	 * コンストラクタ
	 */
	RenderPass(class RenderTarget* rt);

	/**
	 * デストラクタ
	 */
	~RenderPass();

	virtual void Init() = 0;

	void Execute();

	RenderTarget* GetRenderTarget() const { return m_pRT; }

protected:
	virtual void CreateRootSignature() = 0;
	virtual void CreatePSO() = 0;

	ConstantBuffer* m_pWVPCB[FRAME_BUFFER_COUNT]{};	// 定数バッファ
	DescriptorHeap* m_pSnapshotDescriptorHeap{};			// ディスクリプタヒープ
	std::vector<DescriptorHandle*> m_SnapshotRVHandles{};	// SRVハンドル配列
	RootSignature* m_pSnapshotRootSignature{};				// ルートシグネチャ
	PipelineState* m_pSnapshotPSO{};				// パイプラインステート

private:
	void SetRenderTarget();
	void DrawSprite();
	void WaitGPU();

	RenderPass() = delete;

	RenderTarget* m_pRT{};
	DepthStencil* m_pDSV{};

	VertexBuffer* m_pVB{};	// 頂点バッファ
	IndexBuffer* m_pIB{};	// インデックスバッファ
};
