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

void DirectionalLight::OnInit()
{
	// ライトコンポーネントを追加
	auto light = AddComponent<Light>();
	light->Init(Light::DIRECTIONAL);	// 太陽光源として初期化
}

void DirectionalLight::OnUpdate()
{
}

void DirectionalLight::OnUninit()
{
}
