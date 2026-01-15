/*+===================================================================
	File: Line.hpp
	Summary: Lineクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/22 19:06 初回作成
			26/01/15 17:56 コメント記載
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

	/**
	 * @brief 開始点と終了点および色を指定してポイントを追加します。
	 * @param start ポイントの開始点。DirectX::XMFLOAT3 の const 参照。
	 * @param end ポイントの終了点。DirectX::XMFLOAT3 の const 参照。
	 * @param color ポイントの色。DirectX::XMFLOAT4 の const 参照。
	 */
	void AddPoint(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT4& color);
	
	/**
	 * @brief AddPointで追加した頂点データを基に頂点バッファを作成します。
	 */
	void Create();

	/**
	 * @brief 頂点データをクリアします。
	 */
	void Clear();

private:
	VertexBuffer* m_pVB{};											// 頂点バッファ
	std::array<ConstantBuffer*, FRAME_BUFFER_COUNT> m_pWVPCBs{};	// WVP用定数バッファ

	std::vector<Vertex::Line> m_Lines{}; // 線の頂点データ
};
