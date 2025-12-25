/*+===================================================================
	File: Empty.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/30 0:39:09 初回作成
	（これ以降下に更新日時と更新内容を書く）
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
	const float angularVelocity = Transform::ToRadian(45.0f); // 角速度（度/秒）
	const float deltaTime = Time::GetInstance().GetDeltaTime();	// フレーム間の時間差

	const float deltaAngle = angularVelocity * deltaTime;

	//m_Transform.Rotation *= Transform::ToQuaternion(DirectX::SimpleMath::Vector3(0.0f, deltaAngle, 0.0f));
}

void Empty::OnUninit()
{
}
