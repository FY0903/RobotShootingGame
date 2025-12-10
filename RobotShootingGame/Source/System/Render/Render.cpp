/*+===================================================================
	File: Render.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/09 19:24:38 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Render.hpp"

Render::Render()
{
	for (auto& rtv : m_pDefaultRenderTargets)
	{
		rtv = new RenderTarget();
	}

	clearColor[0] = 0.0f;
	clearColor[1] = 0.0f;
	clearColor[2] = 0.0f;
	clearColor[3] = 1.0f;
}

Render::~Render()
{
}

void Render::Init()
{
	m_pDefaultRenderTargets[0]->Create(WINDOW_WIDTH, WINDOW_HEIGHT, DXGI_FORMAT_R32_FLOAT, clearColor);
	m_pDefaultRenderTargets[1]->Create(WINDOW_WIDTH, WINDOW_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, clearColor);
	m_pDefaultRenderTargets[2]->Create(WINDOW_WIDTH, WINDOW_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT, clearColor);
}

void Render::Uninit()
{
}
