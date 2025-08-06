/*+===================================================================
	File: Material.cpp
	Summary: マテリアルのソースファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/26 16:36 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Material.hpp"

Material::~Material()
{
	Term();
}

bool Material::Init(ID3D12Device* pDevice, DescriptorPool* pPool, size_t bufferSize, size_t count)
{
	if (!pDevice || !pPool || count == 0) return false;

	Term();

	m_pDevice = pDevice;
	m_pDevice->AddRef();

	m_pPool = pPool;
	m_pPool->AddRef();
	
	m_Subset.resize(count);

	// ダミーテクスチャ作成
	auto pTexture = new (std::nothrow) Texture();
	if (!pTexture) return false;

	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	// リソースの次元を2Dテクスチャに設定
	desc.Width = 1;	// 幅を1に設定
	desc.Height = 1;	// 高さを1に設定
	desc.DepthOrArraySize = 1;	// 深さまたはアレイサイズを1に設定
	desc.MipLevels = 1;	// ミップレベルを1に設定
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// フォーマットをRGBAに設定
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;	// テクスチャレイアウトを不明に設定
	desc.SampleDesc.Count = 1;	// サンプル数を1に設定
	desc.SampleDesc.Quality = 0;	// クオリティを0に設定

	if (!pTexture->Init(pDevice, pPool, &desc, false, false))
	{
		MessageBox(nullptr, "ダミーテクスチャの初期化に失敗しました", "エラー", MB_OK | MB_ICONERROR);
		pTexture->Term();
		delete pTexture;
		return false;
	}

	m_pTexture[L"DummyTag"] = pTexture;	// ダミーテクスチャをマップに追加

	auto size = bufferSize * count;
	if (size > 0)
	{
		// 定数バッファの初期化
		for (size_t i = 0; i < m_Subset.size(); ++i)
		{
			auto pBuffer = new (std::nothrow) ConstantBuffer();
			if (!pBuffer)
			{
				MessageBox(nullptr, "定数バッファの初期化に失敗しました", "エラー", MB_OK | MB_ICONERROR);
				return false;
			}

			if (!pBuffer->Init(pDevice, pPool, size))
			{
				MessageBox(nullptr, "定数バッファの初期化に失敗しました", "エラー", MB_OK | MB_ICONERROR);
				return false;
			}

			m_Subset[i].pConstantBuffer = pBuffer;	// 定数バッファをサブセットに設定
			for (size_t j = 0; j < TEXTURE_USAGE_COUNT; ++j)
				m_Subset[i].textureHandles[j].ptr = 0;	// テクスチャハンドルを初期化
		}
	}

	return true;	// 初期化成功
}

void Material::Term()
{
	for (auto& itr : m_pTexture)
	{
		if (itr.second)
		{
			itr.second->Term();	// テクスチャの終了処理
			delete itr.second;	// メモリ解放
			itr.second = nullptr;	// ポインタをnullptrに設定
		}
	}

	for (size_t i = 0; i < m_Subset.size(); ++i)
	{
		if (m_Subset[i].pConstantBuffer)
		{
			m_Subset[i].pConstantBuffer->Term();	// 定数バッファの終了処理
			delete m_Subset[i].pConstantBuffer;	// メモリ解放
			m_Subset[i].pConstantBuffer = nullptr;	// ポインタをnullptrに設定
		}
	}

	m_pTexture.clear();	// テクスチャマップをクリア
	m_Subset.clear();	// サブセットベクターをクリア

	if (m_pDevice)
	{
		m_pDevice->Release();	// デバイスの参照カウントを減らす
		m_pDevice = nullptr;	// ポインタをnullptrに設定
	}

	if (m_pPool)
	{
		m_pPool->Release();	// ディスクリプタプールの参照カウントを減らす
		m_pPool = nullptr;	// ポインタをnullptrに設定
	}
}

bool Material::SetTexture(size_t index, TEXTURE_USAGE usage, const std::wstring& path, DirectX::ResourceUploadBatch& batch)
{
	if (index >= GetCount()) return false;	// インデックスが範囲外ならfalseを返す

	if (m_pTexture.find(path) != m_pTexture.end())
	{
		// 既にテクスチャが存在する場合は再利用
		m_Subset[index].textureHandles[usage] = m_pTexture[path]->GetHandleGPU();
		return true;	// 成功
	}

	auto pTexture = new (std::nothrow) Texture();
	if (!pTexture)
	{
		MessageBox(nullptr, "テクスチャの初期化に失敗しました", "エラー", MB_OK | MB_ICONERROR);
		return false;	// メモリ確保失敗
	}

	bool isSRGB = (usage == TEXTURE_USAGE_DIFFUSE);	// ディフューズマップの場合はsRGBカラー空間を使用

	if (!pTexture->Init(m_pDevice, m_pPool, path.c_str(), isSRGB, batch))
	{
		MessageBox(nullptr, "テクスチャの初期化に失敗しました", "エラー", MB_OK | MB_ICONERROR);
		pTexture->Term();	// テクスチャの終了処理
		delete pTexture;	// メモリ解放
		return false;	// 初期化失敗
	}

	m_pTexture[path] = pTexture;	// テクスチャをマップに追加
	m_Subset[index].textureHandles[usage] = pTexture->GetHandleGPU();	// テクスチャハンドルを設定
	
	return true;	// 成功
}

void* Material::GetBufferPtr(size_t index) const
{
	if (index >= GetCount()) return nullptr;	// インデックスが範囲外ならnullptrを返す

	return m_Subset[index].pConstantBuffer->GetPtr();	// 定数バッファのポインタを返す
}

D3D12_GPU_VIRTUAL_ADDRESS Material::GetBufferAddress(size_t index) const
{
	if (index >= GetCount()) return D3D12_GPU_VIRTUAL_ADDRESS();	// インデックスが範囲外なら0を返す

	return m_Subset[index].pConstantBuffer->GetAddress();	// 定数バッファのGPU仮想アドレスを返す
}

D3D12_GPU_DESCRIPTOR_HANDLE Material::GetTextureHandle(size_t index, TEXTURE_USAGE usage) const
{
	if (index >= GetCount()) return D3D12_GPU_DESCRIPTOR_HANDLE();	// インデックスが範囲外なら無効なハンドルを返す

	return m_Subset[index].textureHandles[usage];	// 指定されたテクスチャのGPUディスクリプタハンドルを返す
}
