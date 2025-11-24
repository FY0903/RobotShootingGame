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

void Sprite::OnInit()
{
	m_pTexture = new Texture();
	if (FAILED(m_pTexture->Load("Assets/Texture/ADC_COW.png")))
	{
		assert(false && "Sprite.cpp テクスチャの読み込みに失敗しました。");
	}

	AddComponent<SpriteRenderer>()->Init(m_pTexture);
}

void Sprite::OnUninit()
{
	delete m_pTexture;
	m_pTexture = nullptr;
}
