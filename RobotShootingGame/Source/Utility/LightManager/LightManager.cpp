/*+===================================================================
	File: LightManager.cpp
	Summary: LightManagerクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/19 12:49 初回作成
			26/01/15 17:51 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "LightManager.hpp"

void LightManager::RegisterLight(Light* pLight)
{
	m_Lights.push_back(pLight);	// ライトコンポーネントを登録
}

const std::vector<class Light*>& LightManager::GetLights() const
{
	return m_Lights;
}
