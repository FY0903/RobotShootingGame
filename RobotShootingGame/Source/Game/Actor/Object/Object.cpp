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
#include "Utility/AnimationManager/AnimationManager.hpp"

void Object::OnInit()
{
	auto pModel = ModelManager::GetInstance().LoadModel("Assets/Model/character/Hew_kyaracter(1.0).fbx");

	m_pAnimations.resize(2);
	m_pAnimations[0] = AnimationManager::GetInstance().LoadAnimation("Assets/Model/character/Animation/taiki_mae.fbx");
	m_pAnimations[1] = AnimationManager::GetInstance().LoadAnimation("Assets/Model/character/Animation/walk.fbx");

	// マテリアルの設定
	//m_pMaterial = MaterialManager::GetInstance().CreateMaterial("SkeletalGBuffer");
	m_pMaterial = MaterialManager::GetInstance().CreateMaterial("DebugGBuffer");

	auto meshRenderer = AddComponent<MeshRenderer>();
	//meshRenderer->Init(pModel);
	meshRenderer->Init();

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
}
