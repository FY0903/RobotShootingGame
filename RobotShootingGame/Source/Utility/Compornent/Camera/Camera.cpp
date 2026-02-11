/*+===================================================================
	File: Camera.cpp
	Summary: Cameraクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/20 18:19:59 初回作成
			26/01/15 17:03 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Camera.hpp"
#include "Utility/Input/Input.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"
#include "Game/Actor/Actor.hpp"

// ==============================
//	constexpr
// ==============================
constexpr float CAMERA_MOVE_SPEED = 0.04f;	// カメラの移動速度
constexpr float CAMERA_MOUSE_SENSITIVITY = 0.005f; // カメラのマウス感度

void Camera::Init(DirectX::XMVECTOR eyePos, DirectX::XMVECTOR targetPos, DirectX::XMVECTOR upVec, float radius, float fov, float aspect)
{
	// メンバ変数の初期化
	m_EyePos = eyePos;
	m_TargetPos = targetPos;
	m_UpVec = upVec;
	m_Radius = radius;
	m_Fov = fov;
	m_Aspect = aspect;
	m_Near = 0.1f;
	m_Far = 1000.0f;

	// ビュー行列とプロジェクション行列の計算
	m_3DVP[0] = DirectX::XMMatrixLookAtLH(m_EyePos, m_TargetPos, m_UpVec);
	m_3DVP[1] = DirectX::XMMatrixPerspectiveFovLH(m_Fov, m_Aspect, m_Near, m_Far);

	m_2DVP[0] = DirectX::XMMatrixIdentity();
	m_2DVP[1] = DirectX::XMMatrixOrthographicOffCenterLH(
		0.0f, static_cast<float>(WINDOW_WIDTH),
		static_cast<float>(WINDOW_HEIGHT), 0.0f,
		0.0f, 1.0f);
}

void Camera::Update()
{
#ifndef _DEBUG
	// 入力処理
	if (Input::IsKeyPress(VK_LEFT))
	{
		m_RadXZ -= CAMERA_MOVE_SPEED;
	}

	if (Input::IsKeyPress(VK_RIGHT))
	{
		m_RadXZ += CAMERA_MOVE_SPEED;
	}

	if (Input::IsKeyPress(VK_UP))
	{
		m_RadY += CAMERA_MOVE_SPEED;
	}

	if (Input::IsKeyPress(VK_DOWN))
	{
		m_RadY -= CAMERA_MOVE_SPEED;
	}

	if (Input::IsKeyPress(MK_LBUTTON) || Input::IsKeyPress(MK_RBUTTON))
	{
		m_RadXZ += static_cast<float>(Input::GetMouseDifferencePos().x * CAMERA_MOUSE_SENSITIVITY);
		m_RadY += static_cast<float>(Input::GetMouseDifferencePos().y * CAMERA_MOUSE_SENSITIVITY);
	}

	// カメラの位置計算
	m_EyePos = DirectX::XMVectorSet(
		m_Radius * cosf(m_RadY) * sinf(m_RadXZ),
		m_Radius * sinf(m_RadY),
		m_Radius * cosf(m_RadY) * cosf(m_RadXZ),
		0.0f);
#endif // _DEBUG
	
	if (m_Owner)
	{
		// オーナーの位置にカメラ位置を設定
		m_Owner->GetTransform().Position = m_EyePos;
	}

	// ビュー行列とプロジェクション行列の計算
	m_3DVP[0] = DirectX::XMMatrixLookAtLH(m_EyePos, m_TargetPos, m_UpVec);
	// 2Dビュー行列とプロジェクション行列は固定
}

const DirectX::XMFLOAT4X4 Camera::Get3DViewMatrixFloat4x4(bool transpose)
{
	DirectX::XMFLOAT4X4 viewf{};
	DirectX::XMMATRIX wiewMat = m_3DVP[0];

	if (transpose)
		wiewMat = DirectX::XMMatrixTranspose(wiewMat);
	DirectX::XMStoreFloat4x4(&viewf, wiewMat);

	return viewf;
}

const DirectX::XMFLOAT4X4 Camera::Get3DProjectionMatrixFloat4x4(bool transpose)
{
	DirectX::XMFLOAT4X4 projf{};
	DirectX::XMMATRIX projMat = m_3DVP[1];

	if (transpose)
		projMat = DirectX::XMMatrixTranspose(projMat);
	DirectX::XMStoreFloat4x4(&projf, projMat);

	return projf;
}

const DirectX::XMFLOAT4X4 Camera::Get2DViewMatrixFloat4x4(bool transpose)
{
	DirectX::XMFLOAT4X4 viewf{};
	DirectX::XMMATRIX wiewMat = m_2DVP[0];

	if (transpose)
		wiewMat = DirectX::XMMatrixTranspose(wiewMat);
	DirectX::XMStoreFloat4x4(&viewf, wiewMat);

	return viewf;
}

const DirectX::XMFLOAT4X4 Camera::Get2DProjectionMatrixFloat4x4(bool transpose)
{
	DirectX::XMFLOAT4X4 projf{};
	DirectX::XMMATRIX projMat = m_2DVP[1];
	
	if (transpose)
		projMat = DirectX::XMMatrixTranspose(projMat);
	DirectX::XMStoreFloat4x4(&projf, projMat);
	
	return projf;
}

DirectX::XMVECTOR Camera::GetForward() const
{
	DirectX::XMMATRIX invViewMat = DirectX::XMMatrixInverse(nullptr, Get3DViewMatrix());
	DirectX::XMVECTOR forward = invViewMat.r[2];
	return forward;
}

DirectX::XMVECTOR Camera::GetRight() const
{
	DirectX::XMMATRIX invViewMat = DirectX::XMMatrixInverse(nullptr, Get3DViewMatrix());
	DirectX::XMVECTOR right = invViewMat.r[0];
	return right;
}
