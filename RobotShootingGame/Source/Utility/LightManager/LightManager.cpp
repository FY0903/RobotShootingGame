/*+===================================================================
	File: LightManager.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/19 12:49:59 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "LightManager.hpp"

void LightManager::RegisterLight(Light* pLight)
{
	m_Lights.push_back(pLight);
}

const std::vector<class Light*>& LightManager::GetLights() const
{
	return m_Lights;
}
