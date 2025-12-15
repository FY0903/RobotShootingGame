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
#include "System/Engine/Engine.hpp"
#include <d3dcompiler.h>

// ==============================
//	lib
// ==============================
#pragma comment(lib, "d3dcompiler.lib")

PipelineState::PipelineState(size_t renderTargetNum, bool alphaBlend)
{
	// ブレンドステートの設定
	D3D12_BLEND_DESC desc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);	// デフォルトのブレンドステートを取得

	// アルファブレンドを有効にする場合
	if (alphaBlend)
	{
		desc.RenderTarget[0].BlendEnable = TRUE;									// ブレンドを有効にする
		desc.RenderTarget[0].LogicOpEnable = FALSE;									// 論理演算を無効にする
		desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;						// ソースのブレンドファクター
		desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;					// デスティネーションのブレンドファクター
		desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;							// ブレンドオペレーション
		desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;						// アルファのソースのブレンドファクター
		desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;						// アルファのデスティネーションのブレンドファクター
		desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;						// アルファのブレンドオペレーション
		desc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;							// 論理演算オペレーション
		desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// 書き込みマスク
	}

	// パイプラインステートの設定
	m_Desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);		// ラスタライザーステートの設定
	m_Desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;					// カリングしない
	m_Desc.BlendState = desc;												// ブレンドステートの設定
	m_Desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);	// 深度ステンシルステートの設定
	m_Desc.SampleMask = UINT_MAX;											// サンプルマスク
	m_Desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	// プリミティブトポロジーの設定（三角形リスト）
	m_Desc.NumRenderTargets = static_cast<UINT>(renderTargetNum);			// レンダーターゲットの数
	m_Desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;								// 深度ステンシルビューのフォーマット
	m_Desc.SampleDesc.Count = 1;											// マルチサンプリングのサンプル数
	m_Desc.SampleDesc.Quality = 0;
	for (size_t i = 0; i < renderTargetNum; ++i)
	{
		m_Desc.RTVFormats[i] = DXGI_FORMAT_R8G8B8A8_UNORM;					// レンダーターゲットビューのフォーマット
	}
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

void PipelineState::SetDSVFormat(DXGI_FORMAT format)
{
	m_Desc.DSVFormat = format;
	if (format == DXGI_FORMAT_UNKNOWN)
	{
		m_Desc.DepthStencilState.DepthEnable = FALSE; // 深度テストを無効化
	}
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
