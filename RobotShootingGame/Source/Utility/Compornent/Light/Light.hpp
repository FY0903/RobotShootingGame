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
		DirectX::XMVECTOR dir = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
		DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	void Update() override final;
	void Draw() override final;
	void Uninit() override final;

	inline Type GetType() const { return m_Type; }
	DirectX::XMFLOAT4 GetDirection() const;
	inline DirectX::XMFLOAT4 GetColor() const { return m_Color; }
	inline void SetColor(const DirectX::XMFLOAT4& color) { m_Color = color; }

private:
	Type m_Type{};
	DirectX::XMVECTOR m_Direction{};
	DirectX::XMFLOAT4 m_Color{};
	float m_RadXZ{};	// XZ平面でのラジアン
	float m_RadY{};		// Y軸でのラジアン
};
