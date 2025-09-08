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
#include "../../Utility/Input/Input.hpp"

// ==============================
//	constexpr
// ==============================
constexpr float CAMERA_MOVE_SPEED = 0.04f;	// カメラの移動速度
constexpr float CAMERA_MOUSE_SENSITIVITY = 0.002f; // カメラのマウス感度
constexpr float CAMERA_ZOOM_SPEED = 0.5f; // カメラのズーム速度
constexpr float CAMERA_LIMIT_ZOOM_IN = 1.0f; // カメラのズームイン限界
constexpr float CAMERA_LIMIT_ZOOM_OUT = 15.0f; // カメラのズームアウト限界

Camera::Camera(DirectX::XMVECTOR eyePos, DirectX::XMVECTOR targetPos, DirectX::XMVECTOR upVec, float radius, float fov, float aspect)
	: m_EyePos(eyePos), m_TargetPos(targetPos), m_UpVec(upVec), m_Radius(radius), m_Fov(fov), m_Aspect(aspect)
{
}

void Camera::Update()
{
	KeyMove();
	MouseMove();
	Zoom();

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
		if (CAMERA_LIMIT_ZOOM_OUT > m_Radius)
		{
			m_Radius += CAMERA_ZOOM_SPEED;
		}
	}
}
