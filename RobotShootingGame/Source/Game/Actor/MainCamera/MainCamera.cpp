/*+===================================================================
	File: MainCamera.cpp
	Summary: メインカメラクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/21 18:12 初回作成
			26/01/14 18:34 コメント記載
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
	camera->SetRadius(30.0f);
	//camera->SetTargetPos(DirectX::XMVectorSet(0.0f, 2.0f, 0.0f, 0.0f));

	// カメラマネージャーに登録
	CameraManager::GetInstance().RegisterCamera("Camera01", camera);
}

void MainCamera::OnUpdate()
{
}

void MainCamera::OnUninit()
{
}
