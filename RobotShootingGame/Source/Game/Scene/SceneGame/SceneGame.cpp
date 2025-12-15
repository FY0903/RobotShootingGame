/*+===================================================================
	File: SceneGame.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/21 17:30:12 初回作成
	（これ以降下に更新日時と更新内容を書く）
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

void SceneGame::OnInit()
{
	// メインカメラの生成
	AddGameObject<MainCamera>(Layer::DEFAULT);

	// グリッドの生成
	AddGameObject<Grid>(Layer::DEFAULT);

	// スプライトの生成
	Sprite* sprite[2];
	for (int i = 0; i < _countof(sprite); ++i)
		sprite[i] = AddGameObject<Sprite>(Layer::DEFAULT);

	sprite[0]->GetTransform().Position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -5.0f);
	sprite[0]->GetTransform().Scale = DirectX::SimpleMath::Vector3(5.0f, 5.0f, 1.0f);
	sprite[1]->GetTransform().Position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 5.0f);
	sprite[1]->GetTransform().Scale = DirectX::SimpleMath::Vector3(5.0f, 5.0f, 1.0f);

	// オブジェクトの生成
	//AddGameObject<Object>(Layer::DEFAULT);

	// エンプティの生成
	AddGameObject<Empty>(Layer::DEFAULT);
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
