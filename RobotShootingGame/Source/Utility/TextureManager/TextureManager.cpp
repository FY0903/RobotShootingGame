/*+===================================================================
	File: TextureManager.cpp
	Summary: TextureManagerクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/01 11:17 初回作成
            26/01/16 11:48 コメント記載
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

Texture* TextureManager::CreateDefaultTexture(const std::string& textureName, BYTE R, BYTE G, BYTE B, BYTE A)
{
	// すでに作成されている場合はそれを返す
	if (auto it = m_textureMap.find(textureName); it != m_textureMap.end())
	{
		return it->second;
	}

	// デフォルトテクスチャを新規作成する
	Texture* pTexture = new Texture();
	if (FAILED(pTexture->CreateDefaultTexture(R, G, B, A)))
	{
		assert(0 && textureName.c_str() && "TextureManager.cpp デフォルトテクスチャの作成に失敗しました。");
		delete pTexture;
		return nullptr;
	}
	
	// マップに登録して返す
	m_textureMap[textureName] = pTexture;
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
