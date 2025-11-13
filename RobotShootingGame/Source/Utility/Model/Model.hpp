/*+===================================================================
	File: Model.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/08 9:11:08 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Engine/Engine.hpp"
#include "Utility/VertexBuffer/VertexBuffer.hpp"
#include "Utility/ConstantBuffer/ConstantBuffer.hpp"
#include "Utility/RootSignature/RootSignature.hpp"
#include "Utility/PipelineState/PipelineState.hpp"
#include "Utility/IndexBuffer/IndexBuffer.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"
#include "Utility/ModelLoader/ModelLoader.hpp"
#include "Utility/DescriptorHeap/DescriptorHeap.hpp"
#include "Utility/Texture/Texture.hpp"
#include <vector>

// ==============================
//	前方宣言
// ==============================
class Camera;

/**
 * @brief Modelクラス
 */
class Model
{
public:
	/**
	 * コンストラクタ
	 */
	Model(std::vector<Mesh> Meshes, Camera& Camera);

	/**
	 * デストラクタ
	 */
	~Model();

	void Update();
	void Draw();

private:
	std::vector<Mesh> m_Meshes{};	// メッシュデータ
	Camera& m_Camera;				// カメラ
	std::vector<Texture*> m_pTextures{}; // テクスチャ

	std::vector<VertexBuffer*> m_pVertexBuffers{};	// 頂点バッファ
	std::vector<IndexBuffer*> m_pIndexBuffers{};	// インデックスバッファ
	ConstantBuffer* m_pConstantBuffer[FRAME_BUFFER_COUNT]{};	// 定数バッファ
	RootSignature* m_pRootSignature{};	// ルートシグネチャ
	PipelineState* m_pPipelineState{};	// パイプラインステート
	DescriptorHeap* m_pDescriptorHeap{}; // ディスクリプタヒープ
	std::vector<DescriptorHandle*> m_pMaterialHandles{}; // ディスクリプタハンドル
};
