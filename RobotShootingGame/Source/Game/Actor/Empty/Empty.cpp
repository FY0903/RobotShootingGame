/*+===================================================================
	File: Empty.cpp
	Summary: 空のアクターを表すクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/30 00:39 初回作成
			26/01/14 17:57 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Empty.hpp"
#include "Game/Actor/Object/Object.hpp"

void Empty::OnInit()
{
	// 子オブジェクトとしてObjectを追加
	AddChildActor<Object>();
}

void Empty::OnUpdate()
{
}

void Empty::OnFixedUpdate()
{
	// 角速度とデルタタイムを取得
	const float angularVelocity = Transform::ToRadian(45.0f); // 角速度（度/秒）
	const float deltaTime = Time::GetInstance().GetDeltaTime();	// フレーム間の時間差

	// 回転角度を計算
	const float deltaAngle = angularVelocity * deltaTime;

	// 回転を適用
	//m_Transform.Rotation *= Transform::ToQuaternion(DirectX::SimpleMath::Vector3(0.0f, deltaAngle, 0.0f));
}

void Empty::OnUninit()
{
}
