/*+===================================================================
	File: TextureManager.hpp
	Summary: TextureManagerクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/01 11:17 初回作成
            26/01/16 11:48 コメント記載
===================================================================+*/
#pragma once

/**
 * @brief TextureManagerクラス
 */
class TextureManager : public Singleton<TextureManager>
{
public:
	/**
	 * @brief 指定したパスのテクスチャを読み込み、Texture クラスのインスタンスへのポインタを返します。
	 * @param texturePath 読み込むテクスチャファイルのパス（相対または絶対パス）。
	 * @return 読み込まれた Texture オブジェクトへのポインタ。読み込みに失敗した場合は nullptr を返します。
	 */
	class Texture* LoadTexture(const std::string& texturePath);

	/**
	 * @brief 指定した名前と色でデフォルトのテクスチャを作成します。
	 * @param textureName 作成するテクスチャの名前または識別子。
	 * @param R 赤成分（BYTE、0~255）。
	 * @param B 青成分（BYTE、0~255）。
	 * @param G 緑成分（BYTE、0~255）。
	 * @param A アルファ成分（BYTE、0~255）。
	 * @return 作成された Texture オブジェクトへのポインタ。所有権やライフサイクルは実装に依存します。
	 */
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
