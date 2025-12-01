/*+===================================================================
	File: TextureManager.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/01 11:17:51 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "TextureManager.hpp"
#include "Utility/Texture/Texture.hpp"

Texture* TextureManager::LoadTexture(const std::string& texturePath)
{
	// すでに読み込まれている場合はそれを返す
	if (auto it = m_textureMap.find(texturePath); it != m_textureMap.end())
	{
		return it->second;
	}

	// テクスチャを新規作成して読み込む
	Texture* pTexture = new Texture();
	if (FAILED(pTexture->Load(texturePath)))
	{
		assert(0 && texturePath.c_str() && "TextureManager.cpp テクスチャの読み込みに失敗しました。");
		delete pTexture;
		return nullptr;
	}

	// マップに登録して返す
	m_textureMap[texturePath] = pTexture;
	return pTexture;
}

TextureManager::~TextureManager()
{
	for (auto& pair : m_textureMap)
	{
		delete pair.second;
		pair.second = nullptr;
	}
	m_textureMap.clear();
}
