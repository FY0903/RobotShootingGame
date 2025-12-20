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

DepthTexture::DepthTexture()
{
	// 深度レンダーターゲットの生成
	m_pDepthRT = new RenderTarget();
	m_pDepthRT->Create(WINDOW_WIDTH, WINDOW_HEIGHT, DXGI_FORMAT_R32_FLOAT);

	// 深度ステンシルの生成
	m_pDepthStencil = new DepthStencil();
	m_pDepthStencil->Create(WINDOW_WIDTH, WINDOW_HEIGHT, DXGI_FORMAT_D32_FLOAT);

	// ルートシグネチャの生成
	CreateRootSignature();

	// パイプラインステートの生成
	CreatePSO();
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
}

void DepthTexture::Draw()
{
	// レンダーアイテムがなければ終了
	if (m_RenderItems.empty()) return;

	// レンダーターゲットの設定
	SetRenderTarget();

	// GPUの待機
	WaitGPU();
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

		auto pso = item.pMaterial->GetPipelineState();
		if (!m_pRootSignature || !pso) continue;
		cmd->SetGraphicsRootSignature(m_pRootSignature->Get());
		cmd->SetPipelineState(pso->Get());

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
