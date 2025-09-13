/*+===================================================================
	File: Scene.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/02 14:54:10 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Scene.hpp"
#include "../../System/Window/Window.hpp"
#include "../../Utility/Texture/Texture.hpp"
#include "../../Utility/ModelLoader/ModelLoader.hpp"
#include "../../Utility/Sound/Sound.hpp"
#include <iterator>
#include <memory>

void Scene::Init()
{
	DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(0.0f, 5.0f, 5.0f, 0.0f);
	DirectX::XMVECTOR targetPos = DirectX::XMVectorZero();
	DirectX::XMVECTOR upVec = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	float fov = DirectX::XMConvertToRadians(45.0f);
	float aspect = static_cast<float>(Window::GetInstance().GetWidth()) / static_cast<float>(Window::GetInstance().GetHeight());

	// Cameraの生成
	m_pCamera = new Camera(eyePos, targetPos, upVec, 10.0f, fov, aspect);

	// Objectの生成
	m_pObject = new Object(*m_pCamera);

	// Spriteの生成
	std::shared_ptr<Texture> texture = std::make_shared<Texture>();
	texture->Load("Assets/Texture/ADC_COW.png");
	m_pSprite = new Sprite(texture, *m_pCamera);

	// Modelの生成
	ModelData model = ModelLoader::GetInstance().Load("Assets/Model/DORO/Doro.fbx", false, false);
	//ModelLoader::GetInstance().LoadAnimation("Assets/Model/character/taiki_mae.fbx", model, "Idle");
	//ModelLoader::GetInstance().LoadAnimation("Assets/Model/character/walk.fbx", model, "Walk");
	m_pModel = new Model(model, *m_pCamera);

	// Lineの生成
	m_pLine = new Line(*m_pCamera);
	m_pLine->AddPoint({ -10.0f, 0.0f, 0.0f }, { 10.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }); // X軸
	m_pLine->AddPoint({ 0.0f, -10.0f, 0.0f }, { 0.0f, 10.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }); // Y軸
	m_pLine->AddPoint({ 0.0f, 0.0f, -10.0f }, { 0.0f, 0.0f, 10.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }); // Z軸
	m_pLine->Create();

	// BGMの読み込みと再生
	Sound::GetInstance().Load("BGM", "Assets/Sound/BGM/TestBGM.mp3", false, true);
	Sound::GetInstance().Play("BGM");
	Sound::GetInstance().SetBGMVolume(0.1f);
}

void Scene::Update()
{
	// Cameraの更新
	m_pCamera->Update();

	// Lineの更新
	m_pLine->Update();

	// Objectの更新
	//m_pObject->Update();

	// Spriteの更新
	//m_pSprite->Update();

	// Modelの更新
	m_pModel->Update();
	//m_pModel->Update("Idle", 0, "Walk", 0, 0.0f);
}

void Scene::Draw()
{
	// Lineの描画
	m_pLine->Draw();

	// Objectの描画
	//m_pObject->Draw();

	// Spriteの描画
	//m_pSprite->Draw();

	// Modelの描画
	m_pModel->Draw();
}

void Scene::UnInit()
{
	delete m_pLine;
	m_pLine = nullptr;

	delete m_pModel;
	m_pModel = nullptr;

	delete m_pSprite;
	m_pSprite = nullptr;

	delete m_pObject;
	m_pObject = nullptr;

	delete m_pCamera;
	m_pCamera = nullptr;
}
