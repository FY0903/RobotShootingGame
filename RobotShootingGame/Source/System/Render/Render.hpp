/*+===================================================================
	File: Render.hpp
	Summary: 描画処理を管理するクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/09 19:24 初回作成
			26/01/15 09:30 コメント記載
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
	/**
	 * @brief Gバッファのレンダーターゲットの種類
	 */
	enum GbufferRT : size_t
	{
		Albedo,			// アルベド
		Normal,			// 法線
		WorldPos,		// ワールド座標
		Num,
	};

	/**
	 * @brief レンダーアイテム
	 */
	struct RenderItem
	{
		Material* pMaterial{};				// マテリアル
		std::vector<VertexBuffer*> pVBs{};	// 頂点バッファ
		std::vector<IndexBuffer*> pIBs{};	// インデックスバッファ
		std::vector<UINT> indexCounts{};	// インデックスカウント配列
		size_t meshSize{};					// メッシュ数
		D3D_PRIMITIVE_TOPOLOGY type{};		// プリミティブトポロジー
		float sortKey{};					// ソートキー（透明度ソート用）
	};

	/**
	 * @brief 初期化処理
	 */
	void Init();

	/**
	 * @brief 描画処理
	 */
	void Draw();

	/**
	 * @brief バックバッファにスナップショットレンダーターゲットをコピー
	 */
	void CopyBackBufferToFrameBuffer();

	/**
	 * @brief レンダーアイテムをレンダーキューに追加します。
	 * @param item キューに追加するレンダーアイテム。
	 */
	void EnqueueRenderItem(const RenderItem& item);
	
	/**
	 * @brief 現在の DepthStencil オブジェクトへのポインタを取得します。
	 * @return DepthStencil へのポインタ。
	 */
	DepthStencil* GetDepthStencil() const { return m_pDepthStencil; }

private:
	friend class Singleton<Render>;
	
	/**
	 * @brief レンダーパスのインスタンスを作成・初期化します。
	 * @tparam T 作成するレンダーパスの型。デフォルトは RenderPass。
	 */
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

		// 初期化
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

	/**
	 * @brief レンダーパスの初期化処理
	 */
	void InitRenderPasses();

	/**
	 * @brief 不透明オブジェクトの描画処理
	 */
	void DrawOpaque();

	/**
	 * @brief 透明オブジェクトの描画処理
	 */
	void DrawTransparent();

	/**
	 * @brief ポストプロセスの描画処理
	 */
	void DrawPostProcess();

	/**
	 * @brief GPUの待機処理
	 */
	void WaitGPU();

	/**
	 * @brief Gバッファ用レンダーターゲットをセット
	 */
	void SetGbufferRenderTargets();

	/**
	 * @brief スナップショット用レンダーターゲットをセット
	 */
	void SetSnapshotRenderTarget();

	/**
	 * @brief 不透明スプライトの描画処理
	 */
	void DrawOpaqueSprite();

	/**
	 * @brief レンダーアイテムのリセット処理
	 */
	void ResetRenderItems();

	/**
	 * @brief RenderItem オブジェクトの一覧を受け取り、それらを描画します。
	 * @param renderItems 描画対象の RenderItem を格納した std::vector への const 参照。
	 */
	void DrawRenderItems(const std::vector<RenderItem>& renderItems);

	class DepthTexture* m_pDepthTexture{};			// 深度テクスチャ

	std::array<RenderTarget*, Num> m_GbufferRT{};	// Gバッファ用レンダーターゲット
	DepthStencil* m_pDepthStencil{};				// 深度ステンシルバッファ

	RenderTarget* m_pSnapshotRT{};					// スナップショット用レンダーターゲット

	float clearColor[4]{};	// クリアカラー
	
	std::vector<RenderItem> m_OpaqueRenderItems{};		// 不透明レンダーアイテムマップ
	std::vector<RenderItem> m_TransparentRenderItems{};	// 透明レンダーアイテムマップ

	std::vector<RenderPass*> m_RenderPasses{};			// レンダーパス配列

	VertexBuffer* m_pVB{};									// 頂点バッファ
	IndexBuffer* m_pIB{};									// インデックスバッファ
	ConstantBuffer* m_pWVPCB[FRAME_BUFFER_COUNT]{};			// 定数バッファ
	ConstantBuffer* m_pLightCB[FRAME_BUFFER_COUNT]{};		// ライト用定数バッファ
	ConstantBuffer* m_pCameraCB[FRAME_BUFFER_COUNT]{};		// カメラ用定数バッファ
	ConstantBuffer* m_pSSAOKernelCB[FRAME_BUFFER_COUNT]{};	// SSAOカーネル用定数バッファ
	
	DescriptorHeap* m_pSnapshotDescriptorHeap{};			// スナップショット用ディスクリプタヒープ
	std::vector<DescriptorHandle*> m_SnapshotRVHandles{};	// スナップショット用SRVハンドル配列
	RootSignature* m_pSnapshotRootSignature{};				// スナップショット用ルートシグネチャ
	PipelineState* m_pSnapshotPSO{};						// スナップショット用パイプラインステート
	
	DescriptorHeap* m_pFrameBufferDescriptorHeap{};				// フレームバッファ用ディスクリプタヒープ
	std::vector<DescriptorHandle*> m_FrameBufferRVHandles{};	// フレームバッファ用SRVハンドル配列
	RootSignature* m_pFrameBufferRootSignature{};				// フレームバッファ用ルートシグネチャ
	PipelineState* m_pFrameBufferPSO{};							// フレームバッファ用パイプラインステート
};
