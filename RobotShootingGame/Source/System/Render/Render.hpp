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
#include "System/RenderPass/RenderPass.hpp"
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
	void CopyBackBufferToFrameBuffer();

	void EnqueueRenderItem(const RenderItem& item);

	DepthStencil* GetDepthStencil() const { return m_pDepthStencil; }

private:
	friend class Singleton<Render>;

	template <typename T = RenderPass>
	inline void AddRenderPass()
	{
		T* pass{};

		// レンダーパスの追加
		if (!m_RenderPasses.size())
		{
			// 1つ目のレンダーパスはスナップショット用レンダーターゲットを渡す
			pass = new T(m_pSnapshotRT);
		}
		else
		{
			// 2つ目以降のレンダーパスは直前のレンダーターゲットを渡す
			pass = new T(m_RenderPasses.back()->GetRenderTarget());
		}
		
		pass->Init();
		m_RenderPasses.push_back(pass);
	}

	/**
	 * コンストラクタ
	 */
	Render();

	/**
	 * デストラクタ
	 */
	~Render();

	void InitRenderPasses();

	void DrawOpaque();
	void DrawTransparent();
	void DrawPostProcess();
	void WaitGPU();

	void SetGbufferRenderTargets();
	void SetSnapshotRenderTarget();
	void DrawOpaqueSprite();
	void ResetRenderItems();
	void DrawRenderItems(const std::vector<RenderItem>& renderItems);

	class DepthTexture* m_pDepthTexture{};					// 深度テクスチャ

	std::array<RenderTarget*, NumGbufferRT> m_GbufferRT{};	// Gバッファ用レンダーターゲット
	DepthStencil* m_pDepthStencil{};						// 深度ステンシルバッファ

	RenderTarget* m_pSnapshotRT{};							// スナップショット用レンダーターゲット

	float clearColor[4]{};
	
	std::vector<RenderItem> m_OpaqueRenderItems{};		// 不透明レンダーアイテムマップ
	std::vector<RenderItem> m_TransparentRenderItems{};	// 透明レンダーアイテムマップ

	std::vector<RenderPass*> m_RenderPasses{};			// レンダーパス配列

	VertexBuffer* m_pVertexBuffer{};		// 頂点バッファ
	IndexBuffer* m_pIndexBuffer{};			// インデックスバッファ
	ConstantBuffer* m_pWVPCB[FRAME_BUFFER_COUNT]{};	// 定数バッファ
	ConstantBuffer* m_pLightCB[FRAME_BUFFER_COUNT]{}; // ライト用定数バッファ
	ConstantBuffer* m_pCameraCB[FRAME_BUFFER_COUNT]{}; // カメラ用定数バッファ
	ConstantBuffer* m_pSSAOKernelCB[FRAME_BUFFER_COUNT]{};	// SSAOカーネル用定数バッファ
	
	DescriptorHeap* m_pSnapshotDescriptorHeap{};			// スナップショット用ディスクリプタヒープ
	std::vector<DescriptorHandle*> m_SnapshotRVHandles{};	// スナップショット用SRVハンドル配列
	RootSignature* m_pSnapshotRootSignature{};		// スナップショット用ルートシグネチャ
	PipelineState* m_pSnapshotPSO{};				// スナップショット用パイプラインステート
	
	DescriptorHeap* m_pFrameBufferDescriptorHeap{};				// フレームバッファ用ディスクリプタヒープ
	std::vector<DescriptorHandle*> m_FrameBufferRVHandles{};	// フレームバッファ用SRVハンドル配列
	RootSignature* m_pFrameBufferRootSignature{};	// フレームバッファ用ルートシグネチャ
	PipelineState* m_pFrameBufferPSO{};				// フレームバッファ用パイプラインステート
};
