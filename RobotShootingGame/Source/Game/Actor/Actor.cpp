/*+===================================================================
	File: Actor.cpp
	Summary: アクターの基底クラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/14 11:45 初回作成
			26/01/14 18:53 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Actor.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"

void Actor::Init()
{
	// 派生クラスの初期化処理
	OnInit();
}

void Actor::Update()
{
	// 派生クラスの更新処理
	OnUpdate();

	// コンポーネントの更新処理
	for (auto& component : m_Components)
		component->Update();

	// 子アクターの更新処理
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

void Actor::FixedUpdate()
{
	// 派生クラスの固定更新処理
	OnFixedUpdate();

	// コンポーネントの固定更新処理
	for (auto& component : m_Components)
		component->FixedUpdate();

	// 子アクターの固定更新処理
	for (auto& child : m_ChildActors)
		child->FixedUpdate();
}

void Actor::Draw()
{
	// コンポーネントの描画処理
	for (auto& component : m_Components)
		component->Draw();

	// 子アクターの描画処理
	for (auto& child : m_ChildActors)
		child->Draw();
}

void Actor::Uninit()
{
	for (auto& child : m_ChildActors)
	{
		child->Uninit();
		delete child;
	}

	// 派生クラスの終了処理
	OnUninit();

	for (auto& component : m_Components)
	{
		component->Uninit();
		delete component;
	}
	m_Components.clear();
}
