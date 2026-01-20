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
#include "Utility/ShadowMapManager/ShadowMapManager.hpp"
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

	// ビュー行列とプロジェクション行列の計算
	m_VP[0] = DirectX::XMMatrixIdentity();
	m_VP[1] = DirectX::XMMatrixOrthographicLH(50.0f, 50.0f, 0.1f, 1000.0f);

	// ライトマネージャーに登録
	LightManager::GetInstance().RegisterLight(this);

	if (m_Type == Type::DIRECTIONAL)
	{
		// シャドウマップマネージャーに登録
		ShadowMapManager::GetInstance().CreateShadowMap(this, ShadowMap::ULTRA);
	}
}

void Light::Update()
{
	DirectX::XMFLOAT3 pos = GetPosition();
	DirectX::XMFLOAT3 dir = GetDirection();

	// ビュー行列の計算
	m_VP[0] = DirectX::XMMatrixLookAtLH(
		DirectX::XMVectorSet(
			pos.x, pos.y, pos.z, 0.0f),
		DirectX::XMVectorSet(
			pos.x + dir.x,
			pos.y + dir.y,
			pos.z + dir.z,
			0.0f
		),
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

void Light::Uninit()
{
	// シャドウマップマネージャーから破棄
	ShadowMapManager::GetInstance().DestroyShadowMap(this);
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

DirectX::XMFLOAT4X4 Light::GetViewMatrixFloat4x4(bool transpose) const
{
	DirectX::XMFLOAT4X4 viewf{};
	DirectX::XMMATRIX wiewMat = m_VP[0];

	if (transpose)
		wiewMat = DirectX::XMMatrixTranspose(wiewMat);
	DirectX::XMStoreFloat4x4(&viewf, wiewMat);

	return viewf;
}

DirectX::XMFLOAT4X4 Light::GetProjectionMatrixFloat4x4(bool transpose) const
{
	DirectX::XMFLOAT4X4 projf{};
	DirectX::XMMATRIX projMat = m_VP[1];

	if (transpose)
		projMat = DirectX::XMMatrixTranspose(projMat);
	DirectX::XMStoreFloat4x4(&projf, projMat);
	
	return projf;
}
