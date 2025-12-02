/*+===================================================================
	File: MeshRenderer.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/14 17:22:58 初回作成
	（これ以降下に更新日時と更新内容を書く）
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

	void Init(Model* pModel = nullptr);
	void Update() override final;
	void Draw() override final;
	void Uninit() override final;

private:
	void Init(std::vector<Model::Mesh> meshes);

	Model* m_pModel{};			// モデルデータ
	std::vector<Model::Mesh> m_Meshes{};

	std::vector<VertexBuffer*> m_pVertexBuffers{};	// 頂点バッファ
	std::vector<IndexBuffer*> m_pIndexBuffers{};	// インデックスバッファ
	ConstantBuffer* m_pWVPCB[FRAME_BUFFER_COUNT]{};	// 定数バッファ
	ConstantBuffer* m_pBoneMatrixCB[FRAME_BUFFER_COUNT]{}; // ボーン行列用定数バッファ
};
