/*+===================================================================
	File: Actor.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/14 11:45:12 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Actor.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"

Actor::Actor()
{
}

Actor::~Actor()
{
	for (auto& component : m_Components)
	{
		component->Uninit();
		delete component;
	}
	m_Components.clear();
}

void Actor::Init()
{
	OnInit();
}

void Actor::Update()
{
	OnUpdate();

	for (auto& component : m_Components)
	{
		component->Update();
	}
}

void Actor::Draw()
{
	for (auto& component : m_Components)
	{
		component->Draw();
	}
}

void Actor::Uninit()
{
	OnUninit();
	for (auto& component : m_Components)
	{
		component->Uninit();
		delete component;
	}
	m_Components.clear();
}
