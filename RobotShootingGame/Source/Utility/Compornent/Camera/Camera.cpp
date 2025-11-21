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
	m_VP[0] = DirectX::XMMatrixLookAtLH(m_EyePos, m_TargetPos, m_UpVec);
	m_VP[1] = DirectX::XMMatrixPerspectiveFovLH(m_Fov, m_Aspect, 0.1f, 1000.0f);


	DirectX::XMFLOAT4X4 VPf[2]{};
	DirectX::XMStoreFloat4x4(&VPf[0], DirectX::XMMatrixTranspose(m_VP[0]));
	DirectX::XMStoreFloat4x4(&VPf[1], DirectX::XMMatrixTranspose(m_VP[1]));

	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_pCB[i] = new ConstantBuffer(sizeof(CB::VP));
		assert(m_pCB[i]);	// nullptrチェック
		CB::VP* ptr = m_pCB[i]->GetPtr<CB::VP>();

		ptr->ViewMat = VPf[0];
		ptr->ProjMat = VPf[1];
	}

	// ルートシグネチャの生成
	m_pRootSignature = new RootSignature();
	assert(m_pRootSignature);	// nullptrチェック
	m_pRootSignature->AddRootParameter(1, D3D12_SHADER_VISIBILITY_VERTEX);	// 定数バッファビュー
	m_pRootSignature->Create();
}

void Camera::Update()
{
	if (!m_pCB[0] || !m_pCB[1] || !m_pRootSignature)
	{
		assert(false && "メンバ変数が初期化されていません。初期化不備です。");
		throw std::runtime_error("Camera::Init() が呼ばれてません。必ず呼び出してください。");
	}

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

	// ビュー行列とプロジェクション行列の計算
	m_VP[0] = DirectX::XMMatrixLookAtLH(m_EyePos, m_TargetPos, m_UpVec);
	m_VP[1] = DirectX::XMMatrixPerspectiveFovLH(m_Fov, m_Aspect, 0.1f, 1000.0f);

	DirectX::XMFLOAT4X4 VPf[2]{};
	DirectX::XMStoreFloat4x4(&VPf[0], DirectX::XMMatrixTranspose(m_VP[0]));
	DirectX::XMStoreFloat4x4(&VPf[1], DirectX::XMMatrixTranspose(m_VP[1]));

	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();
	CB::VP* ptr = m_pCB[currentIndex]->GetPtr<CB::VP>();
	ptr->ViewMat = VPf[0];
	ptr->ProjMat = VPf[1];
}

void Camera::Draw()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得
	auto commandList = Engine::GetInstance().GetCommandList();				// コマンドリストを取得
	
	commandList->SetGraphicsRootSignature(m_pRootSignature->Get());			// ルートシグネチャを設定
	commandList->SetGraphicsRootConstantBufferView(0, m_pCB[currentIndex]->GetAddress());	// 定数バッファを設定
}

void Camera::Uninit()
{
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		if (m_pCB[i])
		{
			delete m_pCB[i];
			m_pCB[i] = nullptr;
		}
	}
	if (m_pRootSignature)
	{
		delete m_pRootSignature;
		m_pRootSignature = nullptr;
	}
}
