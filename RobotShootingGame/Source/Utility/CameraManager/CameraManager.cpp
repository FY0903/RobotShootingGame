/*+===================================================================
	File: CameraManager.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/23 16:39:11 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "CameraManager.hpp"

void CameraManager::RegisterCamera(const std::string& name, Camera* pCamera)
{
	m_pCameraData[name] = pCamera;

	SetMainCamera(name);
}

void CameraManager::SetMainCamera(const std::string& name)
{
	if (m_pCameraData.find(name) != m_pCameraData.end())
	{
		m_pMainCamera = m_pCameraData[name];
	}
}
