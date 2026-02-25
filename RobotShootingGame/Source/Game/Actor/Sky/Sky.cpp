/*+===================================================================
	File: Sky.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2026/02/10 20:21:53 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Sky.hpp"
#include "Utility/MaterialManager/MaterialManager.hpp"
#include "Utility/Compornent/MeshRenderer/MeshRenderer.hpp"
#include "Utility/Compornent/SpriteRenderer/SpriteRenderer.hpp"
#include "Utility/ModelManager/ModelManager.hpp"
#include "Utility/LightManager/LightManager.hpp"
#include "Utility/Compornent/Light/Light.hpp"
#include "Utility/CameraManager/CameraManager.hpp"
#include "Utility/TextureManager/TextureManager.hpp"

void Sky::OnInit()
{
	auto pModel = ModelManager::GetInstance().LoadModel("Assets/Model/skybox/skybox.fbx", false);

	// マテリアルの設定
	m_pMaterial = MaterialManager::GetInstance().CreateMaterial("AtmosphericScattering");
	
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->Init(pModel);
	m_pSkyCB = m_pMaterial->SetCBAtRegister(1, sizeof(CB::Sky));

	m_Transform.Scale = DirectX::SimpleMath::Vector3(800.0f, 800.0f, 800.0f);
}

void Sky::OnUpdate()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();
	auto lights = LightManager::GetInstance().GetLights();

	CB::Sky* pSky = m_pSkyCB[currentIndex]->GetPtr<CB::Sky>();
	pSky->cameraPos = CameraManager::GetInstance().GetMainCameraPosition();
	pSky->lightDir = lights[0]->GetDirection();
}

void Sky::OnUninit()
{
}
