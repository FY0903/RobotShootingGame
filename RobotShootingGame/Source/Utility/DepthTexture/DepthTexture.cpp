/*+===================================================================
	File: DepthTexture.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/19 15:51:14 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "DepthTexture.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"
#include "Utility/CameraManager/CameraManager.hpp"

DepthTexture::DepthTexture()
{
	// 深度レンダーターゲットの生成
	m_pDepthRT = new RenderTarget();
	m_pDepthRT->Create(WINDOW_WIDTH, WINDOW_HEIGHT, DXGI_FORMAT_R32_FLOAT);

	// 深度ステンシルの生成
	m_pDepthStencil = new DepthStencil();
	m_pDepthStencil->Create(WINDOW_WIDTH, WINDOW_HEIGHT, DXGI_FORMAT_D32_FLOAT);

	float halfWidth = WINDOW_WIDTH / 2.0f;
	float halfHeight = WINDOW_HEIGHT / 2.0f;

	Vertex::Sprite vertices[4]{};
	vertices[0].Position = { -halfWidth, halfHeight, 0.0f };
	vertices[1].Position = { halfWidth, halfHeight, 0.0f };
	vertices[2].Position = { halfWidth, -halfHeight, 0.0f };
	vertices[3].Position = { -halfWidth, -halfHeight, 0.0f };

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

	// 定数バッファの生成
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_pWVPCB[i] = new ConstantBuffer(sizeof(CB::WVP));
		assert(m_pWVPCB[i]);	// nullptrチェック

		CB::WVP* ptr = m_pWVPCB[i]->GetPtr<CB::WVP>();
		ptr->WorldMat = DirectX::SimpleMath::Matrix::Identity;
		ptr->ViewMat = DirectX::SimpleMath::Matrix::Identity;
		ptr->ProjMat = DirectX::SimpleMath::Matrix::Identity;
	}

	// ルートシグネチャの生成
	m_pRootSignature = new RootSignature();
	assert(m_pRootSignature);	// nullptrチェック
	m_pRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
	m_pRootSignature->AddStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR); // スタティックサンプラー
	m_pRootSignature->Create();

	// パイプラインステートの生成
	m_pPipelineState = new PipelineState();
	assert(m_pPipelineState);	// nullptrチェック
	m_pPipelineState->SetInputLayout(Vertex::Sprite::InputLayout);
	m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
	m_pPipelineState->SetVS(L"Assets/Shader/SpriteVS.cso");
	m_pPipelineState->SetPS(L"Assets/Shader/DepthPS.cso");
	m_pPipelineState->SetRTVFormat(DXGI_FORMAT_R32_FLOAT, 0);
	m_pPipelineState->Create();
}

DepthTexture::~DepthTexture()
{
	delete m_pDepthRT;
	m_pDepthRT = nullptr;
	
	delete m_pDepthStencil;
	m_pDepthStencil = nullptr;
	
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		delete m_pWVPCB[i];
		m_pWVPCB[i] = nullptr;
	}
	
	delete m_pVertexBuffer;
	m_pVertexBuffer = nullptr;
	
	delete m_pIndexBuffer;
	m_pIndexBuffer = nullptr;
	
	delete m_pRootSignature;
	m_pRootSignature = nullptr;

	delete m_pPipelineState;
	m_pPipelineState = nullptr;
}

void DepthTexture::Draw()
{
	// レンダーターゲットの設定
	SetRenderTarget();

	// スプライトの描画
	DrawSprite();

	// GPUの待機
	WaitGPU();
}

void DepthTexture::SetRenderTarget()
{
	// リソースバリアの設定
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_pDepthRT->Resource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	Engine::GetInstance().GetCommandList()->ResourceBarrier(1, &barrier);

	// レンダーターゲットのハンドルを取得
	auto rtvHandle = m_pDepthRT->GetDescriptorHandle()->HandleCPU;

	// 深度ステンシルバッファのハンドルを取得
	auto dsvHandle = m_pDepthStencil->GetDescriptorHandle()->HandleCPU;

	// レンダーターゲットと深度ステンシルバッファを設定
	Engine::GetInstance().GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	// レンダーターゲットをクリア
	Engine::GetInstance().GetCommandList()->ClearRenderTargetView(
		rtvHandle,
		m_pDepthRT->GetClearValue().Color,
		0,
		nullptr);

	// 深度ステンシルバッファをクリア
	Engine::GetInstance().GetCommandList()->ClearDepthStencilView(
		dsvHandle,
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f,
		0,
		0,
		nullptr);
}

void DepthTexture::UpdateCB()
{
	// 定数バッファの更新
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得

	CB::WVP* ptr = m_pWVPCB[currentIndex]->GetPtr<CB::WVP>();
	ptr->WorldMat = DirectX::SimpleMath::Matrix::Identity;
	ptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->Get3DViewMatrixFloat4x4(false);
	ptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->Get3DProjectionMatrixFloat4x4(false);
}

void DepthTexture::DrawSprite()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得
	auto commandList = Engine::GetInstance().GetCommandList();				// コマンドリストを取得

	auto vbView = m_pVertexBuffer->GetView();	// 頂点バッファビューを取得
	auto ibView = m_pIndexBuffer->GetView();	// インデックスバッファビューを取得

	commandList->SetGraphicsRootSignature(m_pRootSignature->Get());			// ルートシグネチャを設定
	commandList->SetPipelineState(m_pPipelineState->Get());					// パイプラインステートを設定
	commandList->SetGraphicsRootConstantBufferView(0, m_pWVPCB[currentIndex]->GetAddress());	// 定数バッファを設定

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// プリミティブトポロジーを設定
	commandList->IASetVertexBuffers(0, 1, &vbView);								// 頂点バッファを設定
	commandList->IASetIndexBuffer(&ibView);										// インデックスバッファを設定

	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);	// 描画
}

void DepthTexture::WaitGPU()
{
	// リソースバリアの設定
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_pDepthRT->Resource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// リソースバリアの適用
	Engine::GetInstance().GetCommandList()->ResourceBarrier(1, &barrier);
}
