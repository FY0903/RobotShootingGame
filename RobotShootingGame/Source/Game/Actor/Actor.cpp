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

	for (auto& child : m_ChildActors)
	{
		Transform worldTransform = child->m_Transform;

		// 親の変換を適用
		child->m_Transform.Scale = m_Transform.Scale * child->m_Transform.Scale;
		child->m_Transform.Rotation = m_Transform.Rotation * child->m_Transform.Rotation;
		child->m_Transform.Position = m_Transform.Position + child->m_Transform.Position;

		child->Update();

		// 子の変換を元に戻す
		child->m_Transform = worldTransform;
	}
}

void Actor::Draw()
{
	for (auto& component : m_Components)
	{
		component->Draw();
	}

	for (auto& child : m_ChildActors)
	{
		child->Draw();
	}
}

void Actor::Uninit()
{
	for (auto& child : m_ChildActors)
	{
		child->Uninit();
		delete child;
	}

	OnUninit();
	for (auto& component : m_Components)
	{
		component->Uninit();
		delete component;
	}
	m_Components.clear();
}
