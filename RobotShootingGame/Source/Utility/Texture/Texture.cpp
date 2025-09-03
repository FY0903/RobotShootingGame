/*+===================================================================
	File: Texture.cpp
	Summary: テクスチャクラスのソースファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/24 16:30 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Texture.hpp"
#include <DDSTextureLoader.h>
#include "../Pool/DescriptorPool/DescriptorPool.hpp"

Texture::~Texture()
{
	Term();
}

bool Texture::Init(ID3D12Device* pDevice, DescriptorHeap* pPool, const wchar_t* filename, DirectX::ResourceUploadBatch& uploadBatch)
{
	// 引数チェック
	if (!pDevice || !pPool || !filename) return false;	// pDevice, pPool, filenameのいずれかがnullptrならfalseを返す

	assert(!m_pPool);	// m_pPoolがnullptrでないことを確認
	assert(!m_pHandle);	// m_pHandleがnullptrでないことを確認

	// ディスクリプタプールを設定
	m_pPool = pPool;
	m_pPool->AddRef();	// ディスクリプタプールの参照カウントを増やす

	m_pHandle = m_pPool->AllocHandle();	// ディスクリプタハンドルを割り当て
	if (!m_pHandle) return false;	// ディスクリプタハンドルの割り当てに失敗したらfalseを返す

	// ファイルからテクスチャを生成
	bool isCube = false;	// キューブマップではないと仮定
	HRESULT hr = DirectX::CreateDDSTextureFromFile(
		pDevice,					// デバイス
		uploadBatch,				// アップロードバッチ
		filename,					// テクスチャのファイル名
		m_pTex.GetAddressOf(),		// テクスチャリソースのポインタ
		true,					// ミップマップを生成するかどうか
		0,						// 最大サイズ（0は無制限）
		nullptr,				// アルファモード（使用しないのでnullptr）
		&isCube);				// キューブマップかどうかのフラグ
	if (FAILED(hr))
	{
		MessageBox(nullptr, "テクスチャの読み込みに失敗しました。", "エラー", MB_OK | MB_ICONERROR);	// エラーメッセージを表示
		return false;	// 失敗したらfalseを返す
	}

	// シェーダーリソースビューの設定を求める
	auto viewDesc = GetViewDesc(isCube);	// キューブマップかどうかでビューの設定を取得

	// シェーダーリソースビューを作成
	pDevice->CreateShaderResourceView(
		m_pTex.Get(),			// テクスチャリソース
		&viewDesc,				// ビューの設定
		m_pHandle->HandleCPU);	// CPUディスクリプタハンドル

	return true;	// 成功したらtrueを返す
}

bool Texture::Init(ID3D12Device* pDevice, DescriptorHeap* pPool, const D3D12_RESOURCE_DESC* pDesc, bool isCube)
{
	if (!pDevice || !pPool || !pDesc) return false;	// 引数チェック

	assert(!m_pPool);	// m_pPoolがnullptrでないことを確認
	assert(!m_pHandle);	// m_pHandleがnullptrでないことを確認

	// ディスクリプタプールを設定
	m_pPool = pPool;
	m_pPool->AddRef();	// ディスクリプタプールの参照カウントを増やす

	// ディスクリプタハンドルを取得
	m_pHandle = m_pPool->AllocHandle();	// ディスクリプタハンドルを割り当て
	if (!m_pHandle) return false;	// ディスクリプタハンドルの割り当てに失敗したらfalseを返す

	D3D12_HEAP_PROPERTIES prop{};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT;	// ヒープのタイプをデフォルトに設定
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// CPUページプロパティを不明に設定
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// メモリプールの設定を不明にする
	prop.CreationNodeMask = 0;	// 作成ノードマスクを0に設定（単一ノード）
	prop.VisibleNodeMask = 0;	// 可視ノードマスクを0に設定（単一ノード）

	HRESULT hr = pDevice->CreateCommittedResource(
		&prop,					// ヒーププロパティ
		D3D12_HEAP_FLAG_NONE,	// ヒープフラグ（なし）
		pDesc,					// リソースの設定
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, // 初期状態をピクセルシェーダーリソースに設定
		nullptr,				// クリア値（なし）
		IID_PPV_ARGS(m_pTex.GetAddressOf())); // リソースのポインタ
	if (FAILED(hr))
	{
		MessageBox(nullptr, "テクスチャの作成に失敗しました。", "エラー", MB_OK | MB_ICONERROR); // エラーメッセージを表示
		return false; // 失敗したらfalseを返す
	}

	// シェーダーリソースビューの設定を求める
	auto viewDesc = GetViewDesc(isCube); // キューブマップかどうかでビューの設定を取得

	// シェーダーリソースビューを作成
	pDevice->CreateShaderResourceView(
		m_pTex.Get(),			// テクスチャリソース
		&viewDesc,				// ビューの設定
		m_pHandle->HandleCPU);	// CPUディスクリプタハンドル

	return true; // 成功したらtrueを返す
}

void Texture::Term()
{
	m_pTex.Reset();	// テクスチャリソースをリセット

	// ディスクリプタハンドルを解放
	if (m_pHandle && m_pPool)
	{
		m_pPool->FreeHandle(m_pHandle);	// ディスクリプタハンドルをプールに返す
		m_pHandle = nullptr;	// ハンドルをnullptrに設定
	}

	// ディスクリプタプールを解放
	if (m_pPool)
	{
		m_pPool->Release();	// ディスクリプタプールの参照カウントを減らす
		m_pPool = nullptr;	// プールをnullptrに設定
	}
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetHandleCPU() const
{
	if (m_pHandle && m_pHandle->HasCPU())
	{
		return m_pHandle->HandleCPU;	// CPUディスクリプタハンドルを返す
	}

	return D3D12_CPU_DESCRIPTOR_HANDLE();	// ハンドルがない場合は空のハンドルを返す
}

D3D12_GPU_DESCRIPTOR_HANDLE Texture::GetHandleGPU() const
{
	if (m_pHandle && m_pHandle->HasGPU())
	{
		return m_pHandle->HandleGPU;	// GPUディスクリプタハンドルを返す
	}

	return D3D12_GPU_DESCRIPTOR_HANDLE();	// ハンドルがない場合は空のハンドルを返す
}

D3D12_SHADER_RESOURCE_VIEW_DESC Texture::GetViewDesc(bool isCube)
{
	auto desc = m_pTex->GetDesc();	// テクスチャの設定を取得
	D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc{};

	viewDesc.Format = desc.Format;	// フォーマットを設定
	viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;	// シェーダーのコンポーネントマッピングをデフォルトに設定

	switch (desc.Dimension)
	{
	case D3D12_RESOURCE_DIMENSION_BUFFER:
		abort();	// バッファの場合はサポートされていないため、処理を中断
		break;
	case D3D12_RESOURCE_DIMENSION_TEXTURE1D:	// 1Dテクスチャの場合
		if (desc.Dimension > 1)	// 1Dテクスチャアレイの場合
		{
			viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;	// 1Dテクスチャアレイの場合

			viewDesc.Texture1DArray.MostDetailedMip = 0;	// 最も詳細なミップレベルを0に設定
			viewDesc.Texture1DArray.MipLevels = desc.MipLevels;	// ミップレベル数を設定
			viewDesc.Texture1DArray.FirstArraySlice = 0;	// 最初のアレイスライスを0に設定
			viewDesc.Texture1DArray.ArraySize = desc.DepthOrArraySize;	// アレイサイズを設定
			viewDesc.Texture1DArray.ResourceMinLODClamp = 0.0f;	// 最小LODクランプを0に設定
		}
		else // 1Dテクスチャの場合
		{
			viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;	// 1Dテクスチャの場合

			viewDesc.Texture1D.MostDetailedMip = 0;	// 最も詳細なミップレベルを0に設定
			viewDesc.Texture1D.MipLevels = desc.MipLevels;	// ミップレベル数を設定
			viewDesc.Texture1D.ResourceMinLODClamp = 0.0f;	// 最小LODクランプを0に設定
		}
		break;
	case D3D12_RESOURCE_DIMENSION_TEXTURE2D:	// 2Dテクスチャの場合
		if (isCube)	// キューブマップの場合
		{
			if (desc.DepthOrArraySize > 6)	// キューブマップアレイの場合
			{
				viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;	// キューブマップアレイの場合

				viewDesc.TextureCubeArray.MostDetailedMip = 0;	// 最も詳細なミップレベルを0に設定
				viewDesc.TextureCubeArray.MipLevels = desc.MipLevels;	// ミップレベル数を設定
				viewDesc.TextureCubeArray.First2DArrayFace = 0;	// 最初の2Dアレイスライスを0に設定
				viewDesc.TextureCubeArray.NumCubes = desc.DepthOrArraySize / 6;	// キューブ数を設定
				viewDesc.TextureCubeArray.ResourceMinLODClamp = 0.0f;	// 最小LODクランプを0に設定
			}
			else // キューブマップの場合
			{
				viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;	// キューブマップの場合

				viewDesc.TextureCube.MostDetailedMip = 0;	// 最も詳細なミップレベルを0に設定
				viewDesc.TextureCube.MipLevels = desc.MipLevels;	// ミップレベル数を設定
				viewDesc.TextureCube.ResourceMinLODClamp = 0.0f;	// 最小LODクランプを0に設定
			}
		}
		else // キューブマップではない場合
		{
			if (desc.DepthOrArraySize > 1)	// 2Dテクスチャアレイまたはマルチサンプルの場合
			{
				if (desc.MipLevels > 1)	// 2Dマルチサンプルアレイの場合
				{
					viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;	// 2Dマルチサンプルアレイの場合

					viewDesc.Texture2DMSArray.FirstArraySlice = 0;	// 最初のアレイスライスを0に設定
					viewDesc.Texture2DMSArray.ArraySize = desc.DepthOrArraySize;	// アレイサイズを設定
				}
				else // 2Dテクスチャアレイの場合
				{
					viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;	// 2Dテクスチャアレイの場合

					viewDesc.Texture2DArray.MostDetailedMip = 0;	// 最も詳細なミップレベルを0に設定
					viewDesc.Texture2DArray.MipLevels = desc.MipLevels;	// ミップレベル数を設定
					viewDesc.Texture2DArray.FirstArraySlice = 0;	// 最初のアレイスライスを0に設定
					viewDesc.Texture2DArray.ArraySize = desc.DepthOrArraySize;	// アレイサイズを設定
					viewDesc.Texture2DArray.PlaneSlice = 0;	// プレーンスライスを0に設定
					viewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;	// 最小LODクランプを0に設定
				}
			}
			else // 2Dテクスチャまたはマルチサンプルの場合
			{
				if (desc.MipLevels > 1)	// 2Dマルチサンプルの場合
				{
					viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;	// 2Dマルチサンプルの場合
				}
				else // 2Dテクスチャの場合
				{
					viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2Dテクスチャの場合

					viewDesc.Texture2D.MostDetailedMip = 0;	// 最も詳細なミップレベルを0に設定
					viewDesc.Texture2D.MipLevels = desc.MipLevels;	// ミップレベル数を設定
					viewDesc.Texture2D.PlaneSlice = 0;	// プレーンスライスを0に設定
					viewDesc.Texture2D.ResourceMinLODClamp = 0.0f;	// 最小LODクランプを0に設定
				}
			}
		}
		break;
	case D3D12_RESOURCE_DIMENSION_TEXTURE3D:	// 3Dテクスチャの場合
		viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;	// 3Dテクスチャの場合

		viewDesc.Texture3D.MostDetailedMip = 0;	// 最も詳細なミップレベルを0に設定
		viewDesc.Texture3D.MipLevels = desc.MipLevels;	// ミップレベル数を設定
		viewDesc.Texture3D.ResourceMinLODClamp = 0.0f;	// 最小LODクランプを0に設定
		break;
	default:
		abort();	// サポートされていないリソース次元の場合は処理を中断
		break;
	}

	return viewDesc;	// ビューの設定を返す
}
