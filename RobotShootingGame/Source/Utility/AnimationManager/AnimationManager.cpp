/*+===================================================================
	File: AnimationManager.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/01 20:16:06 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "AnimationManager.hpp"
#include "Utility/Animation/Animation.hpp"

Animation* AnimationManager::LoadAnimation(const std::string& animePath)
{
	// すでに読み込まれている場合はそれを返す
	if (auto it = m_animationMap.find(animePath); it != m_animationMap.end())
	{
		return it->second;
	}

	// アニメーションを新規作成して読み込む
	Animation* pAnimation = new Animation();
	if (FAILED(pAnimation->Load(animePath)))
	{
		assert(0 && animePath.c_str() && "AnimationManager.cpp アニメーションの読み込みに失敗しました。");
		delete pAnimation;
		return nullptr;
	}

	// マップに登録して返す
	m_animationMap[animePath] = pAnimation;
	return pAnimation;
}

AnimationManager::~AnimationManager()
{
	for (auto& pair : m_animationMap)
	{
		delete pair.second;
		pair.second = nullptr;
	}

	m_animationMap.clear();
}
