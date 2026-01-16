/*+===================================================================
	File: RenderTarget.cpp
	Summary: RenderTargetクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/03 15:07 初回作成
			26/01/16 11:08 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "RenderTarget.hpp"
#include "System/Engine/Engine.hpp"

RenderTarget::~RenderTarget()
{
	delete m_pSnapshotDescriptorHeap;
	m_pSnapshotDescriptorHeap = nullptr;
}

HRESULT RenderTarget::Create(UINT width, UINT height, DXGI_FORMAT format, float clearColor[4])
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
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET	// リソースのフラグ
	);

	// 最適化されたクリア値の設定
	m_ClearValue.Format = format;
	if (clearColor)
	{
		m_ClearValue.Color[0] = clearColor[0];
		m_ClearValue.Color[1] = clearColor[1];
		m_ClearValue.Color[2] = clearColor[2];
		m_ClearValue.Color[3] = clearColor[3];
	}
	else
	{
		m_ClearValue.Color[0] = 0.0f;
		m_ClearValue.Color[1] = 0.0f;
		m_ClearValue.Color[2] = 0.0f;
		m_ClearValue.Color[3] = 1.0f;
	}

	// リソースを生成
	HRESULT hr = Engine::GetInstance().GetDevice()->CreateCommittedResource(
		&prop,										// ヒープのプロパティ
		D3D12_HEAP_FLAG_NONE,						// ヒープのフラグ
		&desc,										// リソースの設定
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,	// リソースの初期状態
		&m_ClearValue,								// 最適化されたクリア値
		IID_PPV_ARGS(m_pResource.ReleaseAndGetAddressOf())); // リソースのポインタ

	if (FAILED(hr))
	{
		MessageBox(nullptr, "レンダーターゲットの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return hr;
	}

	// 幅と高さを保存
	m_Width = width;
	m_Height = height;

	m_pSnapshotDescriptorHeap = new DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	m_pDescriptorHandle = m_pSnapshotDescriptorHeap->Register(m_pResource.Get());
	if (!m_pDescriptorHandle)
	{
		MessageBox(nullptr, "レンダーターゲットのディスクリプタハンドルの登録に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// シェーダーリソースビューの設定
	m_ViewDesc.Format = m_pResource->GetDesc().Format; // フォーマット
	m_ViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; // コンポーネントのマッピング
	m_ViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // テクスチャ2D
	m_ViewDesc.Texture2D.MipLevels = 1; // ミップ数

	return hr;
}
