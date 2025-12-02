/*+===================================================================
	File: Camera.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/20 18:19:59 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/Compornent/Component.hpp"
#include "System/main.hpp"
#include "System/Engine/Engine.hpp"
#include "Utility/ConstantBuffer/ConstantBuffer.hpp"
#include "Utility/RootSignature/RootSignature.hpp"

/**
 * @brief Cameraクラス
 */
class Camera : public Component
{
public:
	Component::Component;

	/**
	 * コンストラクタ
	 */
	Camera() = default;

	/**
	 * デストラクタ
	 */
	~Camera() = default;

	void Init(DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(0.0f, 5.0f, 5.0f, 0.0f),
		DirectX::XMVECTOR targetPos = DirectX::XMVectorZero(),
		DirectX::XMVECTOR upVec = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
		float radius = 10.0f,
		float fov = DirectX::XMConvertToRadians(60.0f),
		float aspect = 16.0f / 9.0f);
	void Update() override final;
	void Draw() override final;
	void Uninit() override final;

	const DirectX::XMFLOAT4X4 GetViewMatrixFloat4x4(bool transpose = true);

	const DirectX::XMFLOAT4X4 GetProjectionMatrixFloat4x4(bool transpose = true);

private:
	DirectX::XMMATRIX m_VP[2]{}; // ビュー/プロジェクション行列

	DirectX::XMVECTOR m_EyePos{};		// 視線位置
	DirectX::XMVECTOR m_TargetPos{};	// 注視点
	DirectX::XMVECTOR m_UpVec{};		// 上方ベクトル

	float m_Radius{};	// 半径
	float m_RadXZ{};	// XZ平面でのラジアン
	float m_RadY{};		// Y軸でのラジアン

	float m_Fov{};		// 視野角
	float m_Aspect{};	// アスペクト比
};
