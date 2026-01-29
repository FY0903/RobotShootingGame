/*+===================================================================
	File: Fog.cpp
	Summary: Fogクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2026/01/17 17:52 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Fog.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"
#include "Utility/CameraManager/CameraManager.hpp"
#include "Utility/LightManager/LightManager.hpp"
#include "Utility/Compornent/Light/Light.hpp"

Fog::~Fog()
{
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		delete m_pCameraCB[i];
		m_pCameraCB[i] = nullptr;

		delete m_pLVPCB[i];
		m_pLVPCB[i] = nullptr;
	}
}

void Fog::Init()
{
	// 定数バッファの生成
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_pCameraCB[i] = new ConstantBuffer(sizeof(CB::Camera));
		assert(m_pCameraCB[i]);	// nullptrチェック
		m_pLVPCB[i] = new ConstantBuffer(sizeof(CB::LVP));
		assert(m_pLVPCB[i]);	// nullptrチェック
	}

	// ルートシグネチャとパイプラインステートの生成
	CreateRootSignature();
	CreatePSO();
}

void Fog::UpdateCB()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得
	auto commandList = Engine::GetInstance().GetCommandList();				// コマンドリストを取得

	// カメラの逆VP行列を定数バッファに転送
	DirectX::XMMATRIX V = CameraManager::GetInstance().GetMainCamera()->Get3DViewMatrix();
	DirectX::XMMATRIX P = CameraManager::GetInstance().GetMainCamera()->Get3DProjectionMatrix();
	DirectX::XMMATRIX invV = DirectX::XMMatrixInverse(nullptr, V);
	DirectX::XMMATRIX invP = DirectX::XMMatrixInverse(nullptr, P);
	DirectX::XMFLOAT4X4 invVMat{}, invPMat{};
	DirectX::XMStoreFloat4x4(&invVMat, invV);
	DirectX::XMStoreFloat4x4(&invPMat, invP);
	CB::Camera* cameraPtr = m_pCameraCB[currentIndex]->GetPtr<CB::Camera>();
	cameraPtr->invVMat = invVMat;
	cameraPtr->invPMat = invPMat;
	cameraPtr->PMat = CameraManager::GetInstance().GetMainCamera()->Get3DProjectionMatrixFloat4x4(false);
	cameraPtr->Pos = CameraManager::GetInstance().GetMainCameraPosition();

	// ライトVP行列を定数バッファに転送
	CB::LVP* lvpPtr = m_pLVPCB[currentIndex]->GetPtr<CB::LVP>();
	lvpPtr->VMat = LightManager::GetInstance().GetLights()[0]->GetViewMatrixFloat4x4(false);
	lvpPtr->PMat = LightManager::GetInstance().GetLights()[0]->GetProjectionMatrixFloat4x4(false);

	commandList->SetGraphicsRootConstantBufferView(2, m_pCameraCB[currentIndex]->GetAddress()); // カメラ用定数バッファを設定
	commandList->SetGraphicsRootConstantBufferView(3, m_pLVPCB[currentIndex]->GetAddress());     // ライトVP用定数バッファを設定
}

void Fog::CreateRootSignature()
{
	m_pRootSignature = new RootSignature();
	m_pRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
	m_pRootSignature->AddDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, D3D12_SHADER_VISIBILITY_PIXEL); // SRV
	m_pRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_PIXEL); // カメラ用定数バッファ
	m_pRootSignature->AddRootParameter(1, D3D12_SHADER_VISIBILITY_PIXEL); // ライトVP用定数バッファ
	m_pRootSignature->AddStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR); // スタティックサンプラー
	m_pRootSignature->Create();
}

void Fog::CreatePSO()
{
	m_pPSO = new PipelineState();
	assert(m_pPSO);	// nullptrチェック
	m_pPSO->SetInputLayout(Vertex::Sprite::InputLayout);
	m_pPSO->SetRootSignature(m_pRootSignature->Get());
	m_pPSO->SetVS(L"Assets/Shader/SpriteVS.cso");
	m_pPSO->SetPS(L"Assets/Shader/FogPS.cso");
	m_pPSO->SetDSVFormat(DXGI_FORMAT_UNKNOWN);
	m_pPSO->Create();
}
