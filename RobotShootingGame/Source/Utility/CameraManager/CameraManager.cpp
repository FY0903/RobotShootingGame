/*+===================================================================
	File: CameraManager.cpp
	Summary: カメラ管理クラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/23 16:39 初回作成
			26/01/14 16:50 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "CameraManager.hpp"

void CameraManager::RegisterCamera(const std::string& name, Camera* pCamera)
{
	m_pCameraData[name] = pCamera;

	// 最初に登録されたカメラをメインカメラに設定
	SetMainCamera(name);
}

void CameraManager::SetMainCamera(const std::string& name)
{
	// 指定された名前のカメラが存在する場合、メインカメラを設定
	if (m_pCameraData.find(name) != m_pCameraData.end())
	{
		m_pMainCamera = m_pCameraData[name];
	}
}

float CameraManager::CalculateDistanceToMainCamera(const DirectX::XMFLOAT4X4& worldMat) const
{
	// カメラとオブジェクトの位置ベクトルを取得
	DirectX::XMFLOAT3 cameraPos = GetMainCameraPosition();
	DirectX::XMVECTOR camPosVec = DirectX::XMVectorSet(cameraPos.x, cameraPos.y, cameraPos.z, 0.0f);
	DirectX::XMVECTOR objPosVec = DirectX::XMVectorSet(worldMat._41, worldMat._42, worldMat._43, 0.0f);

	// 距離を計算
	DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(camPosVec, objPosVec);
	DirectX::XMVECTOR distVec = DirectX::XMVector3Length(diffVec);

	// 結果をfloat型に変換して返す
	float distance{};
	DirectX::XMStoreFloat(&distance, distVec);

	return distance;
}

DirectX::XMFLOAT3 CameraManager::GetMainCameraPosition() const
{
	Camera* pCamera = GetMainCamera();
	if (!pCamera) return DirectX::XMFLOAT3{};

	// ビュー行列を取得
	DirectX::XMFLOAT4X4 viewMat = pCamera->Get3DViewMatrixFloat4x4(false);
	DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&viewMat);
	DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(nullptr, view);

	// 逆行列をXMFLOAT4X4に変換
	DirectX::XMFLOAT4X4 invViewMat{};
	DirectX::XMStoreFloat4x4(&invViewMat, invView);

	// カメラ位置を取得
	return DirectX::XMFLOAT3{ invViewMat._41, invViewMat._42, invViewMat._43 };
}

CameraManager::~CameraManager()
{
	m_pMainCamera = nullptr;
	m_pCameraData.clear();
}
