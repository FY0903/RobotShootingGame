/*+===================================================================
	File: Transform.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/14 11:54:12 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

#pragma once

// ==============================
//	include
// ==============================
#include <DirectXMath.h>

// ==============================
//	using
// ==============================
using Vector3 = DirectX::XMFLOAT3;

struct Transform
{
	Vector3 Position{}; // 位置
	Vector3 Rotation{}; // 回転（オイラー角）
	Vector3 Scale{}; // スケール

	Transform()
		: Position(0.0f, 0.0f, 0.0f)
		, Rotation(0.0f, 0.0f, 0.0f)
		, Scale(1.0f, 1.0f, 1.0f)
	{
	}

	Transform(const Vector3& position)
		: Position(position)
		, Rotation(0.0f, 0.0f, 0.0f)
		, Scale(1.0f, 1.0f, 1.0f)
	{
	}

	Transform(const Vector3& position, const Vector3& rotation)
		: Position(position)
		, Rotation(rotation)
		, Scale(1.0f, 1.0f, 1.0f)
	{
	}

	Transform(const Vector3& position, const Vector3& rotation, const Vector3& scale)
		: Position(position)
		, Rotation(rotation)
		, Scale(scale)
	{
	}

	DirectX::XMMATRIX GetWorldMatrix() const
	{
		DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(Position.x, Position.y, Position.z);
		return scaleMatrix * rotationMatrix * translationMatrix;
	}
};
