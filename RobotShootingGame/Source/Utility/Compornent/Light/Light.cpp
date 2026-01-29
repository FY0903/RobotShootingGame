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
#include "Utility/CameraManager/CameraManager.hpp"

// ==============================
//	define
// ==============================
#undef near
#undef far

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

	if (m_Type == Type::DIRECTIONAL)
	{
		// シャドウマップマネージャーに登録
		ShadowMapManager::GetInstance().CreateShadowMap(this, ShadowMap::ULTRA);
	}

	// ビュー行列とプロジェクション行列の計算
	m_VP[0] = DirectX::XMMatrixIdentity();
	m_VP[1] = DirectX::XMMatrixOrthographicLH(10.0f, 10.0f, 0.1f, 5.0f);
}

void Light::Update()
{
	DirectX::XMFLOAT3 position = GetPosition();
	DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
	DirectX::XMVECTOR targetVec = DirectX::XMVectorZero();
	DirectX::XMFLOAT3 direction = GetDirection();
	DirectX::XMVECTOR dirVec = DirectX::XMLoadFloat3(&direction);
	targetVec = DirectX::XMVectorSubtract(pos, DirectX::XMVectorScale(dirVec, 2.0f));

	m_VP[0] = DirectX::XMMatrixLookAtLH(pos, targetVec, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

#if 0

	DirectX::XMMATRIX VP = m_VP[0] * m_VP[1];

	const auto cam = CameraManager::GetInstance().GetMainCamera();

	const auto& camPos = cam->GetPosition();
	const auto& camForward = cam->GetForward();
	const auto& camRight = cam->GetRight();
	const auto& camUp = DirectX::XMVector3Cross(camForward, camRight);
	const auto& camFov = cam->GetFov();
	const auto& camAspect = cam->GetAspect();
	const auto& camNear = cam->GetNear();
	const auto& camFar = cam->GetFar();

	float nearY = tanf(camFov * 0.5f) * camNear;
	float nearX = nearY * camAspect;
	float farY = tanf(camFov * 0.5f) * camFar;
	float farX = farY * camAspect;

	DirectX::XMVECTOR nearCenter = DirectX::XMVectorAdd(
		camPos,
		DirectX::XMVectorScale(camForward, camNear));
	DirectX::XMVECTOR farCenter = DirectX::XMVectorAdd(
		camPos,
		DirectX::XMVectorScale(camForward, camFar));

	DirectX::XMVECTOR frustumCorners[8]{};

	// 近クリップ面
	frustumCorners[0] = DirectX::XMVectorAdd(
		nearCenter,
		DirectX::XMVectorAdd(
			DirectX::XMVectorScale(camUp, nearY),
			DirectX::XMVectorScale(camRight, nearX)));
	frustumCorners[1] = DirectX::XMVectorAdd(
		nearCenter,
		DirectX::XMVectorAdd(
			DirectX::XMVectorScale(camUp, nearY),
			DirectX::XMVectorScale(camRight, -nearX)));
	frustumCorners[2] = DirectX::XMVectorAdd(
		nearCenter,
		DirectX::XMVectorAdd(
			DirectX::XMVectorScale(camUp, -nearY),
			DirectX::XMVectorScale(camRight, nearX)));
	frustumCorners[3] = DirectX::XMVectorAdd(
		nearCenter,
		DirectX::XMVectorAdd(
			DirectX::XMVectorScale(camUp, -nearY),
			DirectX::XMVectorScale(camRight, -nearX)));

	// 遠クリップ面
	frustumCorners[4] = DirectX::XMVectorAdd(
		farCenter,
		DirectX::XMVectorAdd(
			DirectX::XMVectorScale(camUp, farY),
			DirectX::XMVectorScale(camRight, farX)));
	frustumCorners[5] = DirectX::XMVectorAdd(
		farCenter,
		DirectX::XMVectorAdd(
			DirectX::XMVectorScale(camUp, farY),
			DirectX::XMVectorScale(camRight, -farX)));
	frustumCorners[6] = DirectX::XMVectorAdd(
		farCenter,
		DirectX::XMVectorAdd(
			DirectX::XMVectorScale(camUp, -farY),
			DirectX::XMVectorScale(camRight, farX)));
	frustumCorners[7] = DirectX::XMVectorAdd(
		farCenter,
		DirectX::XMVectorAdd(
			DirectX::XMVectorScale(camUp, -farY),
			DirectX::XMVectorScale(camRight, -farX)));

	DirectX::XMVECTOR vMax{}, vMin{};
	vMax = DirectX::XMVectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);
	vMin = DirectX::XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
	for (auto& corner : frustumCorners)
	{
		// ビュープロジェクション行列で変換
		DirectX::XMVECTOR trfCorner = DirectX::XMVector3Transform(corner, VP);
		vMax = DirectX::XMVectorMax(vMax, trfCorner);
		vMin = DirectX::XMVectorMin(vMin, trfCorner);
	}

	float vMaxX = DirectX::XMVectorGetX(vMax);
	float vMinX = DirectX::XMVectorGetX(vMin);
	float vMaxY = DirectX::XMVectorGetY(vMax);
	float vMinY = DirectX::XMVectorGetY(vMin);

	float xScale = 2.0f / (vMaxX - vMinX);
	float yScale = 2.0f / (vMaxY - vMinY);
	float xOffset = -0.5f * (vMaxX + vMinX) * xScale;
	float yOffset = -0.5f * (vMaxY + vMinY) * yScale;
	DirectX::XMMATRIX cropMat =
		DirectX::XMMatrixSet(
			xScale, 0.0f, 0.0f, 0.0f,
			0.0f, yScale, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			xOffset, yOffset, 0.0f, 1.0f);

	m_VPC = VP;
#endif
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
	DirectX::XMVector3Normalize(direction);
	DirectX::XMFLOAT3 directionF{};
	DirectX::XMStoreFloat3(&directionF, direction);
	return directionF;
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

DirectX::XMFLOAT4X4 Light::GetViewProjectionCropMatrixFloat4x4(bool transpose) const
{
	DirectX::XMFLOAT4X4 lvpF{};
	DirectX::XMMATRIX lvpMat = m_VPC;
	if (transpose)
		lvpMat = DirectX::XMMatrixTranspose(lvpMat);
	DirectX::XMStoreFloat4x4(&lvpF, lvpMat);
	return lvpF;
}
