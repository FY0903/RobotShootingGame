/*+===================================================================
	File: Compornent.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/14 14:34:53 初回作成
	（これ以降下に更新日時と更新内容を書く）
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
