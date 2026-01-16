/*+===================================================================
	File: DirectionalLight.cpp
	Summary: 太陽光源を表すクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/18 15:43 初回作成
			26/01/14 17:54 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "DirectionalLight.hpp"
#include "Utility/Compornent/Light/Light.hpp"
#include "Utility/Input/Input.hpp"

void DirectionalLight::OnInit()
{
	// ライトコンポーネントを追加
	auto light = AddComponent<Light>();
	light->Init(Light::DIRECTIONAL);	// 太陽光源として初期化
}

void DirectionalLight::OnUpdate()
{
#ifdef _DEBUG
	if (Input::IsKeyPress('I'))
	{
		m_Transform.Rotation *= Transform::ToQuaternion(DirectX::SimpleMath::Vector3(-0.01f, 0.0f, 0.0f));
	}

	if (Input::IsKeyPress('K'))
	{
		m_Transform.Rotation *= Transform::ToQuaternion(DirectX::SimpleMath::Vector3(0.01f, 0.0f, 0.0f));
	}

	if (Input::IsKeyPress('J'))
	{
		m_Transform.Rotation *= Transform::ToQuaternion(DirectX::SimpleMath::Vector3(0.0f, -0.01f, 0.0f));
	}

	if (Input::IsKeyPress('L'))
	{
		m_Transform.Rotation *= Transform::ToQuaternion(DirectX::SimpleMath::Vector3(0.0f, 0.01f, 0.0f));
	}
#endif // _DEBUG

}

void DirectionalLight::OnUninit()
{
}
