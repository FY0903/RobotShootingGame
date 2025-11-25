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
#include <SimpleMath.h>

// ==============================
//	using
// ==============================

struct Transform
{
	DirectX::SimpleMath::Vector3 Position{};	// 位置
	DirectX::SimpleMath::Quaternion Rotation{}; // 回転（オイラー角）
	DirectX::SimpleMath::Vector3 Scale{};		// スケール

	Transform()
		: Position(0.0f, 0.0f, 0.0f)
		, Rotation(0.0f, 0.0f, 0.0f, 1.0f)
		, Scale(1.0f, 1.0f, 1.0f)
	{
	}

	DirectX::SimpleMath::Quaternion ToQuaternion(const DirectX::SimpleMath::Vector3& eulerAngles)
	{
		// TODO: 後で自分で作成する
		return DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(eulerAngles.y, eulerAngles.x, eulerAngles.z);
	}

	DirectX::SimpleMath::Vector3 ToEulerAngles(const DirectX::SimpleMath::Quaternion& quaternion)
	{
		// TODO: 後で自分で作成する
		return quaternion.ToEuler();
	}

	DirectX::XMMATRIX GetWorldMatrix()
	{
		DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(Rotation);
		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(Position.x, Position.y, Position.z);
		return scaleMatrix * rotationMatrix * translationMatrix;
	}

	DirectX::XMFLOAT4X4 GetWorldMatrixFloat4x4(bool transpose = true)
	{
		DirectX::XMFLOAT4X4 worldf{};
		DirectX::XMMATRIX worldMat = GetWorldMatrix();

		if (transpose)
			worldMat = DirectX::XMMatrixTranspose(worldMat);

		DirectX::XMStoreFloat4x4(&worldf, worldMat);

		return worldf;
	}
};
