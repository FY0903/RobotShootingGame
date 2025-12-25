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
#include "Utility/CameraManager/CameraManager.hpp"

void MainCamera::OnInit()
{
	// カメラコンポーネントの生成と初期化
	Camera* camera = AddComponent<Camera>();

	camera->Init();
	//camera->SetTargetPos(DirectX::XMLoadFloat3(&targetPos));
	//camera->SetRadius(10.0f);

	// カメラマネージャーに登録
	CameraManager::GetInstance().RegisterCamera("Camera01", camera);
}

void MainCamera::OnUpdate()
{
}

void MainCamera::OnUninit()
{
}
