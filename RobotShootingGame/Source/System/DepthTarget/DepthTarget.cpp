/*+===================================================================
	File: DepthTarget.cpp
	Summary: DSVのソースファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/27 20:55 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "DepthTarget.hpp"
#include "../../Utility/Pool/DescriptorPool/DescriptorPool.hpp"

DepthTarget::~DepthTarget()
{
	Term();
}

bool DepthTarget::Init(ID3D12Device* pDevice, DescriptorPool* pPoolDSV, uint32_t width, uint32_t height, DXGI_FORMAT format)
{
	if (!pDevice || !pPoolDSV || width == 0 || height == 0) return false;

	assert(!m_pHandleDSV); // m_pHandleDSVがnullptrでないことを確認
	assert(!m_pPoolDSV); // m_pPoolDSVがnullptrでないことを確認

	m_pPoolDSV = pPoolDSV;
	m_pPoolDSV->AddRef(); // ディスクリプタプールの参照カウントを増やす

	m_pHandleDSV = m_pPoolDSV->AllocHandle(); // ディスクリプタハンドルを割り当て
	if (!m_pHandleDSV) return false; // ディスクリプタハンドルの割り当てに失敗したらfalseを返す

	D3D12_HEAP_PROPERTIES prop{};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT; // ヒープタイプをデフォルトに設定
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN; // CPUページプロパティを不明に設定
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN; // メモリプールの設定を不明にする
	prop.CreationNodeMask = 1; // 作成ノードマスクを1に設定（単一ノード）
	prop.VisibleNodeMask = 1; // 可視ノードマスクを1に設定（単一ノード）

	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // リソースの次元を2Dテクスチャに設定
	desc.Alignment = 0; // アライメントを0に設定（デフォルト）
	desc.Width = static_cast<UINT64>(width); // 幅を設定
	desc.Height = static_cast<UINT>(height); // 高さを設定
	desc.DepthOrArraySize = 1; // 深度またはアレイサイズを1に設定
	desc.MipLevels = 1; // ミップレベルを1に設定
	desc.Format = format; // フォーマットを指定
	desc.SampleDesc.Count = 1; // サンプル数を1に設定（マルチサンプリングなし）
	desc.SampleDesc.Quality = 0; // サンプル品質を0に設定（デフォルト）
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // テクスチャレイアウトを不明に設定
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // 深度ステンシルを許可するフラグを設定

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = format; // クリア値のフォーマットを指定
	clearValue.DepthStencil.Depth = 1.0f; // 深度クリア値を1.0に設定
	clearValue.DepthStencil.Stencil = 0; // ステンシルクリア値を0に設定

	HRESULT hr = pDevice->CreateCommittedResource(
		&prop,								// ヒーププロパティ
		D3D12_HEAP_FLAG_NONE,				// ヒープフラグ（なし）
		&desc,								// リソースの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	// 初期状態（深度書き込み）
		&clearValue,						// クリア値
		IID_PPV_ARGS(m_pTarget.GetAddressOf())); // リソースのポインタ
	if (FAILED(hr))
	{
		MessageBox(nullptr, "深度ターゲットのリソース作成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	m_ViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // ビューの次元を2Dテクスチャに設定
	m_ViewDesc.Format = format; // ビューのフォーマットを指定
	m_ViewDesc.Texture2D.MipSlice = 0; // ミップスライスを0に設定
	m_ViewDesc.Flags = D3D12_DSV_FLAG_NONE; // フラグをなしに設定

	pDevice->CreateDepthStencilView(
		m_pTarget.Get(),				// 深度ターゲットリソース
		&m_ViewDesc,					// 深度ステンシルビューの設定
		m_pHandleDSV->HandleCPU);		// ディスクリプタハンドル

	return true; // 初期化成功
}

void DepthTarget::Term()
{
	m_pTarget.Reset();

	if (m_pPoolDSV && m_pHandleDSV)
	{
		m_pPoolDSV->FreeHandle(m_pHandleDSV); // ディスクリプタハンドルを解放
		m_pHandleDSV = nullptr; // ハンドルをnullptrに設定
	}

	if (m_pPoolDSV)
	{
		m_pPoolDSV->Release(); // ディスクリプタプールの参照カウントを減らす
		m_pPoolDSV = nullptr; // プールをnullptrに設定
	}
}

D3D12_RESOURCE_DESC DepthTarget::GetDesc() const
{
	if (m_pTarget)
	{
		return m_pTarget->GetDesc(); // リソースの説明を返す
	}

	return D3D12_RESOURCE_DESC(); // リソースがない場合は空の説明を返す
}
