/*+===================================================================
	File: Light.cpp
	Summary: Lightクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/18 14:54 初回作成
			26/01/15 17:52 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Light.hpp"
#include "Utility/Input/Input.hpp"
#include "Utility/LightManager/LightManager.hpp"
#include "Game/Actor/Actor.hpp"
#include "Utility/Transform/Transform.hpp"

// ==============================
//	constexpr
// ==============================
constexpr float LIGHT_ROTATE_SPEED = 0.02f; // ライトの回転速度

void Light::Init(Type type, DirectX::XMFLOAT4 color, float range, float angle)
{
	m_Type = type;
	m_Color = color;
	m_Range = range;
	m_Angle = angle;

	// ライトマネージャーに登録
	LightManager::GetInstance().RegisterLight(this);
}

DirectX::XMFLOAT3 Light::GetPosition() const
{
	// オーナーが存在しない場合はデフォルト値を返す
	if (!m_Owner) return DirectX::XMFLOAT3{};

	Transform transform = m_Owner->GetTransform();
	return transform.Position;
}

DirectX::XMFLOAT3 Light::GetDirection() const
{
	// オーナーが存在しない場合はデフォルト値を返す
	if (!m_Owner) return DirectX::XMFLOAT3{};

	// ライトの向きを計算
	Transform transform = m_Owner->GetTransform();
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
	DirectX::SimpleMath::Vector3 direction = DirectX::XMVector3Rotate(forward, transform.Rotation);
	
	return DirectX::XMFLOAT3(direction.x, direction.y, direction.z);
}
