/*+===================================================================
	File: SceneBase.cpp
	Summary: シーンの基底クラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/21 15:33 初回作成
			26/01/14 19:14 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SceneBase.hpp"

void SceneBase::Init()
{
	// シーン固有の初期化処理
	OnInit();
}

void SceneBase::Update()
{
	// シーン固有の更新処理
	OnUpdate();

	// 全レイヤーのアクターを更新
	for (size_t i = 0; i < static_cast<size_t>(Layer::MAX); ++i)
	{
		for (Actor* actor : m_Actors[i])
		{
			actor->Update();
		}
	}
}

void SceneBase::FixedUpdate()
{
	// シーン固有の固定更新処理
	OnFixedUpdate();
	
	// 全レイヤーのアクターを固定更新
	for (size_t i = 0; i < static_cast<size_t>(Layer::MAX); ++i)
	{
		for (Actor* actor : m_Actors[i])
		{
			actor->FixedUpdate();
		}
	}
}

void SceneBase::Draw()
{
	// シーン固有の描画処理
	OnDraw();

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
	// シーン固有の終了処理
	OnUninit();

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
