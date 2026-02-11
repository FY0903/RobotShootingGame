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
#include "Utility/ModelManager/ModelManager.hpp"
#include "Utility/LightManager/LightManager.hpp"
#include "Utility/Compornent/Light/Light.hpp"
#include "Utility/CameraManager/CameraManager.hpp"

void Sky::OnInit()
{
	auto pModel = ModelManager::GetInstance().LoadModel("Assets/Model/skybox/skybox.fbx", false);

	// マテリアルの設定
	m_pMaterial = MaterialManager::GetInstance().CreateMaterial("AtmosphericScattering");

	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->Init(pModel);

	m_pSkyCB = m_pMaterial->SetCBAtRegister(1, sizeof(CB::Sky));		// レジスタ番号1にスカイ用定数バッファを設定
	m_pCameraCB = m_pMaterial->SetCBAtRegister(2, sizeof(CB::Camera));	// レジスタ番号2にカメラ用定数バッファを設定
	m_pLightCB = m_pMaterial->SetCBAtRegister(3, sizeof(CB::Light));	// レジスタ番号3にライト用定数バッファを設定
	m_pMaterial->SetIsOpaque(false); // ライティングされたくないので不透明設定をfalseにする

	// スカイ用定数バッファの初期化
	for (int i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		CB::Sky* pSkyPtr = m_pSkyCB[i]->GetPtr<CB::Sky>();
		pSkyPtr->InnerRadius = 10000.0f;
		pSkyPtr->OuterRadius = 10250.0f;
		pSkyPtr->Kr = 0.0025f;
		pSkyPtr->Km = 0.001f;
	}

	m_Transform.Scale = DirectX::XMFLOAT3(20500.0f, 20500.0f, 20500.0f);
}

void Sky::OnUpdate()
{
	size_t currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();
	CB::Camera* pCameraCB = m_pCameraCB[currentIndex]->GetPtr<CB::Camera>();
	CB::Light* pLightCB = m_pLightCB[currentIndex]->GetPtr<CB::Light>();

	// カメラ情報の設定
	pCameraCB->Pos = CameraManager::GetInstance().GetMainCameraPosition();

	auto lights = LightManager::GetInstance().GetLights();

	// ライト情報の設定
	pLightCB->Position = lights[0]->GetPosition();
	pLightCB->Color = lights[0]->GetColor();
	pLightCB->Direction = lights[0]->GetDirection();
	pLightCB->Range = lights[0]->GetRange();
	pLightCB->Angle = lights[0]->GetAngle();
}

void Sky::OnUninit()
{
}
