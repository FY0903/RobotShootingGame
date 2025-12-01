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
#include "Utility/Input/Input.hpp"
#include "Utility/ModelManager/ModelManager.hpp"

void Object::OnInit()
{
	auto pModel = ModelManager::GetInstance().LoadModel("Assets/Model/character/Hew_kyaracter(1.0).fbx");

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

	// マテリアルの設定
	m_pMaterial = MaterialManager::GetInstance().CreateMaterial("SkeletalMesh");

	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->Init(pModel);

	auto skeletalAnimator = AddComponent<SkeletalAnimator>();
	skeletalAnimator->Init(pModel);
	skeletalAnimator->PlayAnimation(m_pAnimations[0]);
}

void Object::OnUpdate()
{
	if (Input::IsKeyTrigger('W'))
	{
		auto skeletalAnimator = GetComponent<SkeletalAnimator>();
		skeletalAnimator->BlendAnimation(m_pAnimations[1], 2.0f);
	}

	if (Input::IsKeyTrigger('S'))
	{
		auto skeletalAnimator = GetComponent<SkeletalAnimator>();
		skeletalAnimator->BlendAnimation(m_pAnimations[0], 2.0f);
	}
}

void Object::OnUninit()
{
	for (auto& animation : m_pAnimations)
	{
		if (animation)
		{
			delete animation;
			animation = nullptr;
		}
	}
}
