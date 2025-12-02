/*+===================================================================
	File: ModelManager.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/01 17:23:28 初回作成
	（これ以降下に更新日時と更新内容を書く）
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

	std::unordered_map<std::string, Model*> m_modelMap;
};
