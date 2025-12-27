/*+===================================================================
	File: SpotLight.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/27 18:49:31 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SpotLight.hpp"
#include "Utility/Compornent/Light/Light.hpp"

void SpotLight::OnInit()
{
	AddComponent<Light>()->Init(Light::SPOT,
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		50.0f);
	m_Transform.Position = DirectX::XMFLOAT3(3.0f, 3.0f, 3.0f);
	m_Transform.Rotation = Transform::ToQuaternion(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(45.0f), 0.0f, 0.0f));
}

void SpotLight::OnUpdate()
{
}

void SpotLight::OnUninit()
{
}
