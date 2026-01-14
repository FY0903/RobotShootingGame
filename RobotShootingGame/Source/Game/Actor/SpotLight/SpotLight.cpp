/*+===================================================================
	File: SpotLight.cpp
	Summary: スポットライトを表すクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/27 18:49 初回作成
			26/01/14 18:40 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SpotLight.hpp"
#include "Utility/Compornent/Light/Light.hpp"

void SpotLight::OnInit()
{
	// ライトコンポーネントを追加
	AddComponent<Light>()->Init(Light::SPOT,
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		50.0f);

	// 初期位置と回転を設定
	m_Transform.Position = DirectX::XMFLOAT3(3.0f, 3.0f, 3.0f);
	m_Transform.Rotation = Transform::ToQuaternion(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(45.0f), 0.0f, 0.0f));
}

void SpotLight::OnUpdate()
{
}

void SpotLight::OnUninit()
{
}
