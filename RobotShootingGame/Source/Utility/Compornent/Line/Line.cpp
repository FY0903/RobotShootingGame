/*+===================================================================
	File: Line.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/22 19:06:18 初回作成
	（これ以降下に更新日時と更新内容を書く）
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
	CB::WVP* ptr = m_pWVPCB[currentIndex]->GetPtr<CB::WVP>();

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
	item.pWVPCB = m_pWVPCB[currentIndex];
	item.type = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	item.pVertexBuffers.push_back(m_pVertexBuffer);
	item.MeshSize = 1;
	item.indexCounts.push_back(static_cast<UINT>(m_Lines.size()));

	// レンダーキューに登録
	Render::GetInstance().EnqueueRenderItem(item);
}

void Line::Uninit()
{
	delete m_pVertexBuffer;
	m_pVertexBuffer = nullptr;

	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		delete m_pWVPCB[i];
		m_pWVPCB[i] = nullptr;
	}
}

void Line::AddPoint(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT4& color)
{
	m_Lines.push_back({ start, color });
	m_Lines.push_back({ end, color });
}

void Line::Create()
{
	auto vertexSize = sizeof(Vertex::Line) * m_Lines.size();
	auto vertexStride = sizeof(Vertex::Line);
	m_pVertexBuffer = new VertexBuffer(vertexSize, vertexStride, m_Lines.data());

	// 定数バッファの生成
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_pWVPCB[i] = new ConstantBuffer(sizeof(CB::WVP));
		assert(m_pWVPCB[i]);	// nullptrチェック
		CB::WVP* ptr = m_pWVPCB[i]->GetPtr<CB::WVP>();
		ptr->WorldMat = m_Owner->GetTransform().GetWorldMatrixFloat4x4();
		ptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->Get3DViewMatrixFloat4x4();
		ptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->Get3DProjectionMatrixFloat4x4();
	}
}

void Line::Clear()
{
	m_Lines.clear();
}
