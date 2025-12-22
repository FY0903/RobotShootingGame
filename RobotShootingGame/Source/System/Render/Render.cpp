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
#include "Utility/MaterialManager/MaterialManager.hpp"
#include "Utility/LightManager/LightManager.hpp"
#include "Utility/Compornent/Light/Light.hpp"
#include "Utility/DepthTexture/DepthTexture.hpp"

Render::Render()
{
	for (auto& rtv : m_GbufferRT)
	{
		rtv = new RenderTarget();
	}
	m_pDepthStencil = new DepthStencil();
	m_pDepthTexture = new DepthTexture();

	clearColor[0] = 0.0f;
	clearColor[1] = 0.0f;
	clearColor[2] = 0.0f;
	clearColor[3] = 0.0f;

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
	auto DepthSRV = m_pDepthTexture->GetRenderTarget();
	m_SRVHandles.push_back(m_pDescriptorHeap->Register(DepthSRV->Resource(), DepthSRV->ViewDesc()));

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
		m_pLightCB[i] = new ConstantBuffer(sizeof(CB::Light));
		assert(m_pLightCB[i]);	// nullptrチェック

		CB::WVP* ptr = m_pWVPCB[i]->GetPtr<CB::WVP>();
		ptr->WorldMat = DirectX::SimpleMath::Matrix::Identity;
		ptr->ViewMat = DirectX::SimpleMath::Matrix::Identity;
		ptr->ProjMat = projMat;
	}

	// ルートシグネチャの生成
	m_pRootSignature = new RootSignature();
	assert(m_pRootSignature);	// nullptrチェック
	m_pRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
	m_pRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_PIXEL); // 定数バッファ
	m_pRootSignature->AddDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, D3D12_SHADER_VISIBILITY_PIXEL);	// テクスチャ
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
	delete m_pDepthTexture;
	m_pDepthTexture = nullptr;

	for (auto& rtv : m_GbufferRT)
	{
		delete rtv;
		rtv = nullptr;
	}

	delete m_pDepthStencil;
	m_pDepthStencil = nullptr;

	for (auto& cb : m_pWVPCB)
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

void Render::SetGbufferRenderTargets()
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

void Render::DrawOpaque()
{
	// 深度テクスチャに不透明レンダーアイテムをセット
	m_pDepthTexture->SetRenderItems(m_OpaqueRenderItems);

	// 深度テクスチャの描画
	m_pDepthTexture->Draw();

	// Gバッファ用レンダーターゲットをセット
	SetGbufferRenderTargets();

	// 不透明レンダーアイテムの描画
	DrawRenderItems(m_OpaqueRenderItems);

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

void Render::DrawTransparent()
{
	//m_pDepthTexture->DrawDebugSprite();

	//return;

	// 不透明板ポリゴンの描画
	DrawBackBuffer();

	std::stable_sort(
		m_TransparentRenderItems.begin(),
		m_TransparentRenderItems.end(),
		[](const RenderItem& a, const RenderItem& b)
		{
			return a.sortKey > b.sortKey; // カメラから遠い順にソート
	});

	// 透明レンダーアイテムの描画
	DrawRenderItems(m_TransparentRenderItems);

	ResetRenderItems();
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
		DXGI_FORMAT_R32G32B32A32_FLOAT,
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

void Render::DrawBackBuffer()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得
	auto commandList = Engine::GetInstance().GetCommandList();				// コマンドリストを取得
	auto materialHeap = m_pDescriptorHeap->GetHeap();						// ディスクリプタヒープを取得
	auto lights = LightManager::GetInstance().GetLights();					// ライト情報を取得

	// ライト情報を定数バッファに転送
	if (lights.size())
	{
		CB::Light* lightPtr = m_pLightCB[currentIndex]->GetPtr<CB::Light>();
		lightPtr->Direction = lights[0]->GetDirection();
		lightPtr->Color = lights[0]->GetColor();
	}

	auto vbView = m_pVertexBuffer->GetView();	// 頂点バッファビューを取得
	auto ibView = m_pIndexBuffer->GetView();	// インデックスバッファビューを取得

	commandList->SetGraphicsRootSignature(m_pRootSignature->Get());			// ルートシグネチャを設定
	commandList->SetPipelineState(m_pPipelineState->Get());					// パイプラインステートを設定
	commandList->SetGraphicsRootConstantBufferView(0, m_pWVPCB[currentIndex]->GetAddress());	// 定数バッファを設定
	commandList->SetGraphicsRootConstantBufferView(1, m_pLightCB[currentIndex]->GetAddress());	// 定数バッファを設定

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// プリミティブトポロジーを設定
	commandList->IASetVertexBuffers(0, 1, &vbView);								// 頂点バッファを設定
	commandList->IASetIndexBuffer(&ibView);										// インデックスバッファを設定

	commandList->SetDescriptorHeaps(1, &materialHeap);							// ディスクリプタヒープを設定
	commandList->SetGraphicsRootDescriptorTable(2, m_SRVHandles[0]->HandleGPU);	// ディスクリプタテーブルを設定

	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);	// 描画
}

void Render::ResetRenderItems()
{
	m_OpaqueRenderItems.clear();
	m_TransparentRenderItems.clear();
}

void Render::DrawRenderItems(const std::vector<RenderItem>& renderItems)
{
	// コマンドリストを取得
	const auto cmd = Engine::GetInstance().GetCommandList();
	const auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();
	if (!cmd) return;

	for (auto& item : renderItems)
	{
		if (!item.pMaterial) continue;

		// マテリアルのルートシグネチャとパイプラインステートをセット
		auto rootSig = item.pMaterial->GetRootSignature();
		auto pso = item.pMaterial->GetPipelineState();
		if (!rootSig || !pso) continue;
		cmd->SetGraphicsRootSignature(rootSig->Get());
		cmd->SetPipelineState(pso->Get());

		// マテリアルのディスクリプタヒープをセット
		auto descriptorHeap = item.pMaterial->GetDescriptorHeap();
		if (descriptorHeap)
		{
			auto descriptorHeapPtr = descriptorHeap->GetHeap();
			cmd->SetDescriptorHeaps(1, &descriptorHeapPtr);
		}

		// 定数バッファをセット
		cmd->SetGraphicsRootConstantBufferView(0, item.pMaterial->GetCBByRegisterForFrame(0, currentIndex)->GetAddress());
		for (size_t i = 1; i < item.pMaterial->GetCBSize(); ++i)
		{
			if (i > item.pMaterial->GetRootParameterIndex()) assert(false); // RootParameterIndexを超過している

			cmd->SetGraphicsRootConstantBufferView(static_cast<UINT>(i), item.pMaterial->GetCBByRegisterForFrame(static_cast<int>(i), currentIndex)->GetAddress());
		}

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

			if (descriptorHeap)
			{
				auto srvHandle = item.pMaterial->GetDescriptorHandle(i);
				if (srvHandle)
					cmd->SetGraphicsRootDescriptorTable(item.pMaterial->GetRootParameterIndex() - 1, srvHandle->HandleGPU);
			}

			if (item.pIndexBuffers.size())
				cmd->DrawIndexedInstanced(item.indexCounts[i], 1, 0, 0, 0);
			else
				cmd->DrawInstanced(item.indexCounts[i], 1, 0, 0);
		}
	}
}

void Render::EnqueueRenderItem(const RenderItem& item)
{
	// 不透明か透明かでレンダーアイテムを振り分ける
	item.pMaterial->IsOpaque()
		? m_OpaqueRenderItems.push_back(item)
		: m_TransparentRenderItems.push_back(item);
}
