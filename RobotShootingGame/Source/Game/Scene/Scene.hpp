/*+===================================================================
	File: Scene.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/02 14:54:10 初回作成
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

#include "../Object/Object.hpp"
#include "../Camera/Camera.hpp"
#include "../../Utility/Sprite/Sprite.hpp"

/**
 * @brief Sceneクラス
 */
class Scene
{
public:
	/**
	 * コンストラクタ
	 */
	Scene() = default;

	/**
	 * デストラクタ
	 */
	~Scene() = default;

	void Init();
	void Update();
	void Draw();
	void UnInit();

private:
#if 0
	std::vector<VertexBuffer*> m_pVertexBuffers{};	// 頂点バッファ
	std::vector<IndexBuffer*> m_pIndexBuffers{};	// インデックスバッファ
	ConstantBuffer* m_pConstantBuffer[FRAME_BUFFER_COUNT]{};	// 定数バッファ
	RootSignature* m_pRootSignature{};	// ルートシグネチャ
	PipelineState* m_pPipelineState{};	// パイプラインステート
	std::vector<Mesh> m_Meshes{};		// メッシュデータ
	DescriptorHeap* m_pDescriptorHeap{}; // ディスクリプタヒープ
	std::vector<DescriptorHandle*> m_pMaterialHandles{}; // ディスクリプタハンドル
#endif

	Object* m_pObject{}; // Object
	Camera* m_pCamera{}; // カメラ
	Sprite* m_pSprite{}; // スプライト
};
