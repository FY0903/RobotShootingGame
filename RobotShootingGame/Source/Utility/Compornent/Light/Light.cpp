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
	m_VP[1] = DirectX::XMMatrixOrthographicLH(10.0f, 10.0f, 0.1f, 1000.0f);
}

void Light::Update()
{
	// 所有者の位置／方向を取得
	DirectX::XMFLOAT3 position = GetPosition();
	DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
	DirectX::XMFLOAT3 directionF = GetDirection();
	DirectX::XMVECTOR dirVec = DirectX::XMLoadFloat3(&directionF);
	DirectX::XMVector3Normalize(dirVec);

	// メインカメラを取得（なければ従来の単純なLookAtでフォールバック）
	const auto cam = CameraManager::GetInstance().GetMainCamera();
	if (!cam)
	{
		DirectX::XMVECTOR targetVec = DirectX::XMVectorSubtract(pos, DirectX::XMVectorScale(dirVec, 10.0f));
		m_VP[0] = DirectX::XMMatrixLookAtLH(pos, targetVec, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
		return;
	}

	// カメラ情報
	const auto& camPos = cam->GetPosition();
	const auto& camForward = cam->GetForward();
	const auto& camRight = cam->GetRight();
	const auto& camUp = DirectX::XMVector3Cross(camForward, camRight);
	const float camFov = cam->GetFov();
	const float camAspect = cam->GetAspect();
	const float camNear = cam->GetNear();
	const float camFar = cam->GetFar();

	// 視錐台のコーナーをワールド空間で計算
	float nearY = tanf(camFov * 0.5f) * camNear;
	float nearX = nearY * camAspect;
	float farY = tanf(camFov * 0.5f) * 5.0f;
	float farX = farY * camAspect;

	DirectX::XMVECTOR nearCenter = DirectX::XMVectorAdd(camPos, DirectX::XMVectorScale(camForward, camNear));
	DirectX::XMVECTOR farCenter = DirectX::XMVectorAdd(camPos, DirectX::XMVectorScale(camForward, 5.0f));

	DirectX::XMVECTOR frustumCorners[8]{};
	// 近面
	frustumCorners[0] = DirectX::XMVectorAdd(nearCenter, DirectX::XMVectorAdd(DirectX::XMVectorScale(camUp, nearY), DirectX::XMVectorScale(camRight, nearX)));
	frustumCorners[1] = DirectX::XMVectorAdd(nearCenter, DirectX::XMVectorAdd(DirectX::XMVectorScale(camUp, nearY), DirectX::XMVectorScale(camRight, -nearX)));
	frustumCorners[2] = DirectX::XMVectorAdd(nearCenter, DirectX::XMVectorAdd(DirectX::XMVectorScale(camUp, -nearY), DirectX::XMVectorScale(camRight, nearX)));
	frustumCorners[3] = DirectX::XMVectorAdd(nearCenter, DirectX::XMVectorAdd(DirectX::XMVectorScale(camUp, -nearY), DirectX::XMVectorScale(camRight, -nearX)));
	// 遠面
	frustumCorners[4] = DirectX::XMVectorAdd(farCenter, DirectX::XMVectorAdd(DirectX::XMVectorScale(camUp, farY), DirectX::XMVectorScale(camRight, farX)));
	frustumCorners[5] = DirectX::XMVectorAdd(farCenter, DirectX::XMVectorAdd(DirectX::XMVectorScale(camUp, farY), DirectX::XMVectorScale(camRight, -farX)));
	frustumCorners[6] = DirectX::XMVectorAdd(farCenter, DirectX::XMVectorAdd(DirectX::XMVectorScale(camUp, -farY), DirectX::XMVectorScale(camRight, farX)));
	frustumCorners[7] = DirectX::XMVectorAdd(farCenter, DirectX::XMVectorAdd(DirectX::XMVectorScale(camUp, -farY), DirectX::XMVectorScale(camRight, -farX)));

	// 視錐台の中心を求める
	DirectX::XMVECTOR center = DirectX::XMVectorZero();
	for (int i = 0; i < 8; ++i) center = DirectX::XMVectorAdd(center, frustumCorners[i]);
	center = DirectX::XMVectorScale(center, 1.0f / 8.0f);

	const float lightDistance = 5.0f;
	DirectX::XMVECTOR lightPos = DirectX::XMVectorSubtract(center, DirectX::XMVectorScale(dirVec, lightDistance));

	// up ベクトルの決定（ライト方向とほぼ平行にならないようにする）
	DirectX::XMVECTOR upVec = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	float dot = fabs(DirectX::XMVectorGetX(DirectX::XMVector3Dot(upVec, dirVec)));
	if (dot > 0.99f) // ほぼ平行なら別の up を使う
		upVec = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	// ライトのビュー行列
	DirectX::XMMATRIX lightView = DirectX::XMMatrixLookAtLH(lightPos, center, upVec);

	// 視錐台のコーナーをライト空間に変換して AABB を求める
	DirectX::XMVECTOR vMin = DirectX::XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
	DirectX::XMVECTOR vMax = DirectX::XMVectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (int i = 0; i < 8; ++i)
	{
		DirectX::XMVECTOR lc = DirectX::XMVector3Transform(frustumCorners[i], lightView);
		vMin = DirectX::XMVectorMin(vMin, lc);
		vMax = DirectX::XMVectorMax(vMax, lc);
	}

	float sizeX = DirectX::XMVectorGetX(DirectX::XMVectorSubtract(vMax, vMin));
	float sizeY = DirectX::XMVectorGetY(DirectX::XMVectorSubtract(vMax, vMin));

	DirectX::XMMATRIX lightProj = DirectX::XMMatrixOrthographicLH(sizeX, sizeY, 0.1f, 1000.0f);

	m_VP[0] = lightView;
	m_VP[1] = lightProj;
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
