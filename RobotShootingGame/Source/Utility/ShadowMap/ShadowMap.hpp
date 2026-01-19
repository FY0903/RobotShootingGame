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
#include "Utility/VertexBuffer/VertexBuffer.hpp"
#include "Utility/IndexBuffer/IndexBuffer.hpp"
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
	/**
	 * コンストラクタ
	 */
	ShadowMap();

	/**
	 * デストラクタ
	 */
	~ShadowMap() = default;

	void Init();

private:
	RenderTarget* m_pRT{};
	DepthStencil* m_pDSV{};

	VertexBuffer* m_pVB{};	// 頂点バッファ
	IndexBuffer* m_pIB{};	// インデックスバッファ
	ConstantBuffer* m_pWVPCB[FRAME_BUFFER_COUNT]{};	// 定数バッファ
	RootSignature* m_pRootSignature{};				// ルートシグネチャ
	PipelineState* m_pPSO{};						// パイプラインステート
};
