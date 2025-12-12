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
#include "Utility/CameraManager/CameraManager.hpp"
#include "Game/Actor/Actor.hpp"

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

	// 定数バッファの生成
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_pCB[i] = new ConstantBuffer(sizeof(CB::WVP));
		assert(m_pCB[i]);	// nullptrチェック
		CB::WVP* ptr = m_pCB[i]->GetPtr<CB::WVP>();
		ptr->WorldMat = m_Owner->GetTransform().GetWorldMatrixFloat4x4(false);
		ptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->Get3DViewMatrixFloat4x4(false);
		ptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->Get3DProjectionMatrixFloat4x4(false);
	}
}

void SpriteRenderer::Update()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();
	CB::WVP* ptr = m_pCB[currentIndex]->GetPtr<CB::WVP>();

	ptr->WorldMat = m_Owner->GetTransform().GetWorldMatrixFloat4x4(false);
	ptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->Get3DViewMatrixFloat4x4(false);
	ptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->Get3DProjectionMatrixFloat4x4(false);
}

void SpriteRenderer::Draw()
{
	auto material = m_Owner->GetMaterial();

	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();		// 現在のバックバッファのインデックスを取得
	auto commandList = Engine::GetInstance().GetCommandList();					// コマンドリストを取得
	auto materialHeap = material->GetDescriptorHeap();							// ディスクリプタヒープを取得
	auto heap = materialHeap->GetHeap();

	auto vbView = m_pVertexBuffer->GetView();	// 頂点バッファビューを取得
	auto ibView = m_pIndexBuffer->GetView();	// インデックスバッファビューを取得

	commandList->SetGraphicsRootSignature(material->GetRootSignature()->Get());			// ルートシグネチャを設定
	commandList->SetPipelineState(material->GetPipelineState()->Get());					// パイプラインステートを設定
	commandList->SetGraphicsRootConstantBufferView(0, m_pCB[currentIndex]->GetAddress()); // 定数バッファを設定

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// プリミティブトポロジーを設定
	commandList->IASetVertexBuffers(0, 1, &vbView);								// 頂点バッファを設定
	commandList->IASetIndexBuffer(&ibView);										// インデックスバッファを設定

	commandList->SetDescriptorHeaps(1, &heap);														// ディスクリプタヒープを設定
	commandList->SetGraphicsRootDescriptorTable(1, material->GetDescriptorHandle(0)->HandleGPU);	// ディスクリプタテーブルを設定

	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);	// 描画
}

void SpriteRenderer::Uninit()
{
	if (m_pVertexBuffer)
	{
		delete m_pVertexBuffer;
		m_pVertexBuffer = nullptr;
	}

	if (m_pIndexBuffer)
	{
		delete m_pIndexBuffer;
		m_pIndexBuffer = nullptr;
	}

	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		if (m_pCB[i])
		{
			delete m_pCB[i];
			m_pCB[i] = nullptr;
		}
	}
}
