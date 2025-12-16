/*+===================================================================
	File: Texture.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/05 9:36:05 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Texture.hpp"
#include "System/Engine/Engine.hpp"

HRESULT Texture::Load(const std::string& FileName)
{
	HRESULT hr = S_OK;

	// マルチバイト文字列をワイド文字列に変換
	std::wstring path;
	int buff = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, FileName.c_str(), -1, nullptr, 0);
	
	// 変換に成功した場合
	if (buff > 0)
	{
		// バッファサイズ分のメモリを確保
		path.resize(buff);
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, FileName.c_str(), -1, &path[0], buff);
		assert(buff == static_cast<int>(path.size())); // 変換に失敗した場合
	}

	// 画像読み込み
	DirectX::TexMetadata meta{};
	DirectX::ScratchImage scratch{};

	// 拡張子を取得
	if (path.find(L".png") != std::string::npos)
	{
		hr = DirectX::LoadFromWICFile(
			path.c_str(),				// ファイルパス
			DirectX::WIC_FLAGS_NONE,	// フラグ（なし）
			&meta,						// メタデータ
			scratch);					// スクラッチイメージ
	}
	else if (path.find(L".dds") != std::string::npos)
	{
		hr = DirectX::LoadFromDDSFile(
			path.c_str(),						// ファイルパス
			DirectX::DDS_FLAGS_NONE,	// フラグ（なし）
			&meta,						// メタデータ
			scratch);					// スクラッチイメージ
	}
	else if (path.find(L".tga") != std::string::npos)
	{
		hr = DirectX::LoadFromTGAFile(
			path.c_str(),				// ファイルパス
			&meta,						// メタデータ
			scratch);					// スクラッチイメージ
	}

	// テクスチャの作成
	hr = CreateTextureFromScratch(scratch, meta);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "テクスチャの読み込みに失敗しました", "エラー", MB_OK);
		return hr;
	}

	return hr;
}

HRESULT Texture::CreateDefaultTexture(BYTE R, BYTE G, BYTE B, BYTE A)
{
	// 1x1の白テクスチャを作成
	DirectX::TexMetadata meta{};
	DirectX::ScratchImage scratch{};

	HRESULT hr = scratch.Initialize2D(
		DXGI_FORMAT_R8G8B8A8_UNORM, // フォーマット
		1,							// 幅
		1,							// 高さ
		1,							// 配列サイズ
		1);							// ミップ数
	if (FAILED(hr))
	{
		MessageBox(nullptr, "デフォルトテクスチャの初期化に失敗しました", "エラー", MB_OK);
		return hr;
	}

	// メタデータを設定
	meta.format = DXGI_FORMAT_R8G8B8A8_UNORM;
	meta.width = 1;
	meta.height = 1;
	meta.arraySize = 1;
	meta.mipLevels = 1;

	// ピクセルデータを設定（白色）
	// MEMO: DXGI_FORMAT_R8G8B8A8_UNORMだから、最大値は255
	auto img = scratch.GetImage(0, 0, 0);
	img->pixels[0] = R; // R
	img->pixels[1] = G; // G
	img->pixels[2] = B; // B
	img->pixels[3] = A; // A

	// テクスチャの作成
	hr = CreateTextureFromScratch(scratch, meta);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "デフォルトテクスチャの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	return hr;
}

HRESULT Texture::CreateTextureFromScratch(const DirectX::ScratchImage& scratch, const DirectX::TexMetadata& meta)
{
	m_IsOpaque = scratch.IsAlphaAllOpaque();

	// テクスチャの情報を取得
	auto img = scratch.GetImage(0, 0, 0);
	CD3DX12_HEAP_PROPERTIES prop = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	CD3DX12_RESOURCE_DESC resc = CD3DX12_RESOURCE_DESC::Tex2D(
		meta.format,	// フォーマット
		static_cast<UINT>(meta.width),			// 幅
		static_cast<UINT>(meta.height),			// 高さ
		static_cast<UINT16>(meta.arraySize),	// 配列サイズ
		static_cast<UINT16>(meta.mipLevels));	// ミップ数

	// リソースを生成
	HRESULT hr = Engine::GetInstance().GetDevice()->CreateCommittedResource(
		&prop,										// ヒープのプロパティ
		D3D12_HEAP_FLAG_NONE,						// ヒープのフラグ
		&resc,										// リソースの設定
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, // リソースの初期状態
		nullptr,									// 最適化されたクリア値
		IID_PPV_ARGS(m_pResource.ReleaseAndGetAddressOf())); // リソースのポインタ
	if (FAILED(hr))
	{
		MessageBox(nullptr, "テクスチャリソースの生成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	// テクスチャにデータ転送
	hr = m_pResource->WriteToSubresource(
		0,				// サブリソース
		nullptr,		// ボックス（全領域にコピーするのでnullptr）
		img->pixels,	// 転送元データ
		static_cast<UINT>(img->rowPitch),		// 1ラインのサイズ
		static_cast<UINT>(img->slicePitch));	// 1枚のサイズ

	// 幅と高さを保存
	m_Width = static_cast<UINT>(meta.width);
	m_Height = static_cast<UINT>(meta.height);

	// シェーダーリソースビューの設定
	m_ViewDesc.Format = m_pResource->GetDesc().Format; // フォーマット
	m_ViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; // コンポーネントのマッピング
	m_ViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // テクスチャ2D
	m_ViewDesc.Texture2D.MipLevels = 1; // ミップ数

	return hr;
}
