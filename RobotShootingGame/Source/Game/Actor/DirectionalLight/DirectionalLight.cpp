/*+===================================================================
	File: DirectionalLight.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/18 15:43:42 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "DirectionalLight.hpp"
#include "Utility/Compornent/Light/Light.hpp"

void DirectionalLight::OnInit()
{
	auto light = AddComponent<Light>();
	light->Init(Light::DIRECTIONAL,
		DirectX::XMVectorSet(0.0f, -1.0f, -1.0f, 0.0f),
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
}

void DirectionalLight::OnUpdate()
{
}

void DirectionalLight::OnUninit()
{
}
