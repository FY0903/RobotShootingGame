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
#include "System/Engine/Engine.hpp"
#include "Utility/RenderTarget/RenderTarget.hpp"
#include "Utility/DepthStencil/DepthStencil.hpp"

#include "Utility/VertexBuffer/VertexBuffer.hpp"
#include "Utility/IndexBuffer/IndexBuffer.hpp"
#include "Utility/ConstantBuffer/ConstantBuffer.hpp"
#include "Utility/DescriptorHeap/DescriptorHeap.hpp"
#include "Utility/RootSignature/RootSignature.hpp"
#include "Utility/PipelineState/PipelineState.hpp"

// ==============================
//	前方宣言
// ==============================
class Material;

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

	struct RenderItem
	{
		Material* pMaterial{};
		std::vector<VertexBuffer*> pVertexBuffers{};
		std::vector<IndexBuffer*> pIndexBuffers{};
		std::vector<UINT> indexCounts{};
		size_t MeshSize{};
		D3D_PRIMITIVE_TOPOLOGY type{};
		float sortKey{};
	};

	void Init();
	void Draw();

	void EnqueueRenderItem(const RenderItem& item);

	DepthStencil* GetDepthStencil() const { return m_pDepthStencil; }

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

	void DrawOpaque();
	void DrawTransparent();

	void SetGbufferRenderTargets();
	void SetBackBufferRenderTarget();
	void DrawBackBuffer();
	void ResetRenderItems();
	void DrawRenderItems(const std::vector<RenderItem>& renderItems);

	class DepthTexture* m_pDepthTexture{};					// 深度テクスチャ

	std::array<RenderTarget*, NumGbufferRT> m_GbufferRT{};	// Gバッファ用レンダーターゲット
	DepthStencil* m_pDepthStencil{};						// 深度ステンシルバッファ

	RenderTarget* m_pBackBufferRT{};						// バックバッファ用レンダーターゲット

	float clearColor[4]{};
	
	std::vector<RenderItem> m_OpaqueRenderItems{};		// 不透明レンダーアイテムマップ
	std::vector<RenderItem> m_TransparentRenderItems{};	// 透明レンダーアイテムマップ

	VertexBuffer* m_pVertexBuffer{};		// 頂点バッファ
	IndexBuffer* m_pIndexBuffer{};			// インデックスバッファ
	ConstantBuffer* m_pWVPCB[FRAME_BUFFER_COUNT]{};	// 定数バッファ
	ConstantBuffer* m_pLightCB[FRAME_BUFFER_COUNT]{}; // ライト用定数バッファ
	ConstantBuffer* m_pCameraCB[FRAME_BUFFER_COUNT]{}; // カメラ用定数バッファ
	ConstantBuffer* m_pSSAOKernelCB[FRAME_BUFFER_COUNT]{}; // SSAOカーネル用定数バッファ
	DescriptorHeap* m_pDescriptorHeap{};	// ディスクリプタヒープ
	std::vector<DescriptorHandle*> m_SRVHandles{}; // SRVハンドル配列
	RootSignature* m_pRootSignature{};		// ルートシグネチャ
	PipelineState* m_pPipelineState{};		// パイプラインステート
};
