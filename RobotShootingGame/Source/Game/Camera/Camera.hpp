/*+===================================================================
	File: Camera.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/04 14:17:39 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <DirectXMath.h>

/**
 * @brief Cameraクラス
 */
class Camera
{
public:
	/**
	 * コンストラクタ
	 */
	Camera(DirectX::XMVECTOR eyePos, DirectX::XMVECTOR targetPos, DirectX::XMVECTOR upVec, float radius, float fov, float aspect);

	/**
	 * デストラクタ
	 */
	~Camera() = default;

	DirectX::XMMATRIX GetViewMatrix() const
	{
		return DirectX::XMMatrixLookAtLH(m_EyePos, m_TargetPos, m_UpVec);
	}

	DirectX::XMMATRIX GetProjectionMatrix() const
	{
		return DirectX::XMMatrixPerspectiveFovLH(m_Fov, m_Aspect, 0.1f, 1000.0f);
	}

	void Update();

private:
	void KeyMove();
	void MouseMove();
	void Zoom();

	DirectX::XMVECTOR m_EyePos{};
	DirectX::XMVECTOR m_TargetPos{};
	DirectX::XMVECTOR m_UpVec{};

	float m_Radius{}; // 半径
	float m_RadXZ{}; // XZ平面でのラジアン
	float m_RadY{};  // Y軸でのラジアン

	float m_Fov{};
	float m_Aspect{};
};
