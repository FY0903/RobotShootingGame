/*+===================================================================
	File: Title.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/18 0:23:52 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Title.hpp"
#include "Utility/Compornent/SpriteRenderer/SpriteRenderer.hpp"
#include "Utility/MaterialManager/MaterialManager.hpp"
#include "Utility/TextureManager/TextureManager.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"

void Title::OnInit()
{
	// テクスチャの読み込み
	auto tex = TextureManager::GetInstance().LoadTexture("Assets/Texture/Title.png");

	// マテリアルの設定
	m_pMaterial = MaterialManager::GetInstance().CreateMaterial("UI");
	m_pMaterial->SetTexture(tex);

	AddComponent<SpriteRenderer>()->Init(true);
	m_Transform.Scale = { 3526.0f * 0.2f, 694.0f * 0.2f, 1.0f };
}

void Title::OnUpdate()
{
}

void Title::OnUninit()
{
}
