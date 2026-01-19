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

ShadowMap::ShadowMap()
{
	float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	// レンダーターゲットの生成
	m_pRT = new RenderTarget();
	m_pRT->Create(WINDOW_WIDTH, WINDOW_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM, clearColor);

	// 深度ステンシルバッファの生成
	m_pDSV = new DepthStencil();
	m_pDSV->Create(WINDOW_WIDTH, WINDOW_HEIGHT, DXGI_FORMAT_D32_FLOAT);

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

	//// ライトの位置と向きを取得
	//DirectX::XMFLOAT3 pos = m_Owner->GetTransform().Position;
	//DirectX::XMFLOAT3 dir = GetDirection();

	//// ビュー行列の計算
	//DirectX::XMFLOAT4X4 ViewMat{};
	//DirectX::XMStoreFloat4x4(&ViewMat,
	//	DirectX::XMMatrixTranspose(
	//		DirectX::XMMatrixLookAtLH(
	//			DirectX::XMVectorSet(pos.x, pos.y, pos.z, 0.0f),							// ライト位置
	//			DirectX::XMVectorSet(pos.x + dir.x, pos.y + dir.y, pos.z + dir.z, 0.0f),	// ライトの注視点
	//			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f))));							// 上方向

	//// 射影行列の計算
	//DirectX::XMFLOAT4X4 projMat{};
	//DirectX::XMStoreFloat4x4(&projMat,
	//	DirectX::XMMatrixTranspose(
	//		DirectX::XMMatrixOrthographicLH(
	//			static_cast<float>(WINDOW_WIDTH),
	//			static_cast<float>(WINDOW_HEIGHT),
	//			0.1f, 1000.0f)));

	//// 定数バッファの生成
	//for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	//{
	//	m_pWVPCB[i] = new ConstantBuffer(sizeof(CB::WVP));
	//	assert(m_pWVPCB[i]);	// nullptrチェック

	//	CB::WVP* ptr = m_pWVPCB[i]->GetPtr<CB::WVP>();
	//	ptr->WorldMat = DirectX::SimpleMath::Matrix::Identity;
	//	ptr->ViewMat = ViewMat;
	//	ptr->ProjMat = projMat;
	//}

	// ルートシグネチャの生成
	m_pRootSignature = new RootSignature();
	m_pRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
	m_pRootSignature->Create();

	// パイプラインステートの生成
	m_pPSO = new PipelineState();
	m_pPSO->SetInputLayout(Vertex::Sprite::InputLayout);
	m_pPSO->SetRootSignature(m_pRootSignature->Get());
	m_pPSO->SetVS(L"Assets/Shader/SpriteVS.cso");
	m_pPSO->SetPS(L"Assets/Shader/SSAOPS.cso");
	m_pPSO->SetDSVFormat(DXGI_FORMAT_UNKNOWN);
	m_pPSO->Create();
}
