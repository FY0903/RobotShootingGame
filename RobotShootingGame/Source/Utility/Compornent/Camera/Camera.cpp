/*+===================================================================
	File: Camera.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/20 18:19:59 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Camera.hpp"
#include "Utility/Input/Input.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"

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

	// ビュー行列とプロジェクション行列の計算
	m_3DVP[0] = DirectX::XMMatrixLookAtLH(m_EyePos, m_TargetPos, m_UpVec);
	m_3DVP[1] = DirectX::XMMatrixPerspectiveFovLH(m_Fov, m_Aspect, 0.1f, 1000.0f);

	m_2DVP[0] = DirectX::XMMatrixIdentity();
	m_2DVP[1] = DirectX::XMMatrixOrthographicLH(WINDOW_WIDTH, WINDOW_HEIGHT, 0.1f, 1000.0f);
}

void Camera::Update()
{
#ifdef _DEBUG
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
}

void Camera::Draw()
{
	// ビュー行列とプロジェクション行列の計算
	m_3DVP[0] = DirectX::XMMatrixLookAtLH(m_EyePos, m_TargetPos, m_UpVec);
	m_3DVP[1] = DirectX::XMMatrixPerspectiveFovLH(m_Fov, m_Aspect, 0.1f, 1000.0f);

	// 2Dビュー行列とプロジェクション行列は固定
}

void Camera::Uninit()
{
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
