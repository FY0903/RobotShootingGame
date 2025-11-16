/*+===================================================================
	File: SpriteRenderer.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/14 18:31:18 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SpriteRenderer.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"

void SpriteRenderer::Init()
{
	Vertex::Sprite vertices[4]{};

	vertices[0].Position = { -0.5f, 0.5f, 0.0f };
	vertices[1].Position = { 0.5f, 0.5f, 0.0f };
	vertices[2].Position = { 0.5f, -0.5f, 0.0f };
	vertices[3].Position = { -0.5f, -0.5f, 0.0f };

	vertices[0].UV = { 0.0f, 0.0f };
	vertices[1].UV = { 1.0f, 0.0f };
	vertices[2].UV = { 1.0f, 1.0f };
	vertices[3].UV = { 0.0f, 1.0f };

	// 頂点バッファの生成
	auto vertexSize = std::size(vertices) * sizeof(Vertex::Sprite);
	auto vertexStride = sizeof(Vertex::Sprite);
	m_pVertexBuffer = new VertexBuffer(vertexSize, vertexStride, vertices);
	assert(m_pVertexBuffer);	// nullptrチェック

	uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };

	// インデックスバッファの生成
	auto indexSize = std::size(indices) * sizeof(uint32_t);
	m_pIndexBuffer = new IndexBuffer(indexSize, indices);
	assert(m_pIndexBuffer);	// nullptrチェック

	// ディスクリプタヒープの生成
	m_pDescriptorHeap = new DescriptorHeap();
	assert(m_pDescriptorHeap);	// nullptrチェック

	// ディスクリプタハンドルの生成
	m_pMaterialHandle = m_pDescriptorHeap->Register(m_pTexture->Resource(), m_pTexture->ViewDesc());
	assert(m_pMaterialHandle);	// nullptrチェック

	// 定数バッファの生成
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_pConstantBuffer[i] = new ConstantBuffer(sizeof(Transform));
		assert(m_pConstantBuffer[i]);	// nullptrチェック

		Transform* ptr = m_pConstantBuffer[i]->GetPtr<Transform>();
		ptr->World = DirectX::XMMatrixIdentity();
		ptr->View = m_Camera.GetViewMatrix();
		ptr->Proj = m_Camera.GetProjectionMatrix();
	}

	// ルートシグネチャの生成
	m_pRootSignature = new RootSignature();
	assert(m_pRootSignature);	// nullptrチェック
	m_pRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
	m_pRootSignature->AddDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, D3D12_SHADER_VISIBILITY_PIXEL); // テクスチャ
	m_pRootSignature->AddStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR); // スタティックサンプラー
	m_pRootSignature->Create();

	// パイプラインステートの生成
	m_pPipelineState = new PipelineState();
	assert(m_pPipelineState);	// nullptrチェック
	m_pPipelineState->SetInputLayout(Vertex::Sprite::InputLayout);
	m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
	m_pPipelineState->SetVS(L"Assets/Shader/SpriteVS.cso");
	m_pPipelineState->SetPS(L"Assets/Shader/SpritePS.cso");
	m_pPipelineState->Create();
}

void SpriteRenderer::Update()
{
}

void SpriteRenderer::Draw()
{
}

void SpriteRenderer::Uninit()
{
}
