/*+===================================================================
	File: AnimatorController.cpp
	Summary: AnimatorControllerクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/27 11:51 初回作成
			26/01/15 16:59 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "AnimatorController.hpp"

void AnimatorController::Uninit()
{
	for (auto& animation : m_Animations)
	{
		animation.second = nullptr;
	}
}

void AnimatorController::AddAnimation(const std::string& name, Animation* animation)
{
	// すでに同じ名前のアニメーションが存在する場合は追加しない
	if (m_Animations.find(name) != m_Animations.end()) return;

	m_Animations[name] = animation;
}
