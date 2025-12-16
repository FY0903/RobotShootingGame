/*+===================================================================
	File: TextureManager.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/01 11:17:51 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

/**
 * @brief TextureManagerクラス
 */
class TextureManager : public Singleton<TextureManager>
{
public:
	class Texture* LoadTexture(const std::string& texturePath);
	class Texture* CreateDefaultTexture(const std::string& textureName, BYTE R = 255, BYTE G = 255, BYTE B = 255, BYTE A = 255);

private:
	friend class Singleton<TextureManager>;

	/**
	 * コンストラクタ
	 */
	TextureManager() = default;

	/**
	 * デストラクタ
	 */
	~TextureManager();

	std::unordered_map<std::string, class Texture*> m_textureMap{}; // テクスチャマップ
};
