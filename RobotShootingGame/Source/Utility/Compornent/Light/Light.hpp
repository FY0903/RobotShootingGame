/*+===================================================================
	File: Light.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/18 14:54:36 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/Compornent/Component.hpp"

/**
 * @brief Lightクラス
 */
class Light : public Component
{
public:
	enum Type
	{
		DIRECTIONAL,
		POINT,
		SPOT,
		MAX,
	};

	using Component::Component;

	void Init(Type type,
		DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		float range = 1.0f, float angle = DirectX::XMConvertToRadians(60.0f));
	void Update() override final;
	void Draw() override final;
	void Uninit() override final;

	inline Type GetType() const { return m_Type; }
	DirectX::XMFLOAT3 GetPosition() const;
	inline float GetRange() const { return m_Range; }
	inline float GetAngle() const { return m_Angle; }
	DirectX::XMFLOAT3 GetDirection() const;
	inline DirectX::XMFLOAT4 GetColor() const { return m_Color; }
	inline void SetColor(const DirectX::XMFLOAT4& color) { m_Color = color; }

private:
	Type m_Type{};
	DirectX::XMFLOAT4 m_Color{};
	float m_Range{};	// ライトの届く距離
	float m_Angle{};	// スポットライトの角度
};
