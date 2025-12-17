/*+===================================================================
	File: TitleText.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/18 2:42:57 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "TitleText.hpp"
#include "Utility/Compornent/SpriteRenderer/SpriteRenderer.hpp"
#include "Utility/MaterialManager/MaterialManager.hpp"
#include "Utility/TextureManager/TextureManager.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"

void TitleText::OnInit()
{
	// テクスチャの読み込み
	auto tex = TextureManager::GetInstance().LoadTexture("Assets/Texture/TitleText.png");

	// マテリアルの設定
	m_pMaterial = MaterialManager::GetInstance().CreateMaterial("Text");
	m_pMaterial->SetTexture(tex);

	AddComponent<SpriteRenderer>()->Init(true);
	m_Transform.Position = { 0.0f, -100.0f, 0.0f };
	m_Transform.Scale = { 2062.0f * 0.2f, 520.0f * 0.2f, 1.0f };

	// 定数バッファの設定
	m_pPSTimeCB = m_pMaterial->SetCB(sizeof(CB::Time));
}

void TitleText::OnUpdate()
{
	size_t currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();
	CB::Time* pTime = m_pPSTimeCB[currentIndex]->GetPtr<CB::Time>();
	pTime->DeltaTime = Time::GetInstance().GetDeltaTimeSinceStartup();
}

void TitleText::OnUninit()
{
}
