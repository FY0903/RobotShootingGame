/*+===================================================================
	File: Sprite.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/05 15:44:55 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "../Texture/Texture.hpp"
#include "../../System/Engine/Engine.hpp"
#include "../VertexBuffer/VertexBuffer.hpp"
#include "../IndexBuffer/IndexBuffer.hpp"
#include "../ConstantBuffer/ConstantBuffer.hpp"
#include "../DescriptorHeap/DescriptorHeap.hpp"
#include "../RootSignature/RootSignature.hpp"
#include "../PipelineState/PipelineState.hpp"
#include "../../Game/Camera/Camera.hpp"
#include <memory>

/**
 * @brief Spriteクラス
 */
class Sprite
{
public:
	/**
	 * コンストラクタ
	 */
	Sprite(std::shared_ptr<Texture> texture, Camera& camera);

	/**
	 * デストラクタ
	 */
	~Sprite();

	void Update();
	void Draw();

private:
	std::shared_ptr<Texture> m_pTexture{}; // テクスチャ
	Camera& m_Camera; // カメラ

	VertexBuffer* m_pVertexBuffer{};	// 頂点バッファ
	IndexBuffer* m_pIndexBuffer{};	// インデックスバッファ
	ConstantBuffer* m_pConstantBuffer[FRAME_BUFFER_COUNT]{};	// 定数バッファ
	DescriptorHeap* m_pDescriptorHeap{}; // ディスクリプタヒープ
	DescriptorHandle* m_pMaterialHandle{}; // ディスクリプタハンドル
	RootSignature* m_pRootSignature{};	// ルートシグネチャ
	PipelineState* m_pPipelineState{};	// パイプラインステート
};
