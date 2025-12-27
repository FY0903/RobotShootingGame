/*+===================================================================
	File: PointLight.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/27 14:43:37 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "PointLight.hpp"
#include "Utility/Compornent/Light/Light.hpp"

void PointLight::OnInit()
{
	AddComponent<Light>()->Init(Light::POINT,
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		50.0f);

	m_Transform.Position = DirectX::XMFLOAT3(0.0f, 5.0f, 0.0f);
}

void PointLight::OnUpdate()
{
}

void PointLight::OnUninit()
{
}
