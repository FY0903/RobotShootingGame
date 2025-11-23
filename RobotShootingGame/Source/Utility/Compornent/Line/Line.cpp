#include "Line.hpp"
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

void Line::Update()
{
}

void Line::Draw()
{
}

void Line::Uninit()
{
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
	
	m_pRootSignature = new RootSignature();
	assert(m_pRootSignature);	// nullptrチェック
}

void Line::Clear()
{
}
