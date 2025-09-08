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
	m_pCamera = new Camera(eyePos, targetPos, upVec, fov, aspect);

	// Objectの生成
	m_pObject = new Object(*m_pCamera);

	// Spriteの生成
	std::shared_ptr<Texture> texture = std::make_shared<Texture>();
	texture->Load("Assets/Texture/ADC_COW.png");
	m_pSprite = new Sprite(texture, *m_pCamera);

	// Modelの生成
	std::vector<Mesh> meshes = ModelLoader::GetInstance().Load("Assets/Model/character/Hew_kyaracter(1.0).fbx", false, true);
	m_pModel = new Model(meshes, *m_pCamera);
}

void Scene::Update()
{
	// Cameraの更新
	m_pCamera->Update();

	// Objectの更新
	//m_pObject->Update();

	// Spriteの更新
	//m_pSprite->Update();

	// Modelの更新
	m_pModel->Update();
}

void Scene::Draw()
{
	// Objectの描画
	//m_pObject->Draw();

	// Spriteの描画
	//m_pSprite->Draw();

	// Modelの描画
	m_pModel->Draw();
}

void Scene::UnInit()
{
	delete m_pModel;
	m_pModel = nullptr;

	delete m_pSprite;
	m_pSprite = nullptr;

	delete m_pObject;
	m_pObject = nullptr;

	delete m_pCamera;
	m_pCamera = nullptr;
}
