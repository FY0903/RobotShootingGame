/*+===================================================================
	File: Object.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/04 10:24:12 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "../../System/Engine/Engine.hpp"
#include "../../Utility/SharedStruct/SharedStruct.hpp"
#include "../../Utility/VertexBuffer/VertexBuffer.hpp"
#include "../../Utility/IndexBuffer/IndexBuffer.hpp"
#include "../../Utility/ConstantBuffer/ConstantBuffer.hpp"
#include "../../Utility/RootSignature/RootSignature.hpp"
#include "../../Utility/PipelineState/PipelineState.hpp"
#include "../../Utility/ModelLoader/ModelLoader.hpp"
#include <vector>

#include "../Camera/Camera.hpp"

/**
 * @brief Objectクラス
 */
class Object
{
public:
	/**
	 * コンストラクタ
	 */
	Object(Camera& camera);

	/**
	 * デストラクタ
	 */
	~Object();

	void Update();
	void Draw();

private:
	VertexBuffer* m_pVertexBuffer{};	// 頂点バッファ
	IndexBuffer* m_pIndexBuffer{};	// インデックスバッファ
	ConstantBuffer* m_pTransformBuffer[FRAME_BUFFER_COUNT]{};	// 定数バッファ
	RootSignature* m_pRootSignature{};	// ルートシグネチャ
	PipelineState* m_pPipelineState{};	// パイプラインステート
	std::vector<Mesh> m_Meshes{};		// メッシュデータ

	Camera& m_Camera; // カメラ
};
