/*+===================================================================
	File: ShadowMap.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2026/01/19 3:25:58 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "ShadowMap.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"
#include "Utility/MaterialManager/MaterialManager.hpp"
#include "Utility/MaterialBase/MaterialBase.hpp"

ShadowMap::ShadowMap(Light* pLight, Quality quality)
	: m_pLight(pLight)
{
	float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	// 深度レンダーターゲットの生成
	m_pRT = new RenderTarget();
	m_pRT->Create(quality, quality, DXGI_FORMAT_R8G8B8A8_UNORM, clearColor);

	// 深度ステンシルの生成
	m_pDSV = new DepthStencil();
	m_pDSV->Create(quality, quality, DXGI_FORMAT_D32_FLOAT);

	// 定数バッファの生成
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_pWVPCBs[i] = new ConstantBuffer(sizeof(CB::WVP));
		assert(m_pWVPCBs[i]);	// nullptrチェック

		// ライトのビュー行列とプロジェクション行列を設定
		m_pWVPCBs[i]->GetPtr<CB::WVP>()->ViewMat = m_pLight->GetViewMatrixFloat4x4(false);
		m_pWVPCBs[i]->GetPtr<CB::WVP>()->ProjMat = m_pLight->GetProjectionMatrixFloat4x4(false);
	}

	// ビューポートの設定
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.Width = static_cast<float>(quality);
	m_viewport.Height = static_cast<float>(quality);
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	// シザー矩形の設定
	m_scissor.left = 0;
	m_scissor.top = 0;
	m_scissor.right = static_cast<LONG>(quality);
	m_scissor.bottom = static_cast<LONG>(quality);

	// ルートシグネチャの生成
	CreateRootSignature();

	// パイプラインステートの生成
	CreatePSO();
}

ShadowMap::~ShadowMap()
{
	m_pLight = nullptr;

	delete m_pRT;
	m_pRT = nullptr;

	delete m_pDSV;
	m_pDSV = nullptr;

	for (size_t i = 0; i < Num; ++i)
	{
		delete m_pRootsignatures[i];
		m_pRootsignatures[i] = nullptr;

		delete m_pPSOs[i];
		m_pPSOs[i] = nullptr;
	}

	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		delete m_pWVPCBs[i];
		m_pWVPCBs[i] = nullptr;
	}
}

void ShadowMap::Draw()
{
	// レンダーアイテムがなければ終了
	if (m_RenderItems.empty()) return;

	const auto cmd = Engine::GetInstance().GetCommandList();	// コマンドリストを取得

	// ビューポートとシザー矩形の設定
	cmd->RSSetViewports(1, &m_viewport); // ビューポートの設定
	cmd->RSSetScissorRects(1, &m_scissor); // シザー矩形の設定

	// レンダーターゲットの設定
	SetRenderTarget();

	// レンダーアイテムの描画
	DrawRenderItems();

	// GPUの待機
	WaitGPU();

	// ビューポートとシザー矩形をデフォルト設定に戻す
	Engine::GetInstance().SetDefaultViewPortAndScissor();
}

void ShadowMap::CreateRootSignature()
{
	m_pRootsignatures[SkeletalMesh] = new RootSignature();
	assert(m_pRootsignatures[SkeletalMesh]);	// nullptrチェック
	m_pRootsignatures[SkeletalMesh]->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
	m_pRootsignatures[SkeletalMesh]->AddRootParameter(1, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ(スキニング用)
	m_pRootsignatures[SkeletalMesh]->Create();

	m_pRootsignatures[Mesh] = new RootSignature();
	assert(m_pRootsignatures[Mesh]);	// nullptrチェック
	m_pRootsignatures[Mesh]->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
	m_pRootsignatures[Mesh]->Create();

	m_pRootsignatures[Sprite] = new RootSignature();
	assert(m_pRootsignatures[Sprite]);	// nullptrチェック
	m_pRootsignatures[Sprite]->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
	m_pRootsignatures[Sprite]->Create();
}

void ShadowMap::CreatePSO()
{
	// スケルタルメッシュ用PSOの生成
	m_pPSOs[SkeletalMesh] = new PipelineState();
	assert(m_pPSOs[SkeletalMesh]);	// nullptrチェック
	m_pPSOs[SkeletalMesh]->SetInputLayout(Vertex::SkeletalMesh::InputLayout);
	m_pPSOs[SkeletalMesh]->SetRootSignature(m_pRootsignatures[SkeletalMesh]->Get());
	m_pPSOs[SkeletalMesh]->SetVS(L"Assets/Shader/ShadowSkeletalMeshVS.cso");
	m_pPSOs[SkeletalMesh]->SetPS(L"Assets/Shader/ShadowPS.cso");
	m_pPSOs[SkeletalMesh]->Create();

	// メッシュ用PSOの生成
	m_pPSOs[Mesh] = new PipelineState();
	assert(m_pPSOs[Mesh]);	// nullptrチェック
	m_pPSOs[Mesh]->SetInputLayout(Vertex::Mesh::InputLayout);
	m_pPSOs[Mesh]->SetRootSignature(m_pRootsignatures[Mesh]->Get());
	m_pPSOs[Mesh]->SetVS(L"Assets/Shader/ShadowMeshVS.cso");
	m_pPSOs[Mesh]->SetPS(L"Assets/Shader/ShadowPS.cso");
	m_pPSOs[Mesh]->Create();

	// スプライト用PSOの生成
	m_pPSOs[Sprite] = new PipelineState();
	assert(m_pPSOs[Sprite]);	// nullptrチェック
	m_pPSOs[Sprite]->SetInputLayout(Vertex::Sprite::InputLayout);
	m_pPSOs[Sprite]->SetRootSignature(m_pRootsignatures[Sprite]->Get());
	m_pPSOs[Sprite]->SetVS(L"Assets/Shader/ShadowSpriteVS.cso");
	m_pPSOs[Sprite]->SetPS(L"Assets/Shader/ShadowPS.cso");
	m_pPSOs[Sprite]->Create();
}

void ShadowMap::SetRenderTarget()
{
	// リソースバリアの設定
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_pRT->Resource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	Engine::GetInstance().GetCommandList()->ResourceBarrier(1, &barrier);

	// レンダーターゲットのハンドルを取得
	auto rtvHandle = m_pRT->GetDescriptorHandle()->HandleCPU;

	// 深度ステンシルバッファのハンドルを取得
	auto dsvHandle = m_pDSV->GetDescriptorHandle()->HandleCPU;

	// レンダーターゲットと深度ステンシルバッファを設定
	Engine::GetInstance().GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	// レンダーターゲットをクリア
	Engine::GetInstance().GetCommandList()->ClearRenderTargetView(
		rtvHandle,
		m_pRT->GetClearValue().Color,
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

void ShadowMap::UpdateLightVP()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();

	CB::WVP* wvpPtr = m_pWVPCBs[currentIndex]->GetPtr<CB::WVP>();
	wvpPtr->ViewMat = DirectX::SimpleMath::Matrix(); // ビュー行列を単位行列に設定
	wvpPtr->ProjMat = m_pLight->GetProjectionMatrixFloat4x4(false);	// ライトのプロジェクション行列を設定
}

void ShadowMap::DrawRenderItems()
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
			cmd->SetGraphicsRootSignature(m_pRootsignatures[SkeletalMesh]->Get());
			cmd->SetPipelineState(m_pPSOs[SkeletalMesh]->Get());
			break;
		case MaterialBase::Mesh:
			cmd->SetGraphicsRootSignature(m_pRootsignatures[Mesh]->Get());
			cmd->SetPipelineState(m_pPSOs[Mesh]->Get());
			break;
		case MaterialBase::Sprite:
			cmd->SetGraphicsRootSignature(m_pRootsignatures[Sprite]->Get());
			cmd->SetPipelineState(m_pPSOs[Sprite]->Get());
			break;
		default:
			assert(false); // 未対応の入力レイアウトタイプ
			break;
		}

		// WVP行列の更新
		CB::WVP* matPtr = item.pMaterial->GetCBByRegisterForFrame(0, currentIndex)->GetPtr<CB::WVP>();
		CB::WVP* wvpPtr = m_pWVPCBs[currentIndex]->GetPtr<CB::WVP>();
		wvpPtr->WorldMat = matPtr->WorldMat;	// ワールド行列はそのままコピー

		// 定数バッファをセット
		cmd->SetGraphicsRootConstantBufferView(0, m_pWVPCBs[currentIndex]->GetAddress());	// ライトのWVP行列用定数バッファ
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

void ShadowMap::WaitGPU()
{
	// リソースバリアの設定
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_pRT->Resource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// リソースバリアの適用
	Engine::GetInstance().GetCommandList()->ResourceBarrier(1, &barrier);
}
