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
}

void Cloud::OnUpdate()
{
}

void Cloud::OnUninit()
{
}
