/*+===================================================================
	File: SceneGame.cpp
	Summary: ゲームシーンを表すクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/21 17:30 初回作成
			26/01/14 19:13 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SceneGame.hpp"
#include "Game/Actor/MainCamera/MainCamera.hpp"
#include "Game/Actor/Grid/Grid.hpp"
#include "Game/Actor/Sprite/Sprite.hpp"
#include "Game/Actor/Object/Object.hpp"
#include "Game/Actor/Empty/Empty.hpp"
#include "Utility/Sound/Sound.hpp"
#include "Game/Actor/DirectionalLight/DirectionalLight.hpp"
#include "Game/Actor/Cloud/Cloud.hpp"
#include "Game/Actor/Sky/Sky.hpp"

void SceneGame::OnInit()
{
	// メインカメラの生成
	AddGameObject<MainCamera>(Layer::DEFAULT);

	// ディレクショナルライトの生成
	AddGameObject<DirectionalLight>(Layer::DEFAULT);

	// グリッドの生成
	//AddGameObject<Grid>(Layer::DEFAULT);

	// 雲の生成
	auto cloud = AddGameObject<Cloud>(Layer::DEFAULT);
	cloud->GetTransform().Position = DirectX::XMFLOAT3(0.0f, 100.0f, 0.0f);
	cloud->GetTransform().Scale = DirectX::XMFLOAT3(1000.0f, 1000.0f, 1.0f);
	cloud->GetTransform().Rotation = Transform::ToQuaternion(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(90.0f), 0.0f, 0.0f));

	// 空の生成
	auto sky = AddGameObject<Sky>(Layer::DEFAULT);
	sky->GetTransform().Scale = DirectX::XMFLOAT3(500.0f, 500.0f, 500.0f);

	// スプライトの生成
	//auto sprite0 = AddGameObject<Sprite>(Layer::DEFAULT);
	//auto& transform = sprite0->GetTransform();
	//transform.Rotation = Transform::ToQuaternion(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(90.0f), 0.0f, 0.0f));
	//transform.Scale = DirectX::XMFLOAT3(10.0f, 10.0f, 1.0f);

	// オブジェクトの生成
	//AddGameObject<Object>(Layer::DEFAULT);

	// エンプティの生成
	//auto emp = AddGameObject<Empty>(Layer::DEFAULT);
	//emp->GetTransform().Position = DirectX::XMFLOAT3(1.0f, 0.0f, 1.0f);
	//AddGameObject<Empty>(Layer::DEFAULT);

	Sound::GetInstance().Load("BGM", "Assets/Sound/BGM/Title.mp3", false, true);
	//Sound::GetInstance().Play("BGM");
}

void SceneGame::OnUpdate()
{
	//auto obj = GetGameObject<Empty>();
	//Transform& transform = obj->GetTransform();
	//transform.Rotation *= Transform::ToQuaternion(DirectX::SimpleMath::Vector3(0.0f, 0.01f, 0.0f));
}

void SceneGame::OnFixedUpdate()
{
}

void SceneGame::OnDraw()
{
}

void SceneGame::OnUninit()
{
}
