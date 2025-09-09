/*+===================================================================
	File: Line.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/08 19:15:43 初回作成
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
#include <vector>
#include <SimpleMath.h>

// ==============================
//	前方宣言
// ==============================
class Camera;

/**
 * @brief Lineクラス
 */
class Line
{
public:
	/**
	 * コンストラクタ
	 */
	Line(Camera& camera);

	/**
	 * デストラクタ
	 */
	~Line();

	void Update();
	void Draw();

	void AddPoint(DirectX::SimpleMath::Vector3 start, DirectX::SimpleMath::Vector3 end, DirectX::SimpleMath::Vector4 color);
	void Create();

private:
	VertexBuffer* m_pVertexBuffer{};	// 頂点バッファ
	ConstantBuffer* m_pConstantBuffer[FRAME_BUFFER_COUNT]{};	// 定数バッファ
	RootSignature* m_pRootSignature{};	// ルートシグネチャ
	PipelineState* m_pPipelineState{};	// パイプラインステート
	std::vector<Vertex::Line> m_Lines{}; // 線の頂点データ

	Camera& m_Camera; // カメラ
};
