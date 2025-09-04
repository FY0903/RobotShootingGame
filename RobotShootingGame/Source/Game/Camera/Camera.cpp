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

Camera::Camera(DirectX::XMVECTOR eyePos, DirectX::XMVECTOR targetPos, DirectX::XMVECTOR upVec, float fov, float aspect)
	: m_EyePos(eyePos), m_TargetPos(targetPos), m_UpVec(upVec), m_Fov(fov), m_Aspect(aspect)
{

}

void Camera::Update()
{
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
		m_RadXZ += static_cast<float>(Input::GetMouseDifferencePos().x * cx_fCameraMouseSensitivity);
		m_RadY += static_cast<float>(Input::GetMouseDifferencePos().y * cx_fCameraMouseSensitivity);
	}
}
