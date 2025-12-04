/*+===================================================================
	File: Sprite.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/24 8:33:45 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Sprite.hpp"
#include "Utility/Compornent/SpriteRenderer/SpriteRenderer.hpp"
#include "Utility/MaterialManager/MaterialManager.hpp"
#include "Utility/TextureManager/TextureManager.hpp"

void Sprite::OnInit()
{
	// テクスチャの読み込み
	auto pTexture = TextureManager::GetInstance().LoadTexture("Assets/Texture/ADC_COW.png");
	m_pRenderTarget = new RenderTarget();
	if (FAILED(m_pRenderTarget->Create(
		1280,
		720,
		DXGI_FORMAT_R8G8B8A8_UNORM)))
	{
		MessageBox(nullptr, "レンダーターゲットの作成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return;
	}

	// マテリアルの設定
	m_pMaterial = MaterialManager::GetInstance().CreateMaterial("Sprite");
	m_pMaterial->SetTexture(m_pRenderTarget);

	AddComponent<SpriteRenderer>()->Init();

	Engine::GetInstance().SetRenderTarget(m_pRenderTarget);
}

void Sprite::OnUpdate()
{
}

void Sprite::OnUninit()
{
}
