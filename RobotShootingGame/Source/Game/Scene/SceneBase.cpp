/*+===================================================================
	File: SceneBase.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/21 15:33:17 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SceneBase.hpp"

void SceneBase::Update()
{
	// 全レイヤーのアクターを更新
	for (size_t i = 0; i < static_cast<size_t>(Layer::MAX); ++i)
	{
		for (Actor* actor : m_Actors[i])
		{
			actor->Update();
		}
	}
}

void SceneBase::Draw()
{
	// 全レイヤーのアクターを描画
	for (size_t i = 0; i < static_cast<size_t>(Layer::MAX); ++i)
	{
		for (Actor* actor : m_Actors[i])
		{
			actor->Draw();
		}
	}
}

void SceneBase::Uninit()
{
	// 全レイヤーのアクターを解放
	for (size_t i = 0; i < static_cast<size_t>(Layer::MAX); ++i)
	{
		for (Actor* actor : m_Actors[i])
		{
			actor->Uninit();
			delete actor;
		}
		m_Actors[i].clear();
	}
}
