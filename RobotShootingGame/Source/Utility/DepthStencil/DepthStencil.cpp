/*+===================================================================
	File: DepthStencil.cpp
	Summary: DepthStencilクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/11 13:37 初回作成
			26/01/15 18:20 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "DepthStencil.hpp"
#include "System/Engine/Engine.hpp"

DepthStencil::~DepthStencil()
{
	delete m_pSnapshotDescriptorHeap;
	m_pSnapshotDescriptorHeap = nullptr;
}

HRESULT DepthStencil::Create(UINT width, UINT height, DXGI_FORMAT format, float clearDepth, UINT8 clearStencil)
{
	// リソースの設定
	CD3DX12_HEAP_PROPERTIES prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC(
		D3D12_RESOURCE_DIMENSION_TEXTURE2D,		// リソースの設定
		0,										// アライメント
		width,									// 幅
		height,									// 高さ
		1,										// 配列サイズ
		1,										// ミップ数
		format,									// フォーマット
		1,										// サンプルカウント
		0,										// サンプルクオリティ
		D3D12_TEXTURE_LAYOUT_UNKNOWN,			// テクスチャのレイアウト
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE	// リソースのフラグ
	);

	// 最適化されたクリア値の設定
	m_ClearValue.Format = format;
	m_ClearValue.DepthStencil.Depth = clearDepth;
	m_ClearValue.DepthStencil.Stencil = clearStencil;

	// リソースを生成
	HRESULT hr = Engine::GetInstance().GetDevice()->CreateCommittedResource(
		&prop,										// ヒープのプロパティ
		D3D12_HEAP_FLAG_NONE,						// ヒープのフラグ
		&desc,										// リソースの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,			// リソースの初期状態
		&m_ClearValue,								// 最適化されたクリア値
		IID_PPV_ARGS(m_pResource.ReleaseAndGetAddressOf())); // リソースのポインタ

	if (FAILED(hr))
	{
		MessageBox(nullptr, "深度ステンシルバッファの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return hr;
	}

	// 幅と高さを保存
	m_Width = width;
	m_Height = height;

	m_pSnapshotDescriptorHeap = new DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	m_pDescriptorHandle = m_pSnapshotDescriptorHeap->Register(m_pResource.Get());
	if (!m_pDescriptorHandle)
	{
		MessageBox(nullptr, "深度ステンシルバッファのディスクリプタハンドルの登録に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	return hr;
}
