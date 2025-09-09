/*+===================================================================
	File: PipelineState.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/03 13:39:26 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "PipelineState.hpp"
#include "../../System/Engine/Engine.hpp"
#include <d3dcompiler.h>

// ==============================
//	lib
// ==============================
#pragma comment(lib, "d3dcompiler.lib")

PipelineState::PipelineState()
{
	// ブレンドステートの設定
	D3D12_BLEND_DESC desc{};
	desc.AlphaToCoverageEnable = FALSE;											// アルファトゥカバレッジを無効
	desc.IndependentBlendEnable = FALSE;										// レンダーターゲットごとのブレンドステートを無効
	desc.RenderTarget[0].BlendEnable = TRUE;									// ブレンドを有効
	desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;						// ソースのブレンド係数
	desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;					// デストのブレンド係数
	desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;							// ブレンドの演算
	desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;						// ソースのアルファブレンド係数
	desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;						// デストのアルファブレンド係数
	desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;						// アルファブレンドの演算
	desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RGBA全てのチャンネルを描画

	// パイプラインステートの設定
	m_Desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);		// ラスタライザーステートの設定
	m_Desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;					// カリングしない
	m_Desc.BlendState = desc;												// ブレンドステートの設定
	m_Desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);	// 深度ステンシルステートの設定
	m_Desc.SampleMask = UINT_MAX;											// サンプルマスク
	m_Desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	// プリミティブトポロジーの設定（三角形リスト）
	m_Desc.NumRenderTargets = 1;											// レンダーターゲットの数
	m_Desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;						// レンダーターゲットのフォーマット
	m_Desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;								// 深度ステンシルビューのフォーマット
	m_Desc.SampleDesc.Count = 1;											// マルチサンプリングのサンプル数
	m_Desc.SampleDesc.Quality = 0;											// マルチサンプリングの品質
}

void PipelineState::SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout)
{
	m_Desc.InputLayout = layout;
}

void PipelineState::SetRootSignature(ID3D12RootSignature* pRootSignature)
{
	m_Desc.pRootSignature = pRootSignature;
}

void PipelineState::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type)
{
	m_Desc.PrimitiveTopologyType = type;
}

void PipelineState::SetVS(std::wstring filepath)
{
	HRESULT hr = D3DReadFileToBlob(filepath.c_str(), m_pVSBlob.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(nullptr, "頂点シェーダーの読み込みに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return; // エラー終了
	}
	m_Desc.VS = CD3DX12_SHADER_BYTECODE(m_pVSBlob.Get());
}

void PipelineState::SetPS(std::wstring filepath)
{
	HRESULT hr = D3DReadFileToBlob(filepath.c_str(), m_pPSBlob.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(nullptr, "ピクセルシェーダーの読み込みに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return; // エラー終了
	}
	m_Desc.PS = CD3DX12_SHADER_BYTECODE(m_pPSBlob.Get());
}

void PipelineState::Create()
{
	// パイプラインステートオブジェクトの生成
	HRESULT hr = Engine::GetInstance().GetDevice()->CreateGraphicsPipelineState(
		&m_Desc,													// グラフィックスパイプラインステートの記述
		IID_PPV_ARGS(m_pPipelineState.ReleaseAndGetAddressOf()));	// パイプラインステートオブジェクトのポインタ
	if (FAILED(hr))
	{
		MessageBox(nullptr, "パイプラインステートオブジェクトの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return; // エラー終了
	}
}
