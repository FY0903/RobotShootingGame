/*+===================================================================
	File: Mesh.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/26 14:52 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "../ModelLoader/ModelLoader.hpp"
#include "../VertexBuffer/VertexBuffer.hpp"
#include "../IndexBuffer/IndexBuffer.hpp"

/**
 * @brief Meshクラス
 */
class Mesh
{
public:
	/**
	 * コンストラクタ
	 */
	Mesh() = default;

	/**
	 * デストラクタ
	 */
	virtual ~Mesh();

	bool Init(ID3D12Device* pDevice, const MeshData& resource);

	void Term();

	void Draw(ID3D12GraphicsCommandList* pCmdList);

	inline uint32_t GetiMaterialId() const { return m_materialId; }

private:
	Mesh(const Mesh&) = delete;	// コピーコンストラクタを禁止
	void operator=(const Mesh&) = delete;	 // コピー代入演算子を禁止
	VertexBuffer m_VB;					// 頂点バッファ
	IndexBuffer m_IB;					// インデックスバッファ
	uint32_t m_materialId = UINT32_MAX;	// マテリアルID
	uint32_t m_indexCount{};			// インデックス数
};
