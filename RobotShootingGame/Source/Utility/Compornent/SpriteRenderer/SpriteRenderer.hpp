/*+===================================================================
	File: SpriteRenderer.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/14 18:31:18 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Engine/Engine.hpp"
#include "Utility/Compornent/Component.hpp"
#include "Utility/Texture/Texture.hpp"
#include "Utility/VertexBuffer/VertexBuffer.hpp"
#include "Utility/IndexBuffer/IndexBuffer.hpp"
#include "Utility/ConstantBuffer/ConstantBuffer.hpp"
#include "Utility/DescriptorHeap/DescriptorHeap.hpp"
#include "Utility/RootSignature/RootSignature.hpp"
#include "Utility/PipelineState/PipelineState.hpp"

/**
 * @brief SpriteRendererクラス
 */
class SpriteRenderer : public Component
{
public:
	using Component::Component;

	void Init();
	void Update() override final;
	void Draw() override final;
	void Uninit() override final;

	inline void SetTexture(Texture* pTexture) { m_pTexture = pTexture; }

private:
	Texture* m_pTexture{}; // テクスチャ

	VertexBuffer* m_pVertexBuffer{};	// 頂点バッファ
	IndexBuffer* m_pIndexBuffer{};	// インデックスバッファ
	DescriptorHeap* m_pDescriptorHeap{}; // ディスクリプタヒープ
	DescriptorHandle* m_pMaterialHandle{}; // ディスクリプタハンドル
	RootSignature* m_pRootSignature{};	// ルートシグネチャ
	PipelineState* m_pPipelineState{};	// パイプラインステート
};
