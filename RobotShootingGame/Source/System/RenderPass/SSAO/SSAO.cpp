/*+===================================================================
	File: SSAO.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2026/01/16 20:41:06 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SSAO.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"

void SSAO::Init()
{
	// 定数バッファの生成
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_pCB[i] = new ConstantBuffer(sizeof(CB::SSAOKernel));
		assert(m_pCB[i]);	// nullptrチェック
	}

	// カーネルの生成と定数バッファへの転送
	{
		DirectX::XMFLOAT3 kernel[64]{};

		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_real_distribution<float> distNeg1to1(-1.0f, 1.0f);
		std::uniform_real_distribution<float> dist0to1(0.0f, 1.0f);

		for (size_t i = 0; i < 64; ++i)
		{
			DirectX::XMVECTOR vec{};
			vec = DirectX::XMVectorSet(
				distNeg1to1(rng),
				distNeg1to1(rng),
				dist0to1(rng),
				0.0f);
			vec = DirectX::XMVector3Normalize(vec);
			vec = DirectX::XMVectorScale(vec, dist0to1(rng));

			float scale = static_cast<float>(i) / 64.0f;
			scale = 0.1f + scale * scale * (1.0f - 0.1f);
			vec = DirectX::XMVectorScale(vec, scale);

			DirectX::XMStoreFloat3(&kernel[i], vec);
		}

		for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
		{
			auto cbPtr = m_pCB[i]->GetPtr<CB::SSAOKernel>();
			std::memcpy(cbPtr, kernel, sizeof(kernel));
		}
	}
}

void SSAO::SetDepthSRV(RenderTarget* rt)
{
	DescriptorHandle* handle = m_pDescriptorHeap->Register(rt->Resource(), rt->ViewDesc());
	m_SRVHandles.push_back(handle);
}

void SSAO::CreateRootSignature()
{
	m_pRootSignature = new RootSignature();
	m_pRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
	m_pRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_PIXEL);  // 定数バッファ
	m_pRootSignature->AddRootParameter(1, D3D12_SHADER_VISIBILITY_PIXEL);  // 定数バッファ
	m_pRootSignature->AddDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, D3D12_SHADER_VISIBILITY_PIXEL); // SRV
	m_pRootSignature->AddStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR); // スタティックサンプラー
	m_pRootSignature->Create();
}

void SSAO::CreatePSO()
{
	m_pPSO = new PipelineState();
	m_pPSO->SetInputLayout(Vertex::Sprite::InputLayout);
	m_pPSO->SetRootSignature(m_pRootSignature->Get());
	m_pPSO->SetVS(L"Assets/Shader/SpriteVS.cso");
	m_pPSO->SetPS(L"Assets/Shader/SSAOPS.cso");
	m_pPSO->SetDSVFormat(DXGI_FORMAT_UNKNOWN);
	m_pPSO->Create();
}
