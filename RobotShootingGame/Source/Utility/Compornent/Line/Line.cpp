/*+===================================================================
	File: Line.cpp
	Summary: Lineクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/22 19:06 初回作成
			26/01/15 17:56 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Line.hpp"
#include "Game/Actor/Actor.hpp"
#include "Utility/CameraManager/CameraManager.hpp"
#include "System/Render/Render.hpp"

void Line::Update()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();
	auto material = m_Owner->GetMaterial();

	// 定数バッファの更新
	CB::WVP* ptr = m_pWVPCBs[currentIndex]->GetPtr<CB::WVP>();
	ptr->WorldMat = m_Owner->GetTransform().GetWorldMatrixFloat4x4(false);
	ptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->Get3DViewMatrixFloat4x4(false);
	ptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->Get3DProjectionMatrixFloat4x4(false);
}

void Line::Draw()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();

	// レンダーアイテムの設定
	Render::RenderItem item{};
	item.pMaterial = m_Owner->GetMaterial();
	item.type = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	item.pVBs.push_back(m_pVB);
	item.meshSize = 1;
	item.indexCounts.push_back(static_cast<UINT>(m_Lines.size()));

	// レンダーキューに登録
	Render::GetInstance().EnqueueRenderItem(item);
}

void Line::Uninit()
{
	delete m_pVB;
	m_pVB = nullptr;
	m_pWVPCBs.fill(nullptr);
}

void Line::AddPoint(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT4& color)
{
	// 頂点データを追加
	m_Lines.push_back({ start, color });
	m_Lines.push_back({ end, color });
}

void Line::Create()
{
	auto vertexSize = sizeof(Vertex::Line) * m_Lines.size();
	auto vertexStride = sizeof(Vertex::Line);
	m_pVB = new VertexBuffer(vertexSize, vertexStride, m_Lines.data());

	auto material = m_Owner->GetMaterial();

	// 定数バッファの生成
	m_pWVPCBs = material->SetCBAtRegister(0, sizeof(CB::WVP)); // レジスタ番号0にWVP用定数バッファを設定
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		assert(m_pWVPCBs[i]);	// nullptrチェック
		CB::WVP* WVPptr = m_pWVPCBs[i]->GetPtr<CB::WVP>();
		WVPptr->WorldMat = m_Owner->GetTransform().GetWorldMatrixFloat4x4(false);
		WVPptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->Get3DViewMatrixFloat4x4(false);
		WVPptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->Get3DProjectionMatrixFloat4x4(false);
	}
}

void Line::Clear()
{
	m_Lines.clear();
}
