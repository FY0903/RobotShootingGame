/*+===================================================================
	File: Sprite.cpp
	Summary: スプライトを表すクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/24 08:33 初回作成
			26/01/14 18:49 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Sprite.hpp"
#include "Utility/Compornent/SpriteRenderer/SpriteRenderer.hpp"
#include "Utility/MaterialManager/MaterialManager.hpp"
#include "Utility/TextureManager/TextureManager.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"
#include "Utility/ShadowMapManager/ShadowMapManager.hpp"
#include "Utility/LightManager/LightManager.hpp"

void Sprite::OnInit()
{
	// テクスチャの読み込み
	auto pDefTex = TextureManager::GetInstance().CreateDefaultTexture("DefTex");
	//auto tex = TextureManager::GetInstance().LoadTexture("Assets/Texture/TitleLogo.png");
	auto pShadowMaps = ShadowMapManager::GetInstance().GetShadowMaps();

	// マテリアルの設定
	m_pMaterial = MaterialManager::GetInstance().CreateMaterial("Ground");
	m_pMaterial->SetTexture(pDefTex);
	if (!pShadowMaps.empty())
	{
		m_pMaterial->SetTexture(pShadowMaps[0]->GetRenderTarget());
	}
	m_pMaterial->SetIsOpaque(false); // ライティングされたくないので不透明設定をfalseにする
	
	// スプライトレンダラーコンポーネントの追加
	AddComponent<SpriteRenderer>()->Init();

	// 定数バッファの設定
	//m_pTimeCB = m_pMaterial->SetCBAtRegister(1, sizeof(CB::Time)); // レジスタ番号1に時間用定数バッファを設定
	
	m_pLVPC = m_pMaterial->SetCBAtRegister(1, sizeof(CB::LVP)); // レジスタ番号2にライトビュー行列用定数バッファを設定
}

void Sprite::OnUpdate()
{
	// 定数バッファに時間情報を設定
	//size_t currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();

	//CB::Time* pTime = m_pTimeCB[currentIndex]->GetPtr<CB::Time>();
	//pTime->DeltaTime = Time::GetInstance().GetDeltaTimeSinceStartup();

	size_t currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();
	CB::LVP* pLVPC = m_pLVPC[currentIndex]->GetPtr<CB::LVP>();
	pLVPC->VMat = LightManager::GetInstance().GetLights()[0]->GetViewMatrixFloat4x4(false);
	pLVPC->PMat = LightManager::GetInstance().GetLights()[0]->GetProjectionMatrixFloat4x4(false);
}

void Sprite::OnUninit()
{
}
