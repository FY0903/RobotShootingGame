/*+===================================================================
	File: PointLight.cpp
	Summary: 点光源を表すクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/27 14:43 初回作成
			26/01/14 18:39 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "PointLight.hpp"
#include "Utility/Compornent/Light/Light.hpp"

void PointLight::OnInit()
{
	// ライトコンポーネントを追加
	AddComponent<Light>()->Init(Light::POINT,
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		50.0f);

	// 初期位置を設定
	m_Transform.Position = DirectX::XMFLOAT3(0.0f, 5.0f, 0.0f);
}

void PointLight::OnUpdate()
{
}

void PointLight::OnUninit()
{
}
