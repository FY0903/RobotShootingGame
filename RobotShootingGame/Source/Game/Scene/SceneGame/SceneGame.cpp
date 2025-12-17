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
#include "Game/Actor/Title/Title.hpp"
#include "Game/Actor/Skybox/Skybox.hpp"
#include "Game/Actor/TitleText/TitleText.hpp"
#include "Utility/Sound/Sound.hpp"

void SceneGame::OnInit()
{
	// メインカメラの生成
	AddGameObject<MainCamera>(Layer::DEFAULT);

	// グリッドの生成
	//AddGameObject<Grid>(Layer::DEFAULT);

	// スカイボックスの生成
	AddGameObject<Skybox>(Layer::DEFAULT);

	// スプライトの生成
	auto sprite = AddGameObject<Sprite>(Layer::DEFAULT);
	auto& transform = sprite->GetTransform();
	transform.Rotation = Transform::ToQuaternion(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(90.0f), 0.0f, 0.0f));
	transform.Scale = DirectX::XMFLOAT3(500.0f, 500.0f, 1.0f);

	AddGameObject<Title>(Layer::DEFAULT);
	AddGameObject<TitleText>(Layer::DEFAULT);
	// オブジェクトの生成
	//AddGameObject<Object>(Layer::DEFAULT);

	// エンプティの生成
	//AddGameObject<Empty>(Layer::DEFAULT);

	Sound::GetInstance().Load("BGM", "Assets/Sound/BGM/Title.mp3", false, true);
	Sound::GetInstance().Play("BGM");
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
