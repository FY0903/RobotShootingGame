/*+===================================================================
	File: DepthTexture.cpp
	Summary: DepthTextureクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/19 15:51 初回作成
			26/01/15 18:23 コメント記載
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
	m_pVB = new VertexBuffer(vertexSize, vertexStride, vertices);
	assert(m_pVB);	// nullptrチェック

	uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };

	// インデックスバッファの生成
	auto indexSize = std::size(indices) * sizeof(uint32_t);
	m_pIB = new IndexBuffer(indexSize, indices);
	assert(m_pIB);	// nullptrチェック

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
	m_pRootSignature = new RootSignature();
	assert(m_pRootSignature);	// nullptrチェック
	m_pRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
	m_pRootSignature->AddDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, D3D12_SHADER_VISIBILITY_PIXEL);	// テクスチャ
	m_pRootSignature->AddStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR); // スタティックサンプラー
	m_pRootSignature->Create();

	// パイプラインステートの生成
	m_pPSO = new PipelineState();
	assert(m_pPSO);	// nullptrチェック
	m_pPSO->SetInputLayout(Vertex::Sprite::InputLayout);
	m_pPSO->SetRootSignature(m_pRootSignature->Get());
	m_pPSO->SetVS(L"Assets/Shader/SpriteVS.cso");
	m_pPSO->SetPS(L"Assets/Shader/SpritePS.cso");
	m_pPSO->SetDSVFormat(DXGI_FORMAT_UNKNOWN);
	m_pPSO->Create();
}

DepthTexture::~DepthTexture()
{
	delete m_pDepthRT;
	m_pDepthRT = nullptr;
	
	delete m_pDepthStencil;
	m_pDepthStencil = nullptr;

	delete m_pDSVRootSignature;
	m_pDSVRootSignature = nullptr;

	delete m_pSkeletalRootSignature;
	m_pSkeletalRootSignature = nullptr;

	for (size_t i = 0; i < Num; ++i)
	{
		delete m_pPSOs[i];
		m_pPSOs[i] = nullptr;
	}

	m_RenderItems.clear();

	delete m_pVB;
	m_pVB = nullptr;

	delete m_pIB;
	m_pIB = nullptr;

	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		delete m_pWVPCB[i];
		m_pWVPCB[i] = nullptr;
	}

	delete m_pDescriptorHeap;
	m_pDescriptorHeap = nullptr;

	delete m_pRootSignature;
	m_pRootSignature = nullptr;

	delete m_pPSO;
	m_pPSO = nullptr;
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

	auto vbView = m_pVB->GetView();	// 頂点バッファビューを取得
	auto ibView = m_pIB->GetView();	// インデックスバッファビューを取得

	commandList->SetGraphicsRootSignature(m_pRootSignature->Get());			// ルートシグネチャを設定
	commandList->SetPipelineState(m_pPSO->Get());					// パイプラインステートを設定
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
	m_pDSVRootSignature = new RootSignature();
	assert(m_pDSVRootSignature);	// nullptrチェック
	m_pDSVRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
	m_pDSVRootSignature->Create();

	m_pSkeletalRootSignature = new RootSignature();
	assert(m_pSkeletalRootSignature);	// nullptrチェック
	m_pSkeletalRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
	m_pSkeletalRootSignature->AddRootParameter(1, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ(スキニング用)
	m_pSkeletalRootSignature->Create();
}

void DepthTexture::CreatePSO()
{
	// スケルタルメッシュ用PSOの生成
	m_pPSOs[SkeletalMesh] = new PipelineState();
	assert(m_pPSOs[SkeletalMesh]);	// nullptrチェック
	m_pPSOs[SkeletalMesh]->SetInputLayout(Vertex::SkeletalMesh::InputLayout);
	m_pPSOs[SkeletalMesh]->SetRootSignature(m_pSkeletalRootSignature->Get());
	m_pPSOs[SkeletalMesh]->SetVS(L"Assets/Shader/DepthSkeletalMeshVS.cso");
	m_pPSOs[SkeletalMesh]->SetPS(L"Assets/Shader/DepthSkeletalMeshPS.cso");
	m_pPSOs[SkeletalMesh]->SetRTVFormat(DXGI_FORMAT_R32_FLOAT, 0);
	m_pPSOs[SkeletalMesh]->Create();

	// メッシュ用PSOの生成
	m_pPSOs[Mesh] = new PipelineState();
	assert(m_pPSOs[Mesh]);	// nullptrチェック
	m_pPSOs[Mesh]->SetInputLayout(Vertex::Mesh::InputLayout);
	m_pPSOs[Mesh]->SetRootSignature(m_pDSVRootSignature->Get());
	m_pPSOs[Mesh]->SetVS(L"Assets/Shader/DepthMeshVS.cso");
	m_pPSOs[Mesh]->SetPS(L"Assets/Shader/DepthMeshPS.cso");
	m_pPSOs[Mesh]->SetRTVFormat(DXGI_FORMAT_R32_FLOAT, 0);
	m_pPSOs[Mesh]->Create();

	// スプライト用PSOの生成
	m_pPSOs[Sprite] = new PipelineState();
	assert(m_pPSOs[Sprite]);	// nullptrチェック
	m_pPSOs[Sprite]->SetInputLayout(Vertex::Sprite::InputLayout);
	m_pPSOs[Sprite]->SetRootSignature(m_pDSVRootSignature->Get());
	m_pPSOs[Sprite]->SetVS(L"Assets/Shader/DepthSpriteVS.cso");
	m_pPSOs[Sprite]->SetPS(L"Assets/Shader/DepthSpritePS.cso");
	m_pPSOs[Sprite]->SetRTVFormat(DXGI_FORMAT_R32_FLOAT, 0);
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

		// ルートシグネチャとパイプラインステートをセット
		MaterialBase::InputLayoutType layoutType = item.pMaterial->GetInputLayoutType();
		switch (layoutType)
		{
		case MaterialBase::SkeletalMesh:
			cmd->SetGraphicsRootSignature(m_pSkeletalRootSignature->Get());
			cmd->SetPipelineState(m_pPSOs[SkeletalMesh]->Get());
			break;
		case MaterialBase::Mesh:
			cmd->SetGraphicsRootSignature(m_pDSVRootSignature->Get());
			cmd->SetPipelineState(m_pPSOs[Mesh]->Get());
			break;
		case MaterialBase::Sprite:
			cmd->SetGraphicsRootSignature(m_pDSVRootSignature->Get());
			cmd->SetPipelineState(m_pPSOs[Sprite]->Get());
			break;
		default:
			assert(false); // 未対応の入力レイアウトタイプ
			break;
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

			if (item.pIBs.size())
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
