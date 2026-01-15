/*+===================================================================
	File: ModelManager.hpp
	Summary: ModelManagerクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/01 17:23 初回作成
			26/01/15 19:07 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/Model/Model.hpp"

/**
 * @brief ModelManagerクラス
 */
class ModelManager : public Singleton<ModelManager>
{
public:
	/**
	 * @brief 指定したファイルパスからモデルを読み込み、Model オブジェクトへのポインタを返します。
	 * @param modelPath 読み込むモデルファイルへのパスを表す文字列。
	 * @param inverseU U（横）テクスチャ座標を反転する場合に true。既定値は false。
	 * @param inverseV V（縦）テクスチャ座標を反転する場合に true。既定値は false。
	 * @return 読み込まれた Model オブジェクトへのポインタ。読み込みに失敗した場合は nullptr を返す可能性があります。
	 */
	Model* LoadModel(const std::string& modelPath, bool inverseU = false, bool inverseV = false);

private:
	friend class Singleton<ModelManager>;

	/**
	 * コンストラクタ
	 */
	ModelManager() = default;

	/**
	 * デストラクタ
	 */
	~ModelManager();

	std::unordered_map<std::string, Model*> m_modelMap;	// モデルマップ
};
