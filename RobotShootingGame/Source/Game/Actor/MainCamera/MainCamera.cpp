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
	camera->Init(DirectX::XMVectorSet(0.0f, 10000.0f, 0.0f, 0.0f));
	//camera->SetTargetPos(DirectX::XMLoadFloat3(&targetPos));
	//camera->SetRadius(10.0f);
	//camera->SetTargetPos(DirectX::XMVectorSet(0.0f, 5000.0f, 0.0f, 0.0f));

	// カメラマネージャーに登録
	CameraManager::GetInstance().RegisterCamera("Camera01", camera);
}

void MainCamera::OnUpdate()
{
}

void MainCamera::OnUninit()
{
}
