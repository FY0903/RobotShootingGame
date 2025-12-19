/*+===================================================================
	File: Light.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/18 14:54:36 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Light.hpp"
#include "Utility/Input/Input.hpp"
#include "Utility/LightManager/LightManager.hpp"

// ==============================
//	constexpr
// ==============================
constexpr float LIGHT_ROTATE_SPEED = 0.02f; // ライトの回転速度

void Light::Init(Type type, DirectX::XMVECTOR dir, DirectX::XMFLOAT4 color)
{
	m_Type = type;
	m_Direction = dir;
	m_Color = color;

	// ライトマネージャーに登録
	LightManager::GetInstance().RegisterLight(this);
}

void Light::Update()
{
#ifdef _DEBUG
	if (Input::IsKeyPress('J'))
	{
		m_RadXZ -= LIGHT_ROTATE_SPEED;
	}

	if (Input::IsKeyPress('L'))
	{
		m_RadXZ += LIGHT_ROTATE_SPEED;
	}

	if (Input::IsKeyPress('I'))
	{
		m_RadY += LIGHT_ROTATE_SPEED;
	}

	if (Input::IsKeyPress('K'))
	{
		m_RadY -= LIGHT_ROTATE_SPEED;
	}

	// ライトの方向計算
	m_Direction = DirectX::XMVectorSet(
		cosf(m_RadY) * sinf(m_RadXZ),
		sinf(m_RadY),
		cosf(m_RadY) * cosf(m_RadXZ),
		0.0f);
#endif // _DEBUG
}

void Light::Draw()
{
}

void Light::Uninit()
{
}

DirectX::XMFLOAT4 Light::GetDirection() const
{
	DirectX::XMFLOAT4 dir{};
	DirectX::XMStoreFloat4(&dir, m_Direction);
	return dir;
}
