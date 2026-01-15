/*+===================================================================
	File: SpriteRenderer.hpp
	Summary: SpriteRendererクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/14 18:31 初回作成
			26/01/15 18:10 コメント記載
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

	/**
	 * @brief オブジェクトやモジュールの初期化を行います。is2D が true の場合は 2D モードで初期化します。
	 * @param is2D 2D モードで初期化する場合は true。
	 */
	void Init(bool is2D = false);

	/**
	 * @brief 指定したセル数と間隔で初期化する関数。
	 * @param cell 初期化するセルの数（要素数）。size_t 型で指定します。
	 * @param spacing セル間の間隔を表す浮動小数点値（float）。
	 */
	void Init(size_t cell, float spacing);

	/**
	 * @brief 更新処理
	 */
	void Update() override final;

	/**
	 * @brief 描画処理
	 */
	void Draw() override final;

	/**
	 * @brief 終了処理
	 */
	void Uninit() override final;

private:

	VertexBuffer* m_pVB{};	// 頂点バッファ
	IndexBuffer* m_pIB{};	// インデックスバッファ

	std::array<ConstantBuffer*, FRAME_BUFFER_COUNT> m_pWVPCBs{}; // WVP用定数バッファ
	int m_IndexCount{};	// インデックス数
	bool m_Is2D{};		// 2D描画かどうか
};
