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
	AddComponent<Light>();
}

void DirectionalLight::OnUpdate()
{
}

void DirectionalLight::OnUninit()
{
}
