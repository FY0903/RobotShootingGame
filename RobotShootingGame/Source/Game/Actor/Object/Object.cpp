/*+===================================================================
	File: Object.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/24 13:58:08 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Object.hpp"
#include "Utility/Compornent/MeshRenderer/MeshRenderer.hpp"
#include "Utility/Compornent/SkeletalAnimator/SkeletalAnimator.hpp"
#include "Utility/Compornent/AnimatorController/AnimatorController.hpp"

void Object::OnInit()
{
	m_pModel = new Model();
	//if (FAILED(m_pModel->Load("Assets/Model/test/FBX_Demo_Idle.fbx", false, false)))
	if (FAILED(m_pModel->Load("Assets/Model/character/Hew_kyaracter(1.0).fbx", false, false)))
	{
		assert(0 && "Object.cpp モデルの読み込みに失敗しました。");
	}

	m_pAnimations.resize(2);
	m_pAnimations[0] = new Animation();
	if (FAILED(m_pAnimations[0]->Load("Assets/Model/character/Animation/taiki_mae.fbx")))
	{
		assert(0 && "Object.cpp アニメーションの読み込みに失敗しました。");
	}

	m_pAnimations[1] = new Animation();
	if (FAILED(m_pAnimations[1]->Load("Assets/Model/character/Animation/walk.fbx")))
	{
		assert(0 && "Object.cpp アニメーションの読み込みに失敗しました。");
	}

	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->Init(m_pModel);

	auto skeletalAnimator = AddComponent<SkeletalAnimator>();
	skeletalAnimator->Init(m_pModel);

	auto animatorController = AddComponent<AnimatorController>();
	animatorController->AddAnimation("Idle", m_pAnimations[0]);
	animatorController->AddAnimation("Walk", m_pAnimations[1]);
	animatorController->Init();
}

void Object::OnUpdate()
{
}

void Object::OnUninit()
{
	if (m_pModel)
	{
		delete m_pModel;
		m_pModel = nullptr;
	}

	for (auto& animation : m_pAnimations)
	{
		if (animation)
		{
			delete animation;
			animation = nullptr;
		}
	}
}
