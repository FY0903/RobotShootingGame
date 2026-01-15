/*+===================================================================
	File: ModelManager.cpp
	Summary: ModelManagerクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/01 17:23 初回作成
			26/01/15 19:07 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "ModelManager.hpp"

Model* ModelManager::LoadModel(const std::string& modelPath, bool inverseU, bool inverseV)
{
	// すでに読み込まれている場合はそれを返す
	if (auto it = m_modelMap.find(modelPath); it != m_modelMap.end())
	{
		return it->second;
	}

	// モデルを新規作成して読み込む
	Model* pModel = new Model();
	if (FAILED(pModel->Load(modelPath, inverseU, inverseV)))
	{
		assert(0 && modelPath.c_str() && "ModelManager.cpp モデルの読み込みに失敗しました。");
		delete pModel;
		return nullptr;
	}
	
	// マップに登録して返す
	m_modelMap[modelPath] = pModel;
	return pModel;
}

ModelManager::~ModelManager()
{
	for (auto& pair : m_modelMap)
	{
		delete pair.second;
		pair.second = nullptr;
	}
	m_modelMap.clear();
}
