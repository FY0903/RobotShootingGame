/*+===================================================================
	File: Object.cpp
	Summary: オブジェクトクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/24 13:58 初回作成
			26/01/14 18:36 コメント記載
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
	// モデルの読み込み
	//auto pModel = ModelManager::GetInstance().LoadModel("Assets/Model/F15E/F15E.fbx");
	auto pModel = ModelManager::GetInstance().LoadModel("Assets/Model/Kazusa/Kazusa.fbx", false);
	//auto pModel = ModelManager::GetInstance().LoadModel("Assets/Model/character/Hew_kyaracter(1.0).fbx");

	// アニメーションの読み込み
	m_pAnimations.resize(2);
	m_pAnimations[0] = AnimationManager::GetInstance().LoadAnimation("Assets/Model/character/Animation/taiki_mae.fbx");
	m_pAnimations[1] = AnimationManager::GetInstance().LoadAnimation("Assets/Model/character/Animation/walk.fbx");

	// マテリアルの設定
	m_pMaterial = MaterialManager::GetInstance().CreateMaterial("SkeletalGBuffer");

	// メッシュレンダラーコンポーネントを追加
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->Init(pModel);
	//meshRenderer->Init();

	// スケルタルアニメーターコンポーネントを追加
	//auto skeletalAnimator = AddComponent<SkeletalAnimator>();
	//skeletalAnimator->Init(pModel);
	//skeletalAnimator->PlayAnimation(m_pAnimations[0]);

	m_Transform.Scale = DirectX::XMFLOAT3(0.2f, 0.2f, 0.2f);
	//m_Transform.Rotation = Transform::ToQuaternion(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(90.0f), 0.0f, 0.0f));
}

void Object::OnUpdate()
{
	//if (Input::IsKeyTrigger('W'))
	//{
	//	auto skeletalAnimator = GetComponent<SkeletalAnimator>();
	//	skeletalAnimator->BlendAnimation(m_pAnimations[1], 2.0f);
	//}

	//if (Input::IsKeyTrigger('S'))
	//{
	//	auto skeletalAnimator = GetComponent<SkeletalAnimator>();
	//	skeletalAnimator->BlendAnimation(m_pAnimations[0], 2.0f);
	//}
}

void Object::OnUninit()
{
}
