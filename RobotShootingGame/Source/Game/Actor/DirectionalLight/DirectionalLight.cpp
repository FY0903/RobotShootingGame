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
#include "Utility/Compornent/Line/Line.hpp"
#include "Utility/Input/Input.hpp"

void DirectionalLight::OnInit()
{
	m_pMaterial = MaterialManager::GetInstance().CreateMaterial("Line");

	// ライトコンポーネントを追加
	auto light = AddComponent<Light>();
	light->Init(Light::DIRECTIONAL, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));	// 太陽光源として初期化

	auto dir = light->GetDirection();
	DirectX::XMVECTOR dirV = DirectX::XMLoadFloat3(&dir);
	DirectX::XMVECTOR posV = DirectX::XMLoadFloat3(&m_Transform.Position);
	DirectX::XMVECTOR targetV = DirectX::XMVectorSubtract(posV, DirectX::XMVectorScale(dirV, 10.0f));
	DirectX::XMFLOAT3 target{};
	DirectX::XMStoreFloat3(&target, targetV);

	auto line = AddComponent<Line>();
	line->AddPoint(m_Transform.Position, target, DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));
	line->Create();

	m_Transform.Rotation = Transform::ToQuaternion(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(-1.0f), 0.0f, 0.0f));
	m_pMaterial->SetIsOpaque(false);
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
