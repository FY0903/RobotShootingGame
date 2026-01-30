/*+===================================================================
	File: ShadowMapManager.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2026/01/20 12:56:02 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "ShadowMapManager.hpp"

void ShadowMapManager::CreateShadowMap(Light* pLight, ShadowMap::Quality quality)
{
	for (auto& pShadowMap : m_pShadowMaps)
	{
		// すでに同じライトコンポーネントのシャドウマップが存在する場合は作成しない
		if (pShadowMap->GetLight() == pLight) return;
	}

	// シャドウマップの作成
	ShadowMap* pShadowMap = new ShadowMap(pLight, quality);
	m_pShadowMaps.push_back(pShadowMap);
}

void ShadowMapManager::DrawAllShadowMaps(std::vector<Render::RenderItem> renderItem)
{
	for (auto& pShadowMap : m_pShadowMaps)
	{
		// レンダーアイテムの設定
		pShadowMap->SetRenderItems(renderItem);
	
		// ライトのビュー・プロジェクション行列の更新
		pShadowMap->UpdateLightVP();
	
		// シャドウマップの描画
		pShadowMap->Draw();
	}
}

void ShadowMapManager::DestroyShadowMap(Light* pLight)
{
	// 指定されたライトコンポーネントに関連付けられたシャドウマップを破棄
	for (auto it = m_pShadowMaps.begin(); it != m_pShadowMaps.end(); )
	{
		// ライトコンポーネントが一致する場合は削除
		if ((*it)->GetLight() == pLight)
		{
			delete* it;
			it = m_pShadowMaps.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void ShadowMapManager::DestroyAllShadowMaps()
{
	for (auto& pShadowMap : m_pShadowMaps)
	{
		delete pShadowMap;
		pShadowMap = nullptr;
	}
	m_pShadowMaps.clear();
}

ShadowMapManager::~ShadowMapManager()
{
	for (auto& pShadowMap : m_pShadowMaps)
	{
		delete pShadowMap;
		pShadowMap = nullptr;
	}
	m_pShadowMaps.clear();
}
