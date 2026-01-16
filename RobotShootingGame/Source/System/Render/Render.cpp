/*+===================================================================
	File: Render.cpp
	Summary: 描画処理を管理するクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/09 19:24 初回作成
			26/01/15 09:30 コメント記載
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

#include "System/RenderPass/Sepia/Sepia.hpp"

Render::Render()
{
	for (auto& rtv : m_GbufferRT)
	{
		rtv = new RenderTarget();
	}
	m_pSnapshotRT = new RenderTarget();
	m_pDepthStencil = new DepthStencil();
	m_pDepthTexture = new DepthTexture();

	clearColor[0] = 0.0f;
	clearColor[1] = 0.0f;
	clearColor[2] = 0.0f;
	clearColor[3] = 0.0f;

	// スナップショット用レンダーターゲットの生成
	m_pSnapshotRT->Create(
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		clearColor);

	// 深度ステンシルバッファの生成
	m_pDepthStencil->Create(
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		DXGI_FORMAT_D32_FLOAT);

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
	m_pVB = new VertexBuffer(vertexSize, vertexStride, vertices);
	assert(m_pVB);	// nullptrチェック

	uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };

	// インデックスバッファの生成
	auto indexSize = std::size(indices) * sizeof(uint32_t);
	m_pIB = new IndexBuffer(indexSize, indices);
	assert(m_pIB);	// nullptrチェック

	// ディスクリプタヒープの生成
	m_pSnapshotDescriptorHeap = new DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	assert(m_pSnapshotDescriptorHeap);	// nullptrチェック

	m_pFrameBufferDescriptorHeap = new DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	assert(m_pFrameBufferDescriptorHeap);	// nullptrチェック

	// 深度テクスチャのSRV登録
	auto DepthSRV = m_pDepthTexture->GetRenderTarget();
	m_SnapshotRVHandles.push_back(m_pSnapshotDescriptorHeap->Register(DepthSRV->Resource(), DepthSRV->ViewDesc()));

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
		m_pCameraCB[i] = new ConstantBuffer(sizeof(CB::Camera));
		assert(m_pCameraCB[i]);	// nullptrチェック
		m_pSSAOKernelCB[i] = new ConstantBuffer(sizeof(CB::SSAOKernel));
		assert(m_pSSAOKernelCB[i]);	// nullptrチェック

		CB::WVP* ptr = m_pWVPCB[i]->GetPtr<CB::WVP>();
		ptr->WorldMat = DirectX::SimpleMath::Matrix::Identity;
		ptr->ViewMat = DirectX::SimpleMath::Matrix::Identity;
		ptr->ProjMat = projMat;
	}

	// SSAOカーネルの生成と定数バッファへの転送
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
			auto cbPtr = m_pSSAOKernelCB[i]->GetPtr<CB::SSAOKernel>();
			std::memcpy(cbPtr, kernel, sizeof(kernel));
		}
	}
	
	// ルートシグネチャの生成
	m_pSnapshotRootSignature = new RootSignature();
	assert(m_pSnapshotRootSignature);	// nullptrチェック
	m_pSnapshotRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
	m_pSnapshotRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_PIXEL); // 定数バッファ
	m_pSnapshotRootSignature->AddRootParameter(1, D3D12_SHADER_VISIBILITY_PIXEL); // 定数バッファ
	m_pSnapshotRootSignature->AddRootParameter(2, D3D12_SHADER_VISIBILITY_PIXEL); // 定数バッファ
	m_pSnapshotRootSignature->AddDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, D3D12_SHADER_VISIBILITY_PIXEL);	// テクスチャ
	m_pSnapshotRootSignature->AddStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR); // スタティックサンプラー
	m_pSnapshotRootSignature->Create();

	m_pFrameBufferRootSignature = new RootSignature();
	assert(m_pFrameBufferRootSignature);	// nullptrチェック
	m_pFrameBufferRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
	m_pFrameBufferRootSignature->AddDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, D3D12_SHADER_VISIBILITY_PIXEL);	// テクスチャ
	m_pFrameBufferRootSignature->AddStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR); // スタティックサンプラー
	m_pFrameBufferRootSignature->Create();

	// パイプラインステートの生成
	m_pSnapshotPSO = new PipelineState();
	assert(m_pSnapshotPSO);	// nullptrチェック
	m_pSnapshotPSO->SetInputLayout(Vertex::Sprite::InputLayout);
	m_pSnapshotPSO->SetRootSignature(m_pSnapshotRootSignature->Get());
	m_pSnapshotPSO->SetVS(L"Assets/Shader/RenderTargetVS.cso");
	m_pSnapshotPSO->SetPS(L"Assets/Shader/RenderTargetPS.cso");
	m_pSnapshotPSO->SetDSVFormat(DXGI_FORMAT_UNKNOWN);
	m_pSnapshotPSO->Create();

	m_pFrameBufferPSO = new PipelineState();
	assert(m_pFrameBufferPSO);	// nullptrチェック
	m_pFrameBufferPSO->SetInputLayout(Vertex::Sprite::InputLayout);
	m_pFrameBufferPSO->SetRootSignature(m_pFrameBufferRootSignature->Get());
	m_pFrameBufferPSO->SetVS(L"Assets/Shader/SpriteVS.cso");
	m_pFrameBufferPSO->SetPS(L"Assets/Shader/SpritePS.cso");
	m_pFrameBufferPSO->SetDSVFormat(DXGI_FORMAT_UNKNOWN);
	m_pFrameBufferPSO->Create();

	// レンダーパスの初期化
	InitRenderPasses();

	if (m_RenderPasses.size())
	{
		// 最後のレンダーターゲットをフレームバッファ用ディスクリプタヒープに登録
		auto rt = m_RenderPasses.back()->GetRenderTarget();
		m_FrameBufferRVHandles.push_back(
			m_pFrameBufferDescriptorHeap->Register(rt->Resource(), rt->ViewDesc()));
	}
	else
	{
		// スナップショット用レンダーターゲットをフレームバッファ用ディスクリプタヒープに登録
		m_FrameBufferRVHandles.push_back(
			m_pFrameBufferDescriptorHeap->Register(m_pSnapshotRT->Resource(), m_pSnapshotRT->ViewDesc()));
	}
}

Render::~Render()
{
	delete m_pDepthTexture;
	m_pDepthTexture = nullptr;

	delete m_pSnapshotRT;
	m_pSnapshotRT = nullptr;

	for (auto& rtv : m_GbufferRT)
	{
		delete rtv;
		rtv = nullptr;
	}

	for (auto& pass : m_RenderPasses)
	{
		delete pass;
		pass = nullptr;
	}

	delete m_pDepthStencil;
	m_pDepthStencil = nullptr;

	for (auto& cb : m_pWVPCB)
	{
		delete cb;
		cb = nullptr;
	}

	for (auto& cb : m_pLightCB)
	{
		delete cb;
		cb = nullptr;
	}

	for (auto& cb : m_pCameraCB)
	{
		delete cb;
		cb = nullptr;
	}

	for (auto& cb : m_pSSAOKernelCB)
	{
		delete cb;
		cb = nullptr;
	}

	delete m_pVB;
	m_pVB = nullptr;

	delete m_pIB;
	m_pIB = nullptr;

	delete m_pSnapshotDescriptorHeap;
	m_pSnapshotDescriptorHeap = nullptr;

	delete m_pSnapshotRootSignature;
	m_pSnapshotRootSignature = nullptr;

	delete m_pSnapshotPSO;
	m_pSnapshotPSO = nullptr;

	delete m_pFrameBufferDescriptorHeap;
	m_pFrameBufferDescriptorHeap = nullptr;

	delete m_pFrameBufferRootSignature;
	m_pFrameBufferRootSignature = nullptr;

	delete m_pFrameBufferPSO;
	m_pFrameBufferPSO = nullptr;
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

	// ディスクリプタヒープにSRVを登録
	for (size_t i = 0; i < Num; ++i)
	{
		// Gバッファ用レンダーターゲットのSRVをディスクリプタヒープに登録
		auto srvHandle = m_pSnapshotDescriptorHeap->Register(
			m_GbufferRT[i]->Resource(),
			m_GbufferRT[i]->ViewDesc());
		assert(srvHandle);	// nullptrチェック
		m_SnapshotRVHandles.push_back(srvHandle);
	}
}

void Render::InitRenderPasses()
{
	//AddRenderPass<Sepia>();
}

void Render::SetGbufferRenderTargets()
{
	// Gバッファ用レンダーターゲットをレンダーターゲットとして設定
	CD3DX12_RESOURCE_BARRIER barriers[Num]{};
	for (size_t i = 0; i < Num; ++i)
	{
		barriers[i] = CD3DX12_RESOURCE_BARRIER::Transition(
			m_GbufferRT[i]->Resource(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	Engine::GetInstance().GetCommandList()->ResourceBarrier(Num, barriers);

	// レンダーターゲットのハンドルを配列に格納
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[Num]{};
	for (size_t i = 0; i < Num; ++i)
	{
		auto handlePtr = m_GbufferRT[i]->GetDescriptorHandle();
		rtvHandles[i] = handlePtr->HandleCPU;
	}

	// 深度ステンシルバッファのハンドルを取得
	auto dsvHandle = m_pDepthStencil->GetDescriptorHandle()->HandleCPU;

	// レンダーターゲットと深度ステンシルバッファを設定
	Engine::GetInstance().GetCommandList()->OMSetRenderTargets(Num, rtvHandles, FALSE, &dsvHandle);

	// レンダーターゲットをクリア
	for (size_t i = 0; i < Num; ++i)
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

void Render::SetSnapshotRenderTarget()
{
	// バックバッファ用レンダーターゲットをレンダーターゲットとして設定
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_pSnapshotRT->Resource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	// リソースバリアの適用
	Engine::GetInstance().GetCommandList()->ResourceBarrier(1, &barrier);

	// レンダーターゲットと深度ステンシルバッファを設定
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pSnapshotRT->GetDescriptorHandle()->HandleCPU;
	auto dsvHandle = m_pDepthStencil->GetDescriptorHandle()->HandleCPU;

	// レンダーターゲットと深度ステンシルバッファを設定
	Engine::GetInstance().GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	// レンダーターゲットをクリア
	Engine::GetInstance().GetCommandList()->ClearRenderTargetView(
		rtvHandle,
		m_pSnapshotRT->GetClearValue().Color,
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

	//m_pDepthTexture->DrawDebugSprite();

	// 不透明レンダーアイテムの描画
	DrawRenderItems(m_OpaqueRenderItems);

	// Gバッファ用レンダーターゲットをピクセルシェーダーリソースに戻す
	CD3DX12_RESOURCE_BARRIER barriers[Num]{};
	for (size_t i = 0; i < Num; ++i)
	{
		barriers[i] = CD3DX12_RESOURCE_BARRIER::Transition(
			m_GbufferRT[i]->Resource(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}
	Engine::GetInstance().GetCommandList()->ResourceBarrier(Num, barriers);
}

void Render::DrawTransparent()
{
	// 不透明板ポリゴンの描画
	DrawOpaqueSprite();

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

void Render::DrawPostProcess()
{
	// レンダーパスの実行
	for (auto& pass : m_RenderPasses)
	{
		pass->Execute();
	}
}

void Render::WaitGPU()
{
	// リソースバリアの設定
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_pSnapshotRT->Resource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// リソースバリアの適用
	Engine::GetInstance().GetCommandList()->ResourceBarrier(1, &barrier);
}

void Render::Draw()
{
	// 不透明レンダーアイテムの描画
	DrawOpaque();

	// スナップショット用レンダーターゲットをセット
	SetSnapshotRenderTarget();

	// 透明レンダーアイテムの描画
	DrawTransparent();

	// ポストプロセスの描画
	DrawPostProcess();

	// GPUの待機
	WaitGPU();
}

void Render::CopyBackBufferToFrameBuffer()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得
	auto commandList = Engine::GetInstance().GetCommandList();				// コマンドリストを取得
	auto materialHeap = m_pFrameBufferDescriptorHeap->GetHeap();			// ディスクリプタヒープを取得

	auto vbView = m_pVB->GetView();	// 頂点バッファビューを取得
	auto ibView = m_pIB->GetView();	// インデックスバッファビューを取得

	commandList->SetGraphicsRootSignature(m_pFrameBufferRootSignature->Get());			// ルートシグネチャを設定
	commandList->SetPipelineState(m_pFrameBufferPSO->Get());							// パイプラインステートを設定
	commandList->SetGraphicsRootConstantBufferView(0, m_pWVPCB[currentIndex]->GetAddress());	// 定数バッファを設定

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// プリミティブトポロジーを設定
	commandList->IASetVertexBuffers(0, 1, &vbView);								// 頂点バッファを設定
	commandList->IASetIndexBuffer(&ibView);										// インデックスバッファを設定

	commandList->SetDescriptorHeaps(1, &materialHeap);							// ディスクリプタヒープを設定
	commandList->SetGraphicsRootDescriptorTable(1, m_FrameBufferRVHandles[0]->HandleGPU);	// ディスクリプタテーブルを設定

	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);	// 描画
}

void Render::DrawOpaqueSprite()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得
	auto commandList = Engine::GetInstance().GetCommandList();				// コマンドリストを取得
	auto materialHeap = m_pSnapshotDescriptorHeap->GetHeap();				// ディスクリプタヒープを取得
	auto lights = LightManager::GetInstance().GetLights();					// ライト情報を取得

	// ライト情報を定数バッファに転送
	if (lights.size())
	{
		CB::Light* lightPtr = m_pLightCB[currentIndex]->GetPtr<CB::Light>();
		lightPtr->Position = lights[0]->GetPosition();
		lightPtr->Range = lights[0]->GetRange();
		lightPtr->Direction = lights[0]->GetDirection();
		lightPtr->Angle = lights[0]->GetAngle();
		lightPtr->Color = lights[0]->GetColor();
	}

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
	
	auto vbView = m_pVB->GetView();	// 頂点バッファビューを取得
	auto ibView = m_pIB->GetView();	// インデックスバッファビューを取得

	commandList->SetGraphicsRootSignature(m_pSnapshotRootSignature->Get());			// ルートシグネチャを設定
	commandList->SetPipelineState(m_pSnapshotPSO->Get());					// パイプラインステートを設定
	commandList->SetGraphicsRootConstantBufferView(0, m_pWVPCB[currentIndex]->GetAddress());	// 定数バッファを設定
	commandList->SetGraphicsRootConstantBufferView(1, m_pLightCB[currentIndex]->GetAddress());	// 定数バッファを設定
	commandList->SetGraphicsRootConstantBufferView(2, m_pCameraCB[currentIndex]->GetAddress()); // カメラ用定数バッファを設定
	commandList->SetGraphicsRootConstantBufferView(3, m_pSSAOKernelCB[currentIndex]->GetAddress()); // SSAOカーネル用定数バッファを設定

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// プリミティブトポロジーを設定
	commandList->IASetVertexBuffers(0, 1, &vbView);								// 頂点バッファを設定
	commandList->IASetIndexBuffer(&ibView);										// インデックスバッファを設定

	commandList->SetDescriptorHeaps(1, &materialHeap);							// ディスクリプタヒープを設定
	commandList->SetGraphicsRootDescriptorTable(4, m_SnapshotRVHandles[0]->HandleGPU);	// ディスクリプタテーブルを設定

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

		for (size_t i = 0; i < item.meshSize; ++i)
		{
			if (!item.pVBs[i]) continue;

			// 頂点バッファとインデックスバッファをセット
			auto vbView = item.pVBs[i]->GetView();
			cmd->IASetVertexBuffers(0, 1, &vbView);
			if (item.pIBs.size())
			{
				auto ibView = item.pIBs[i]->GetView();
				cmd->IASetIndexBuffer(&ibView);
			}
			cmd->IASetPrimitiveTopology(item.type);

			if (descriptorHeap)
			{
				auto srvHandle = item.pMaterial->GetDescriptorHandle(i);
				if (srvHandle)
					cmd->SetGraphicsRootDescriptorTable(item.pMaterial->GetRootParameterIndex() - 1, srvHandle->HandleGPU);
			}

			if (item.pIBs.size())
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
