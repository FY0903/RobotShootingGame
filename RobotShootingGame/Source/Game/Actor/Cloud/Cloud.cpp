/*+===================================================================
	File: Cloud.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2026/02/03 20:45:41 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Cloud.hpp"
#include "Utility/MaterialManager/MaterialManager.hpp"
#include "Utility/TextureManager/TextureManager.hpp"
#include "Utility/ShadowMapManager/ShadowMapManager.hpp"
#include "Utility/Compornent/SpriteRenderer/SpriteRenderer.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"
#include "Utility/LightManager/LightManager.hpp"

void Cloud::OnInit()
{
	// テクスチャの読み込み
	auto pDefTex = TextureManager::GetInstance().CreateDefaultTexture("DefTex");
	auto pShadowMaps = ShadowMapManager::GetInstance().GetShadowMaps();

	// マテリアルの設定
	m_pMaterial = MaterialManager::GetInstance().CreateMaterial("Cloud");
	m_pMaterial->SetTexture(pDefTex);
	if (!pShadowMaps.empty())
	{
		m_pMaterial->SetTexture(pShadowMaps[0]->GetRenderTarget());
	}
	m_pMaterial->SetIsOpaque(false); // ライティングされたくないので不透明設定をfalseにする

	// スプライトレンダラーコンポーネントの追加
	AddComponent<SpriteRenderer>()->Init();

	m_pLightCB = m_pMaterial->SetCBAtRegister(1, sizeof(CB::Light)); // レジスタ番号1にライト用定数バッファを設定
	m_pTimeCB = m_pMaterial->SetCBAtRegister(2, sizeof(CB::Time)); // レジスタ番号2に時間用定数バッファを設定
}

void Cloud::OnUpdate()
{
	size_t currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();
	CB::Light* pLightCB = m_pLightCB[currentIndex]->GetPtr<CB::Light>();
	CB::Time* pTimeCB = m_pTimeCB[currentIndex]->GetPtr<CB::Time>();

	auto lights = LightManager::GetInstance().GetLights();

	// ライト情報の設定
	pLightCB->Position = lights[0]->GetPosition();
	pLightCB->Color = lights[0]->GetColor();
	pLightCB->Direction = lights[0]->GetDirection();
	pLightCB->Range = lights[0]->GetRange();
	pLightCB->Angle = lights[0]->GetAngle();

	// 時間情報の設定
	pTimeCB->DeltaTime = Time::GetInstance().GetDeltaTimeSinceStartup();
}

void Cloud::OnUninit()
{
}
