/*+===================================================================
	File: Camera.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/04 14:17:39 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Camera.hpp"
#include "Utility/Input/Input.hpp"

// ==============================
//	constexpr
// ==============================
constexpr float CAMERA_MOVE_SPEED = 0.04f;	// カメラの移動速度
constexpr float CAMERA_MOUSE_SENSITIVITY = 0.005f; // カメラのマウス感度
constexpr float CAMERA_ZOOM_SPEED = 0.5f; // カメラのズーム速度
constexpr float CAMERA_LIMIT_ZOOM_IN = 1.0f; // カメラのズームイン限界
constexpr float CAMERA_MIDDLE_BUTTON_SENSITIVITY = 0.001f; // カメラのマウス中ボタン感度

Camera::Camera(DirectX::XMVECTOR eyePos, DirectX::XMVECTOR targetPos, DirectX::XMVECTOR upVec, float radius, float fov, float aspect)
	: m_EyePos(eyePos), m_TargetPos(targetPos), m_UpVec(upVec), m_Radius(radius), m_Fov(fov), m_Aspect(aspect)
{
}

void Camera::Update()
{
	KeyMove();
	MouseMove();
	Zoom();
	MoveTarget();

	m_EyePos = DirectX::XMVectorSet(
		m_Radius * cosf(m_RadY) * sinf(m_RadXZ),
		m_Radius * sinf(m_RadY),
		m_Radius * cosf(m_RadY) * cosf(m_RadXZ),
		0.0f);
}

void Camera::KeyMove()
{
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
}

void Camera::MouseMove()
{
	if (Input::IsKeyPress(MK_LBUTTON) || Input::IsKeyPress(MK_RBUTTON))
	{
		m_RadXZ += static_cast<float>(Input::GetMouseDifferencePos().x * CAMERA_MOUSE_SENSITIVITY);
		m_RadY += static_cast<float>(Input::GetMouseDifferencePos().y * CAMERA_MOUSE_SENSITIVITY);
	}
}

void Camera::Zoom()
{
	// マウスホイールが上にスクロールされた場合
	if (Input::GetMouseWheelInput() > 0)
	{
		if (CAMERA_LIMIT_ZOOM_IN < m_Radius)
		{
			m_Radius -= CAMERA_ZOOM_SPEED;
		}
	}

	// マウスホイールが下にスクロールされた場合
	if (Input::GetMouseWheelInput() < 0)
	{
		m_Radius += CAMERA_ZOOM_SPEED;
	}
}

void Camera::MoveTarget()
{
	if (Input::IsMiddleButtonPress())
	{
		// ワールド行列を作成
        DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation(
            DirectX::XMVectorGetX(m_EyePos),
            DirectX::XMVectorGetY(m_EyePos),
            DirectX::XMVectorGetZ(m_EyePos)
        );

		// 逆行列を計算
		DirectX::XMMATRIX invWorldMatrix = DirectX::XMMatrixInverse(nullptr, worldMatrix);

		// ローカル座標の上方向ベクトルを計算
		DirectX::XMVECTOR localUpVec = DirectX::XMVector3TransformNormal(m_UpVec, invWorldMatrix);

		// マウスの移動量を取得
		float dx = Input::GetMouseDifferencePos().x;
		float dy = Input::GetMouseDifferencePos().y;

		// カメラの向きベクトルと右方向ベクトルを計算
		DirectX::XMVECTOR forward = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(m_TargetPos, m_EyePos));
		DirectX::XMVECTOR right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(localUpVec, forward));

		// 移動量を計算してカメラの位置と注視点を更新
		DirectX::XMVECTOR move = DirectX::XMVectorAdd(DirectX::XMVectorScale(right, -dx * CAMERA_MIDDLE_BUTTON_SENSITIVITY * m_Radius),
			DirectX::XMVectorScale(localUpVec, dy * CAMERA_MIDDLE_BUTTON_SENSITIVITY * m_Radius));

		// 注視点とカメラ位置を更新
		m_TargetPos = DirectX::XMVectorAdd(m_TargetPos, move);
		m_EyePos = DirectX::XMVectorAdd(m_EyePos, move);
	}

	// Fキーで注視点を原点にリセット
	if (Input::IsKeyPress('F')) m_TargetPos = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}
