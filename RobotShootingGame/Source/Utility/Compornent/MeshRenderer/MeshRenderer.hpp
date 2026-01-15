/*+===================================================================
	File: MeshRenderer.hpp
	Summary: MeshRendererクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/14 17:22 初回作成
			26/01/15 18:02 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/Compornent/Component.hpp"
#include "Utility/Model/Model.hpp"
#include "Utility/Animation/Animation.hpp"
#include "System/Engine/Engine.hpp"
#include "Utility/VertexBuffer/VertexBuffer.hpp"
#include "Utility/ConstantBuffer/ConstantBuffer.hpp"
#include "Utility/RootSignature/RootSignature.hpp"
#include "Utility/PipelineState/PipelineState.hpp"
#include "Utility/IndexBuffer/IndexBuffer.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"
#include "Utility/DescriptorHeap/DescriptorHeap.hpp"

// ==============================
//	前方宣言
// ==============================
struct aiNode;

/**
 * @brief MeshRendererクラス
 */
class MeshRenderer : public Component
{
public:
	using Component::Component;

	/**
	 * @brief モデルの初期化を行います。オプションで初期化に使用する Model オブジェクトを指定できます。
	 * @param pModel 初期化に使用する Model オブジェクトへのポインタ。省略または nullptr の場合は既定の初期化を行います。
	 */
	void Init(Model* pModel = nullptr);
	
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
	/**
	 * @brief 与えられた Model::Mesh の一覧で初期化します。
	 * @param meshes 初期化に使用する Model::Mesh を格納した std::vector。
	 */
	void Init(std::vector<Model::Mesh> meshes);

	Model* m_pModel{};						// モデルデータ
	std::vector<Model::Mesh> m_Meshes{};	// メッシュ群

	std::vector<VertexBuffer*> m_pVertexBuffers{};	// 頂点バッファ
	std::vector<IndexBuffer*> m_pIndexBuffers{};	// インデックスバッファ

	std::array<ConstantBuffer*, FRAME_BUFFER_COUNT> m_pWVPCBs{}; // WVP用定数バッファ
	std::array<ConstantBuffer*, FRAME_BUFFER_COUNT> m_pBoneCBs{}; // ボーン用定数バッファ
};
