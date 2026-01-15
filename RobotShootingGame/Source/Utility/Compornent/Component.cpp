/*+===================================================================
	File: Compornent.cpp
	Summary: Compornentクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/14 14:34 初回作成
			26/01/15 18:10 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Component.hpp"

Component::Component(Actor* owner)
{
	m_Owner = owner;
}

Component::~Component()
{
	m_Owner = nullptr;
}
