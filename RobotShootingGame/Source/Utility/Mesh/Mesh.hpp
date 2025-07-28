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

	/**
	 * @brief ID3D12Device オブジェクトと MeshData リソースを使用して初期化を行います。
	 * @param [pDevice] 初期化に使用する ID3D12Device へのポインタ。
	 * @param [resource] 初期化対象の MeshData リソース。
	 * @return 初期化が成功した場合は true、失敗した場合は false を返します。
	 */
	bool Init(ID3D12Device* pDevice, const MeshData& resource);

	/**
	 * @brief 終了処理を実行します。
	 */
	void Term();

	/**
	 * @brief 指定されたコマンドリストを使用して描画処理を実行します。
	 * @param [pCmdList] 描画コマンドを記録するための ID3D12GraphicsCommandList へのポインタ。
	 */
	void Draw(ID3D12GraphicsCommandList* pCmdList);

	/**
	 * @brief マテリアルIDを取得します。
	 * @return マテリアルID（uint32_t型の値）を返します。
	 */
	inline uint32_t GetMaterialId() const { return m_materialId; }

private:
	Mesh(const Mesh&) = delete;	// コピーコンストラクタを禁止
	void operator=(const Mesh&) = delete;	 // コピー代入演算子を禁止
	VertexBuffer m_VB;					// 頂点バッファ
	IndexBuffer m_IB;					// インデックスバッファ
	uint32_t m_materialId = UINT32_MAX;	// マテリアルID
	uint32_t m_indexCount{};			// インデックス数
};
