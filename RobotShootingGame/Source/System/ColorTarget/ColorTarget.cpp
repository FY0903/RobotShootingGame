/*+===================================================================
	File: ColorTarget.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/28 13:57 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "ColorTarget.hpp"
#include "../../Utility/Pool/DescriptorPool/DescriptorPool.hpp"

// ==============================
//	static
// ==============================
namespace
{
	/**
	 * @brief 指定された DXGI フォーマットを sRGB フォーマットに変換します。
	 * @param [format] 変換対象の DXGI_FORMAT 値。
	 * @return sRGB 対応の DXGI_FORMAT。変換できない場合は元の値を返します。
	 */
	DXGI_FORMAT ConvertToSRGB(DXGI_FORMAT format)
	{
		DXGI_FORMAT result = format;	// 入力フォーマットを結果にコピー
		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM: result = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; break;
		case DXGI_FORMAT_BC1_UNORM: result = DXGI_FORMAT_BC1_UNORM_SRGB; break;
		case DXGI_FORMAT_BC2_UNORM: result = DXGI_FORMAT_BC2_UNORM_SRGB; break;
		case DXGI_FORMAT_BC3_UNORM: result = DXGI_FORMAT_BC3_UNORM_SRGB; break;
		case DXGI_FORMAT_B8G8R8A8_UNORM: result = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; break;
		case DXGI_FORMAT_B8G8R8X8_UNORM: result = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB; break;
		case DXGI_FORMAT_BC7_UNORM: result = DXGI_FORMAT_BC7_UNORM_SRGB; break;
		}
		return result;
	}
}

ColorTarget::~ColorTarget()
{
	Term();
}

bool ColorTarget::Init(ID3D12Device* pDevice, DescriptorPool* pPoolRTV, DescriptorPool* pPoolSRV, uint32_t width, uint32_t height, DXGI_FORMAT format, float clearColor[4])
{
	if (!pDevice || !pPoolRTV || !width || !height) return false;	// 無効な引数チェック
	
	assert(!m_pHandleRTV);	// 既にハンドルが存在する場合はエラー
	assert(!m_pPoolRTV);	// 既にプールが存在する場合はエラー

	m_pPoolRTV = pPoolRTV;	// ディスクリプタプールを設定
	m_pPoolRTV->AddRef();	// プールの参照カウントを増やす

	m_pHandleRTV = pPoolRTV->AllocHandle();	// レンダーターゲットビューのハンドルを割り当て
	if (!m_pHandleRTV)
	{
		MessageBox(nullptr, "レンダーターゲットビューのハンドルの割り当てに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false;	// ハンドルの割り当てに失敗した場合はfalseを返す
	}

	if (pPoolSRV)
	{
		m_pPoolSRV = pPoolSRV;	// シェーダーリソースビューのディスクリプタプールを設定
		m_pPoolSRV->AddRef();	// プールの参照カウントを増やす

		m_pHandleSRV = pPoolSRV->AllocHandle();	// シェーダーリソースビューのハンドルを割り当て

		if (!m_pHandleSRV)
		{
			MessageBox(nullptr, "シェーダーリソースビューのハンドルの割り当てに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return false;	// ハンドルの割り当てに失敗した場合はfalseを返す
		}
	}

	D3D12_HEAP_PROPERTIES prop{};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT;	// ヒープタイプをデフォルトに設定
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// CPUページプロパティを不明に設定
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// メモリプールの優先度を不明に設定
	prop.CreationNodeMask = 1;	// 作成ノードマスクを1に設定
	prop.VisibleNodeMask = 1;	// 可視ノードマスクを1に設定

	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	// リソースの次元を2Dテクスチャに設定
	desc.Alignment = 0;	// アライメントを0に設定
	desc.Width = static_cast<UINT64>(width);	// テクスチャの幅を設定
	desc.Height = static_cast<UINT>(height);	// テクスチャの高さを設定
	desc.DepthOrArraySize = 1;	// 深度またはアレイサイズを1に設定
	desc.MipLevels = 1;	// ミップレベルを1に設定
	desc.Format = format;	// フォーマットを指定
	desc.SampleDesc.Count = 1;	// サンプル数を1に設定
	desc.SampleDesc.Quality = 0;	// クオリティを0に設定
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;	// テクスチャレイアウトを不明に設定
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;	// リソースフラグをレンダーターゲットとして設定

	// クリア値を設定
	m_clearColor[0] = clearColor[0];
	m_clearColor[1] = clearColor[1];
	m_clearColor[2] = clearColor[2];
	m_clearColor[3] = clearColor[3];

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = format;	// クリア値のフォーマットを指定
	clearValue.Color[0] = m_clearColor[0];	// クリア値の赤成分を設定
	clearValue.Color[1] = m_clearColor[1];	// クリア値の緑成分を設定
	clearValue.Color[2] = m_clearColor[2];	// クリア値の青成分を設定
	clearValue.Color[3] = m_clearColor[3];	// クリア値のアルファ成分を設定

	HRESULT hr = pDevice->CreateCommittedResource(
		&prop,	// ヒーププロパティ
		D3D12_HEAP_FLAG_NONE,	// ヒープフラグ
		&desc,	// リソースの説明
		D3D12_RESOURCE_STATE_RENDER_TARGET,	// 初期状態をレンダーターゲットに設定
		&clearValue,	// クリア値
		IID_PPV_ARGS(m_pTarget.GetAddressOf())	// リソースのインターフェースを取得
	);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "レンダーターゲットの作成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false;	// レンダーターゲットの作成に失敗した場合はfalseを返す
	}

	m_RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	// レンダーターゲットビューの次元を2Dテクスチャに設定
	m_RTVDesc.Format = format;	// レンダーターゲットビューのフォーマットを指定
	m_RTVDesc.Texture2D.MipSlice = 0;	// ミップスライスを0に設定
	m_RTVDesc.Texture2D.PlaneSlice = 0;	// プレーンスライスを0に設定

	pDevice->CreateRenderTargetView(
		m_pTarget.Get(),	// レンダーターゲットリソース
		&m_RTVDesc,	// レンダーターゲットビューの説明
		m_pHandleRTV->HandleCPU	// CPUハンドルを取得
	);

	if (pPoolSRV)
	{
		m_SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		m_SRVDesc.Format = format;	// シェーダーリソースビューのフォーマットを指定
		m_SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;	// シェーダー4コンポーネントマッピングをデフォルトに設定
		m_SRVDesc.Texture2D.MostDetailedMip = 0;	// 最も詳細なミップを0に設定
		m_SRVDesc.Texture2D.MipLevels = 1;	// ミップレベルを1に設定
		m_SRVDesc.Texture2D.PlaneSlice = 0;	// プレーンスライスを0に設定
		m_SRVDesc.Texture2D.ResourceMinLODClamp = 0.0f;	// リソースの最小LODクランプを0に設定

		pDevice->CreateShaderResourceView(
			m_pTarget.Get(),	// シェーダーリソースリソース
			&m_SRVDesc,	// シェーダーリソースビューの説明
			m_pHandleSRV->HandleCPU	// CPUハンドルを取得
		);
	}

	return true;	// 成功した場合はtrueを返す
}

bool ColorTarget::InitFromBackBuffer(ID3D12Device* pDevice, DescriptorPool* pPoolRTV, bool useSRGB, uint32_t index, IDXGISwapChain* pSwapChain)
{
	if (!pDevice || !pPoolRTV || !pSwapChain) return false;	// 無効な引数チェック

	assert(!m_pHandleRTV);	// 既にハンドルが存在する場合はエラー
	assert(!m_pPoolRTV);	// 既にプールが存在する場合はエラー

	m_pPoolRTV = pPoolRTV;	// ディスクリプタプールを設定
	m_pPoolRTV->AddRef();	// プールの参照カウントを増やす

	m_pHandleRTV = pPoolRTV->AllocHandle();	// レンダーターゲットビューのハンドルを割り当て
	if (!m_pHandleRTV)
	{
		MessageBox(nullptr, "レンダーターゲットビューのハンドルの割り当てに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false;	// ハンドルの割り当てに失敗した場合はfalseを返す
	}

	HRESULT hr = pSwapChain->GetBuffer(index, IID_PPV_ARGS(m_pTarget.GetAddressOf()));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "バックバッファの取得に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false;	// バックバッファの取得に失敗した場合はfalseを返す
	}

	DXGI_SWAP_CHAIN_DESC desc{};
	pSwapChain->GetDesc(&desc);

	auto format = desc.BufferDesc.Format;	// バックバッファのフォーマットを取得

	if (useSRGB)
	{
		format = ConvertToSRGB(format);	// フォーマットをsRGBに変換
	}

	m_RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	// レンダーターゲットビューの次元を2Dテクスチャに設定
	m_RTVDesc.Format = format;	// レンダーターゲットビューのフォーマットをバックバッファのフォーマットに設定
	m_RTVDesc.Texture2D.MipSlice = 0;	// ミップスライスを0に設定
	m_RTVDesc.Texture2D.PlaneSlice = 0;	// プレーンスライスを0に設定

	pDevice->CreateRenderTargetView(
		m_pTarget.Get(),	// レンダーターゲットリソース
		&m_RTVDesc,	// レンダーターゲットビューの説明
		m_pHandleRTV->HandleCPU	// CPUハンドルを取得
	);

	return true;	// 成功した場合はtrueを返す
}

void ColorTarget::Term()
{
	m_pTarget.Reset();	// レンダーターゲットリソースをリセット

	if (m_pPoolRTV && m_pHandleRTV)
	{
		m_pPoolRTV->FreeHandle(m_pHandleRTV);	// レンダーターゲットビューのハンドルを解放
		m_pHandleRTV = nullptr;	// ハンドルをnullptrに設定
	}

	if (m_pPoolRTV)
	{
		m_pPoolRTV->Release();	// ディスクリプタプールの参照カウントを減らす
		m_pPoolRTV = nullptr;	// プールをnullptrに設定
	}

	if (m_pPoolSRV && m_pHandleSRV)
	{
		m_pPoolSRV->FreeHandle(m_pHandleSRV);	// シェーダーリソースビューのハンドルを解放
		m_pHandleSRV = nullptr;	// ハンドルをnullptrに設定
	}

	if (m_pPoolSRV)
	{
		m_pPoolSRV->Release();	// シェーダーリソースビューのディスクリプタプールの参照カウントを減らす
		m_pPoolSRV = nullptr;	// プールをnullptrに設定
	}
}

D3D12_RESOURCE_DESC ColorTarget::GetDesc() const
{
	if (m_pTarget)
	{
		return m_pTarget->GetDesc();	// レンダーターゲットリソースの説明を取得
	}
	else
	{
		return D3D12_RESOURCE_DESC();	// リソースが存在しない場合は空の説明を返す
	}
}

void ColorTarget::ClearView(ID3D12GraphicsCommandList* pCmdList)
{
	// レンダーターゲットビューをクリア
	pCmdList->ClearRenderTargetView(
		m_pHandleRTV->HandleCPU,	// レンダーターゲットビューのハンドル
		m_clearColor,	// クリアカラー
		0, nullptr	// フラグと矩形は使用しない
	);
}
