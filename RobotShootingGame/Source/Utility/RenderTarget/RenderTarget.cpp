/*+===================================================================
	File: RenderTarget.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/03 15:07:34 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "RenderTarget.hpp"
#include "System/Engine/Engine.hpp"

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
	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = format;
	if (clearColor)
	{
		clearValue.Color[0] = clearColor[0];
		clearValue.Color[1] = clearColor[1];
		clearValue.Color[2] = clearColor[2];
		clearValue.Color[3] = clearColor[3];
	}
	else
	{
		clearValue.Color[0] = 0.0f;
		clearValue.Color[1] = 0.0f;
		clearValue.Color[2] = 0.0f;
		clearValue.Color[3] = 1.0f;
	}

	// リソースを生成
	HRESULT hr = Engine::GetInstance().GetDevice()->CreateCommittedResource(
		&prop,										// ヒープのプロパティ
		D3D12_HEAP_FLAG_NONE,						// ヒープのフラグ
		&desc,										// リソースの設定
		D3D12_RESOURCE_STATE_RENDER_TARGET,			// リソースの初期状態
		&clearValue,								// 最適化されたクリア値
		IID_PPV_ARGS(m_pResource.ReleaseAndGetAddressOf())); // リソースのポインタ

	if (FAILED(hr))
	{
		MessageBox(nullptr, "レンダーターゲットリソースの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return hr;
	}

	// 幅と高さを保存
	m_Width = width;
	m_Height = height;

	// シェーダーリソースビューの設定
	m_ViewDesc.Format = m_pResource->GetDesc().Format; // フォーマット
	m_ViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; // コンポーネントのマッピング
	m_ViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // テクスチャ2D
	m_ViewDesc.Texture2D.MipLevels = 1; // ミップ数

	m_pDescriptorHeap = new DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	return hr;
}
