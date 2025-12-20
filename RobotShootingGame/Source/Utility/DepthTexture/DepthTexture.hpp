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

	void Draw();

	RenderTarget* GetRenderTarget() const { return m_pDepthRT; }
	inline void SetRenderItems(const std::vector<Render::RenderItem>& items) { m_RenderItems = items; }

private:
	enum PSOType : size_t
	{
		Mesh,
		Sprite,
		NumPSO,
	};

	void CreateRootSignature();
	void CreatePSO();
	void SetRenderTarget();
	void DrawRenderItems();
	void WaitGPU();

	RenderTarget* m_pDepthRT{};						// 深度レンダーターゲット
	DepthStencil* m_pDepthStencil{};				// 深度ステンシル
	PipelineState* m_pPSOs[NumPSO]{};				// パイプラインステートオブジェクト
	RootSignature* m_pRootSignature{};				// ルートシグネチャ

	std::vector<Render::RenderItem> m_RenderItems{};
};
