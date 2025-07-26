/*+===================================================================
	File: Mesh.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/26 14:52 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Mesh.hpp"

Mesh::~Mesh()
{
	Term();
}

bool Mesh::Init(ID3D12Device* pDevice, const MeshData& resource)
{
	if (!pDevice) return false;

	// 頂点バッファの初期化
	if (!m_VB.Init(pDevice, sizeof(MeshData) * resource.Vertices.size(), resource.Vertices.data())) return false;

	// インデックスバッファの初期化
	if (!m_IB.Init(pDevice, sizeof(uint32_t) * resource.Indices.size(), resource.Indices.data())) return false;

	m_materialId = resource.MaterialId;
	m_indexCount = static_cast<uint32_t>(resource.Indices.size());

	return true; // 成功
}

void Mesh::Term()
{
	m_VB.Term(); // 頂点バッファの終了処理
	m_IB.Term(); // インデックスバッファの終了処理
	m_materialId = UINT32_MAX; // マテリアルIDを初期化
	m_indexCount = 0; // インデックス数を初期化
}

void Mesh::Draw(ID3D12GraphicsCommandList* pCmdList)
{
	auto VBV = m_VB.GetView();
	auto IBV = m_IB.GetView();
	pCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // プリミティブトポロジを設定
	pCmdList->IASetVertexBuffers(0, 1, &VBV); // 頂点バッファを設定
	pCmdList->IASetIndexBuffer(&IBV); // インデックスバッファを設定
	pCmdList->DrawIndexedInstanced(m_indexCount, 1, 0, 0, 0); // インデックス付き描画を実行
}
