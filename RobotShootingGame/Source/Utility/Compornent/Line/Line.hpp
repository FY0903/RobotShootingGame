/*+===================================================================
	File: Line.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/22 19:06:18 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/Compornent/Component.hpp"
#include "System/Engine/Engine.hpp"

#include "Utility/ConstantBuffer/ConstantBuffer.hpp"
#include "Utility/RootSignature/RootSignature.hpp"
#include "Utility/VertexBuffer/VertexBuffer.hpp"
#include "Utility/PipelineState/PipelineState.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"

/**
 * @brief Lineクラス
 */
class Line : public Component
{
public:
	using Component::Component;

	/**
	 * コンストラクタ
	 */
	Line() = default;

	/**
	 * デストラクタ
	 */
	~Line() = default;

	void Update() override final;
	void Draw() override final;
	void Uninit() override final;

	void AddPoint(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT4& color);
	void Create();
	void Clear();

private:
	VertexBuffer* m_pVertexBuffer{};				// 頂点バッファ
	ConstantBuffer* m_pWVPCB[FRAME_BUFFER_COUNT]{};	// 定数バッファ

	std::vector<Vertex::Line> m_Lines{}; // 線の頂点データ
};
