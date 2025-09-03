/*+===================================================================
	File: Texture2D.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/03 16:42:36 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Texture2D.hpp"
#include "../../../DirectXTex/DirectXTex.h"
#include "../../System/Engine/Engine.hpp"

std::wstring GetWideString(const std::string& str)
{
	auto num1 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str.c_str(), -1, nullptr, 0);

	std::wstring wstr;
	wstr.resize(num1);

	auto num2 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str.c_str(), -1, &wstr[0], num1);

	assert(num1 == num2);
	return wstr;
}

std::wstring FileExtension(const std::wstring& path)
{
	auto idx = path.rfind(L'.');
	return path.substr(idx + 1, path.length() - idx - 2);
}

Texture2D* Texture2D::Get(std::string path)
{
	auto wpath = GetWideString(path);

	return Get(wpath);
}

Texture2D* Texture2D::Get(std::wstring path)
{
	auto texture = new Texture2D(path);

	if (!texture->Resource())
	{
		return GetWhite();
	}

	return texture;
}

Texture2D* Texture2D::GetWhite()
{
	ID3D12Resource* buff = GetDefaultResource(4, 4);

	std::vector<unsigned char> data(4 * 4 * 4);
	std::fill(data.begin(), data.end(), 255);

	HRESULT hr = buff->WriteToSubresource(
		0,
		nullptr,
		data.data(),
		4 * 4,
		data.size());
	if (FAILED(hr))
	{
		return nullptr;
	}

	return new Texture2D(buff);
}

D3D12_SHADER_RESOURCE_VIEW_DESC Texture2D::ViewDesc() const
{
	D3D12_SHADER_RESOURCE_VIEW_DESC  desc{};
	desc.Format = m_pResource->GetDesc().Format; // フォーマット
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; // コンポーネントのマッピング
	desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // テクスチャ2D
	desc.Texture2D.MipLevels = 1; // ミップ数

	return desc;
}

Texture2D::Texture2D(std::string path)
{
	if (!Load(path))
	{
		MessageBox(nullptr, "テクスチャの読み込みに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return;
	}
}

Texture2D::Texture2D(std::wstring path)
{
	if (!Load(path))
	{
		MessageBox(nullptr, "テクスチャの読み込みに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return;
	}
}

Texture2D::Texture2D(ID3D12Resource* buffer)
{
	m_pResource = buffer;
	if (!m_pResource)
	{
		MessageBox(nullptr, "テクスチャの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return;
	}
}

bool Texture2D::Load(std::string& path)
{
	std::wstring wpath = GetWideString(path);

	return Load(wpath);
}

bool Texture2D::Load(std::wstring& path)
{
	// WICテクスチャの読み込み
	DirectX::TexMetadata meta{};
	DirectX::ScratchImage scratch{};
	std::wstring ext = FileExtension(path);

	HRESULT hr = E_FAIL;
	if (ext == L"png")
	{
		hr = DirectX::LoadFromWICFile(
			path.c_str(),				// ファイルパス
			DirectX::WIC_FLAGS_NONE,	// フラグ（なし）
			&meta,						// メタデータ
			scratch);					// スクラッチイメージ
	}
	else if (ext == L"dds")
	{
		hr = DirectX::LoadFromDDSFile(
			path.c_str(),				// ファイルパス
			DirectX::DDS_FLAGS_NONE,	// フラグ（なし）
			&meta,						// メタデータ
			scratch);					// スクラッチイメージ
	}
	else if (ext == L"tga")
	{
		hr = DirectX::LoadFromTGAFile(
			path.c_str(),				// ファイルパス
			&meta,						// メタデータ
			scratch);					// スクラッチイメージ
	}

	if (FAILED(hr))
	{
		return false;
	}

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
	hr = Engine::GetInstance().GetDevice()->CreateCommittedResource(
		&prop,										// ヒープのプロパティ
		D3D12_HEAP_FLAG_NONE,						// ヒープのフラグ
		&resc,										// リソースの設定
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, // リソースの初期状態
		nullptr,									// 最適化されたクリア値
		IID_PPV_ARGS(m_pResource.ReleaseAndGetAddressOf())); // リソースのポインタ

	if (FAILED(hr))
	{
		return false;
	}

	// テクスチャにデータ転送
	hr = m_pResource->WriteToSubresource(
		0,				// サブリソース
		nullptr,		// ボックス（全領域にコピーするのでnullptr）
		img->pixels,	// 転送元データ
		static_cast<UINT>(img->rowPitch),	// 1ラインのサイズ
		static_cast<UINT>(img->slicePitch));	// 1枚のサイズ

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

ID3D12Resource* Texture2D::GetDefaultResource(size_t width, size_t height)
{
	auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM,	// フォーマット
		static_cast<UINT>(width),		// 幅
		static_cast<UINT>(height));		// 高さ
	auto texProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	ID3D12Resource* buff = nullptr;
	HRESULT hr = Engine::GetInstance().GetDevice()->CreateCommittedResource(
		&texProp,									// ヒープのプロパティ
		D3D12_HEAP_FLAG_NONE,						// ヒープのフラグ
		&resDesc,									// リソースの設定
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, // リソースの初期状態
		nullptr,									// 最適化されたクリア値
		IID_PPV_ARGS(&buff));						// リソースのポインタ

	if (FAILED(hr))
	{
		assert(SUCCEEDED(hr));
		return nullptr;
	}

	return buff;
}
