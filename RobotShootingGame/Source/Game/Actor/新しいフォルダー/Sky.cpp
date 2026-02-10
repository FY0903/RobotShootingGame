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

void Sky::OnInit()
{
	auto pModel = ModelManager::GetInstance().LoadModel("Assets/Model/skybox/skybox.fbx", false);

	// マテリアルの設定
	m_pMaterial = MaterialManager::GetInstance().CreateMaterial("AtmosphericScattering");

	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->Init(pModel);

	m_pMaterial->SetIsOpaque(false); // ライティングされたくないので不透明設定をfalseにする
}

void Sky::OnUpdate()
{
}

void Sky::OnUninit()
{
}
