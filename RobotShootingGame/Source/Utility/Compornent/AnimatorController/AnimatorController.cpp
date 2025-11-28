/*+===================================================================
	File: AnimatorController.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/27 11:51:37 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "AnimatorController.hpp"

void AnimatorController::Init()
{
	m_StateMachine.Start(this);
	m_StateMachine.ChangeState<AnimatorState::Idle>();
}

void AnimatorController::Update()
{
	m_StateMachine.Update();
}

void AnimatorController::Draw()
{
}

void AnimatorController::Uninit()
{
	for (auto& animation : m_Animations)
	{
		animation.second = nullptr;
	}
}

void AnimatorController::AddAnimation(const std::string& name, Animation* animation)
{
	if (m_Animations.find(name) != m_Animations.end()) return;

	m_Animations[name] = animation;
}
