/*+===================================================================
	File: MainCamera.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/21 18:12:36 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "MainCamera.hpp"
#include "Utility/Compornent/Camera/Camera.hpp"
void MainCamera::Init()
{
	// ルートシグネチャの生成
	m_pRootSignature->Create();

	// カメラコンポーネントの生成と初期化
	AddComponent<Camera>()->Init();
}

void MainCamera::Uninit()
{
}
