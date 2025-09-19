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
#include "../../System/Engine/Engine.hpp"
#include "../../Utility/VertexBuffer/VertexBuffer.hpp"
#include "../../Utility/ConstantBuffer/ConstantBuffer.hpp"
#include "../../Utility/RootSignature/RootSignature.hpp"
#include "../../Utility/PipelineState/PipelineState.hpp"
#include "../../Utility/IndexBuffer/IndexBuffer.hpp"
#include "../../Utility/SharedStruct/SharedStruct.hpp"
#include "../../Utility/ModelLoader/ModelLoader.hpp"
#include "../../Utility/DescriptorHeap/DescriptorHeap.hpp"
#include "../../Utility/Texture/Texture.hpp"
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
	Model(ModelData ModelData, Camera& Camera);

	/**
	 * デストラクタ
	 */
	~Model();

	void Update();
	void Update(std::string AnimationName_1, int frame_1, std::string AnimationName_2, int frame_2, float blendRate);
	void Update(std::string AnimationName, int frame);
	void Draw();

	void AddAnimation(const aiScene* Scene, const std::string& AnimationName);

private:
	void UpdateBoneMatrix(aiNode* node, aiMatrix4x4 matrix);

	ModelData m_ModelData;		// モデルデータ
	Camera& m_Camera;				// カメラ
	std::vector<Texture*> m_pTextures{}; // テクスチャ

	std::vector<VertexBuffer*> m_pVertexBuffers{};	// 頂点バッファ
	std::vector<IndexBuffer*> m_pIndexBuffers{};	// インデックスバッファ
	ConstantBuffer* m_pTransformBuffer[FRAME_BUFFER_COUNT]{};	// 定数バッファ
	ConstantBuffer* m_pBoneBuffer[FRAME_BUFFER_COUNT]{};	// ボーン用定数バッファ
	RootSignature* m_pRootSignature{};	// ルートシグネチャ
	PipelineState* m_pPipelineState{};	// パイプラインステート
	DescriptorHeap* m_pDescriptorHeap{}; // ディスクリプタヒープ
	std::vector<DescriptorHandle*> m_pMaterialHandles{}; // ディスクリプタハンドル

	std::unordered_map<std::string, const aiScene*> m_Animations{}; // アニメーションデータ
};
