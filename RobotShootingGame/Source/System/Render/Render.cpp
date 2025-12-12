/*+===================================================================
	File: Render.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/09 19:24:38 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Render.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"
#include "Utility/CameraManager/CameraManager.hpp"

Render::Render()
{
	for (auto& rtv : m_GbufferRT)
	{
		rtv = new RenderTarget();
	}
	m_pDepthStencil = new DepthStencil();

	clearColor[0] = 0.0f;
	clearColor[1] = 0.0f;
	clearColor[2] = 0.0f;
	clearColor[3] = 1.0f;

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

	// ディスクリプタヒープの生成
	m_pDescriptorHeap = new DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	assert(m_pDescriptorHeap);	// nullptrチェック

	DirectX::XMFLOAT4X4 projMat{};

	DirectX::XMStoreFloat4x4(&projMat,
		DirectX::XMMatrixTranspose(
			DirectX::XMMatrixOrthographicLH(
				static_cast<float>(WINDOW_WIDTH),
				static_cast<float>(WINDOW_HEIGHT),
				0.1f, 1000.0f)));

	// 定数バッファの生成
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_pCB[i] = new ConstantBuffer(sizeof(CB::WVP));
		assert(m_pCB[i]);	// nullptrチェック

		CB::WVP* ptr = m_pCB[i]->GetPtr<CB::WVP>();
		ptr->WorldMat = DirectX::SimpleMath::Matrix::Identity;
		ptr->ViewMat = DirectX::SimpleMath::Matrix::Identity;
		ptr->ProjMat = projMat;
	}

	// ルートシグネチャの生成
	m_pRootSignature = new RootSignature();
	assert(m_pRootSignature);	// nullptrチェック
	m_pRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
	m_pRootSignature->AddDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, D3D12_SHADER_VISIBILITY_PIXEL);	// RTV用テクスチャ
	m_pRootSignature->AddStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR); // スタティックサンプラー
	m_pRootSignature->Create();

	// パイプラインステートの生成
	m_pPipelineState = new PipelineState();
	assert(m_pPipelineState);	// nullptrチェック
	m_pPipelineState->SetInputLayout(Vertex::Sprite::InputLayout);
	m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
	m_pPipelineState->SetVS(L"Assets/Shader/RenderTargetVS.cso");
	m_pPipelineState->SetPS(L"Assets/Shader/RenderTargetPS.cso");
	m_pPipelineState->SetDSVFormat(DXGI_FORMAT_UNKNOWN);
	m_pPipelineState->Create();
}

Render::~Render()
{
	for (auto& rtv : m_GbufferRT)
	{
		delete rtv;
		rtv = nullptr;
	}

	delete m_pDepthStencil;
	m_pDepthStencil = nullptr;

	for (auto& cb : m_pCB)
	{
		delete cb;
		cb = nullptr;
	}

	delete m_pVertexBuffer;
	m_pVertexBuffer = nullptr;

	delete m_pIndexBuffer;
	m_pIndexBuffer = nullptr;

	delete m_pDescriptorHeap;
	m_pDescriptorHeap = nullptr;

	delete m_pRootSignature;
	m_pRootSignature = nullptr;

	delete m_pPipelineState;
	m_pPipelineState = nullptr;
}

void Render::Init()
{
	// Gバッファ用レンダーターゲットの生成
	// アルベド
	m_GbufferRT[Albedo]->Create(
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		clearColor);

	// 法線
	m_GbufferRT[Normal]->Create(
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		clearColor);

	// ワールド座標
	m_GbufferRT[WorldPos]->Create(
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		clearColor);

	// 深度ステンシルバッファの生成
	m_pDepthStencil->Create(
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		DXGI_FORMAT_D32_FLOAT);

	// ディスクリプタヒープにSRVを登録
	for (size_t i = 0; i < NumGbufferRT; ++i)
	{
		// Gバッファ用レンダーターゲットのSRVをディスクリプタヒープに登録
		auto srvHandle = m_pDescriptorHeap->Register(
			m_GbufferRT[i]->Resource(),
			m_GbufferRT[i]->ViewDesc());
		assert(srvHandle);	// nullptrチェック
		m_SRVHandles.push_back(srvHandle);
	}
}

void Render::BeginDraw()
{
	// Gバッファ用レンダーターゲットをレンダーターゲットとして設定
	CD3DX12_RESOURCE_BARRIER barriers[NumGbufferRT]{};
	for (size_t i = 0; i < NumGbufferRT; ++i)
	{
		barriers[i] = CD3DX12_RESOURCE_BARRIER::Transition(
			m_GbufferRT[i]->Resource(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	Engine::GetInstance().GetCommandList()->ResourceBarrier(NumGbufferRT, barriers);

	// レンダーターゲットのハンドルを配列に格納
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[NumGbufferRT]{};
	for (size_t i = 0; i < NumGbufferRT; ++i)
	{
		auto handlePtr = m_GbufferRT[i]->GetDescriptorHandle();
		rtvHandles[i] = handlePtr->HandleCPU;
	}

	// 深度ステンシルバッファのハンドルを取得
	auto dsvHandle = m_pDepthStencil->GetDescriptorHandle()->HandleCPU;

	// レンダーターゲットと深度ステンシルバッファを設定
	Engine::GetInstance().GetCommandList()->OMSetRenderTargets(NumGbufferRT, rtvHandles, FALSE, &dsvHandle);

	// レンダーターゲットをクリア
	for (size_t i = 0; i < NumGbufferRT; ++i)
	{
		Engine::GetInstance().GetCommandList()->ClearRenderTargetView(
			rtvHandles[i],
			m_GbufferRT[i]->GetClearValue().Color,
			0,
			nullptr);
	}

	// 深度ステンシルバッファをクリア
	Engine::GetInstance().GetCommandList()->ClearDepthStencilView(
		dsvHandle,
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f,
		0,
		0,
		nullptr);
}

void Render::EndDraw()
{
	// Gバッファ用レンダーターゲットをピクセルシェーダーリソースに戻す
	CD3DX12_RESOURCE_BARRIER barriers[NumGbufferRT]{};
	for (size_t i = 0; i < NumGbufferRT; ++i)
	{
		barriers[i] = CD3DX12_RESOURCE_BARRIER::Transition(
			m_GbufferRT[i]->Resource(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}
	Engine::GetInstance().GetCommandList()->ResourceBarrier(NumGbufferRT, barriers);
}

void Render::DrawBackBuffer()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得
	auto commandList = Engine::GetInstance().GetCommandList();				// コマンドリストを取得
	auto materialHeap = m_pDescriptorHeap->GetHeap();						// ディスクリプタヒープを取得

	auto vbView = m_pVertexBuffer->GetView();	// 頂点バッファビューを取得
	auto ibView = m_pIndexBuffer->GetView();	// インデックスバッファビューを取得

	commandList->SetGraphicsRootSignature(m_pRootSignature->Get());			// ルートシグネチャを設定
	commandList->SetPipelineState(m_pPipelineState->Get());					// パイプラインステートを設定
	commandList->SetGraphicsRootConstantBufferView(0, m_pCB[currentIndex]->GetAddress());	// 定数バッファを設定

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// プリミティブトポロジーを設定
	commandList->IASetVertexBuffers(0, 1, &vbView);								// 頂点バッファを設定
	commandList->IASetIndexBuffer(&ibView);										// インデックスバッファを設定

	commandList->SetDescriptorHeaps(1, &materialHeap);							// ディスクリプタヒープを設定
	commandList->SetGraphicsRootDescriptorTable(1, m_SRVHandles[0]->HandleGPU);	// ディスクリプタテーブルを設定
	commandList->SetGraphicsRootDescriptorTable(1, m_SRVHandles[1]->HandleGPU);	// ディスクリプタテーブルを設定
	commandList->SetGraphicsRootDescriptorTable(1, m_SRVHandles[2]->HandleGPU);	// ディスクリプタテーブルを設定

	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);	// 描画
}
