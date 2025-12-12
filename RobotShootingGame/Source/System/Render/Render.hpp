/*+===================================================================
	File: Render.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/09 19:24:38 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/RenderTarget/RenderTarget.hpp"
#include "Utility/DepthStencil/DepthStencil.hpp"

#include "System/Engine/Engine.hpp"

#include "Utility/VertexBuffer/VertexBuffer.hpp"
#include "Utility/IndexBuffer/IndexBuffer.hpp"
#include "Utility/ConstantBuffer/ConstantBuffer.hpp"
#include "Utility/DescriptorHeap/DescriptorHeap.hpp"
#include "Utility/RootSignature/RootSignature.hpp"
#include "Utility/PipelineState/PipelineState.hpp"

/**
 * @brief Renderクラス
 */
class Render : public Singleton<Render>
{
public:
	enum GbufferRT : size_t
	{
		Albedo,
		Normal,
		WorldPos,
		NumGbufferRT,
	};

	void Init();

	void BeginDraw();
	void EndDraw();

	void DrawBackBuffer();

private:
	friend class Singleton<Render>;

	/**
	 * コンストラクタ
	 */
	Render();

	/**
	 * デストラクタ
	 */
	~Render();

	std::array<RenderTarget*, NumGbufferRT> m_GbufferRT{};	// Gバッファ用レンダーターゲット
	DepthStencil* m_pDepthStencil{};						// 深度ステンシルバッファ

	float clearColor[4]{};

	VertexBuffer* m_pVertexBuffer{};		// 頂点バッファ
	IndexBuffer* m_pIndexBuffer{};			// インデックスバッファ
	ConstantBuffer* m_pCB[FRAME_BUFFER_COUNT]{};	// 定数バッファ
	DescriptorHeap* m_pDescriptorHeap{};	// ディスクリプタヒープ
	std::vector<DescriptorHandle*> m_SRVHandles{}; // SRVハンドル配列
	RootSignature* m_pRootSignature{};		// ルートシグネチャ
	PipelineState* m_pPipelineState{};		// パイプラインステート
};
