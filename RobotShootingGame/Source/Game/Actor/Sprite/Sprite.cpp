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
#include "Utility/SharedStruct/SharedStruct.hpp"

void Sprite::OnInit()
{
	// テクスチャの読み込み
	auto pDefTex = TextureManager::GetInstance().CreateDefaultTexture("DefTex");

	// マテリアルの設定
	m_pMaterial = MaterialManager::GetInstance().CreateMaterial("Water");
	m_pMaterial->SetTexture(pDefTex);

	AddComponent<SpriteRenderer>()->Init();

	// 定数バッファの設定
	m_pTimeCB = m_pMaterial->SetCBAtRegister(1, sizeof(CB::Time)); // レジスタ番号1に時間用定数バッファを設定
}

void Sprite::OnUpdate()
{
	size_t currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();

	CB::Time* pTime = m_pTimeCB[currentIndex]->GetPtr<CB::Time>();
	pTime->DeltaTime = Time::GetInstance().GetDeltaTimeSinceStartup();
}

void Sprite::OnUninit()
{
}
