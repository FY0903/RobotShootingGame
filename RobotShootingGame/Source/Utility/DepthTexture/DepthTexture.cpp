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
#include "Utility/MaterialManager/MaterialManager.hpp"
#include "Utility/MaterialBase/MaterialBase.hpp"

DepthTexture::DepthTexture()
{
	float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	// 深度レンダーターゲットの生成
	m_pDepthRT = new RenderTarget();
	m_pDepthRT->Create(WINDOW_WIDTH, WINDOW_HEIGHT, DXGI_FORMAT_R32_FLOAT, clearColor);

	// 深度ステンシルの生成
	m_pDepthStencil = new DepthStencil();
	m_pDepthStencil->Create(WINDOW_WIDTH, WINDOW_HEIGHT, DXGI_FORMAT_D32_FLOAT);

	// ルートシグネチャの生成
	CreateRootSignature();

	// パイプラインステートの生成
	CreatePSO();

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

	// 深度テクスチャのSRV登録
	m_SRVHandles.push_back(m_pDescriptorHeap->Register(m_pDepthRT->Resource(), m_pDepthRT->ViewDesc()));

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
		m_pWVPCB[i] = new ConstantBuffer(sizeof(CB::WVP));
		assert(m_pWVPCB[i]);	// nullptrチェック

		CB::WVP* ptr = m_pWVPCB[i]->GetPtr<CB::WVP>();
		ptr->WorldMat = DirectX::SimpleMath::Matrix::Identity;
		ptr->ViewMat = DirectX::SimpleMath::Matrix::Identity;
		ptr->ProjMat = projMat;
	}

	// ルートシグネチャの生成
	m_pRootSig = new RootSignature();
	assert(m_pRootSig);	// nullptrチェック
	m_pRootSig->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
	m_pRootSig->AddDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, D3D12_SHADER_VISIBILITY_PIXEL);	// テクスチャ
	m_pRootSig->AddStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR); // スタティックサンプラー
	m_pRootSig->Create();

	// パイプラインステートの生成
	m_pPipelineState = new PipelineState();
	assert(m_pPipelineState);	// nullptrチェック
	m_pPipelineState->SetInputLayout(Vertex::Sprite::InputLayout);
	m_pPipelineState->SetRootSignature(m_pRootSig->Get());
	m_pPipelineState->SetVS(L"Assets/Shader/SpriteVS.cso");
	m_pPipelineState->SetPS(L"Assets/Shader/SpritePS.cso");
	m_pPipelineState->SetDSVFormat(DXGI_FORMAT_UNKNOWN);
	m_pPipelineState->Create();
}

DepthTexture::~DepthTexture()
{
	delete m_pDepthRT;
	m_pDepthRT = nullptr;
	
	delete m_pDepthStencil;
	m_pDepthStencil = nullptr;

	delete m_pRootSignature;
	m_pRootSignature = nullptr;

	for (size_t i = 0; i < NumPSO; ++i)
	{
		delete m_pPSOs[i];
		m_pPSOs[i] = nullptr;
	}

	m_RenderItems.clear();

	delete m_pVertexBuffer;
	m_pVertexBuffer = nullptr;

	delete m_pIndexBuffer;
	m_pIndexBuffer = nullptr;

	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		delete m_pWVPCB[i];
		m_pWVPCB[i] = nullptr;
	}

	delete m_pDescriptorHeap;
	m_pDescriptorHeap = nullptr;

	delete m_pRootSig;
	m_pRootSig = nullptr;

	delete m_pPipelineState;
	m_pPipelineState = nullptr;
}

void DepthTexture::Draw()
{
	// レンダーアイテムがなければ終了
	if (m_RenderItems.empty()) return;

	// レンダーターゲットの設定
	SetRenderTarget();

	// レンダーアイテムの描画
	DrawRenderItems();

	// GPUの待機
	WaitGPU();
}

void DepthTexture::DrawDebugSprite()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得
	auto commandList = Engine::GetInstance().GetCommandList();				// コマンドリストを取得
	auto materialHeap = m_pDescriptorHeap->GetHeap();						// ディスクリプタヒープを取得

	auto vbView = m_pVertexBuffer->GetView();	// 頂点バッファビューを取得
	auto ibView = m_pIndexBuffer->GetView();	// インデックスバッファビューを取得

	commandList->SetGraphicsRootSignature(m_pRootSig->Get());			// ルートシグネチャを設定
	commandList->SetPipelineState(m_pPipelineState->Get());					// パイプラインステートを設定
	commandList->SetGraphicsRootConstantBufferView(0, m_pWVPCB[currentIndex]->GetAddress());	// 定数バッファを設定

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// プリミティブトポロジーを設定
	commandList->IASetVertexBuffers(0, 1, &vbView);								// 頂点バッファを設定
	commandList->IASetIndexBuffer(&ibView);										// インデックスバッファを設定

	commandList->SetDescriptorHeaps(1, &materialHeap);							// ディスクリプタヒープを設定
	commandList->SetGraphicsRootDescriptorTable(1, m_SRVHandles[0]->HandleGPU);	// ディスクリプタテーブルを設定

	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);	// 描画
}

void DepthTexture::CreateRootSignature()
{
	m_pRootSignature = new RootSignature();
	assert(m_pRootSignature);	// nullptrチェック
	m_pRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
	m_pRootSignature->Create();
}

void DepthTexture::CreatePSO()
{
	// メッシュ用PSOの生成
	m_pPSOs[Mesh] = new PipelineState();
	assert(m_pPSOs[Mesh]);	// nullptrチェック
	m_pPSOs[Mesh]->SetInputLayout(Vertex::Mesh::InputLayout);
	m_pPSOs[Mesh]->SetRootSignature(m_pRootSignature->Get());
	m_pPSOs[Mesh]->SetVS(L"Assets/Shader/DepthMeshVS.cso");
	m_pPSOs[Mesh]->SetPS(L"Assets/Shader/DepthMeshPS.cso");
	m_pPSOs[Mesh]->SetRTVFormat(DXGI_FORMAT_R32_FLOAT, 0);
	m_pPSOs[Mesh]->Create();

	// スプライト用PSOの生成
	m_pPSOs[Sprite] = new PipelineState();
	assert(m_pPSOs[Sprite]);	// nullptrチェック
	m_pPSOs[Sprite]->SetInputLayout(Vertex::Sprite::InputLayout);
	m_pPSOs[Sprite]->SetRootSignature(m_pRootSignature->Get());
	m_pPSOs[Sprite]->SetVS(L"Assets/Shader/DepthSpriteVS.cso");
	m_pPSOs[Sprite]->SetPS(L"Assets/Shader/DepthSpritePS.cso");
	m_pPSOs[Sprite]->SetRTVFormat(DXGI_FORMAT_R32G32B32A32_FLOAT, 0);
	m_pPSOs[Sprite]->Create();
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

void DepthTexture::DrawRenderItems()
{
	const auto cmd = Engine::GetInstance().GetCommandList();						// コマンドリストを取得
	const auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得

	for (auto& item : m_RenderItems)
	{
		if (!item.pMaterial) continue;

		// ルートシグネチャをセット
		cmd->SetGraphicsRootSignature(m_pRootSignature->Get());

		// パイプラインステートをセット
		MaterialBase::InputLayoutType layoutType = item.pMaterial->GetInputLayoutType();
		switch (layoutType)
		{
		case MaterialBase::Sprite:
			cmd->SetPipelineState(m_pPSOs[Sprite]->Get());
			break;
		case MaterialBase::Mesh:
			cmd->SetPipelineState(m_pPSOs[Mesh]->Get());
			break;
		default:
			assert(false); // 未対応の入力レイアウトタイプ
			break;
		}

		// 定数バッファをセット
		cmd->SetGraphicsRootConstantBufferView(0, item.pMaterial->GetCBByRegisterForFrame(0, currentIndex)->GetAddress());

		for (size_t i = 0; i < item.MeshSize; ++i)
		{
			if (!item.pVertexBuffers[i]) continue;

			// 頂点バッファとインデックスバッファをセット
			auto vbView = item.pVertexBuffers[i]->GetView();
			cmd->IASetVertexBuffers(0, 1, &vbView);
			if (item.pIndexBuffers.size())
			{
				auto ibView = item.pIndexBuffers[i]->GetView();
				cmd->IASetIndexBuffer(&ibView);
			}
			cmd->IASetPrimitiveTopology(item.type);

			if (item.pIndexBuffers.size())
				cmd->DrawIndexedInstanced(item.indexCounts[i], 1, 0, 0, 0);
			else
				cmd->DrawInstanced(item.indexCounts[i], 1, 0, 0);
		}
	}
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
