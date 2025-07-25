/*+===================================================================
	File: ConstantBuffer.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/25 9:52 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "ConstantBuffer.hpp"
#include "../Pool/DescriptorPool/DescriptorPool.hpp"

ConstantBuffer::~ConstantBuffer()
{
	Term();
}

bool ConstantBuffer::Init(ID3D12Device* pDevice, DescriptorPool* pPool, size_t size)
{
	if (!pDevice || !pPool || size == 0) return false;

	assert(!m_pPool);	// 既に初期化されている場合はエラー
	assert(!m_pHandle);	// 既にハンドルが割り当てられている場合はエラー

	m_pPool = pPool;
	m_pPool->AddRef();	// プールの参照カウントを増やす

	size_t align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;	// 定数バッファのアライメント
	UINT64 sizeAligned = (size + (align - 1)) & ~(align - 1);	// アライメントされたサイズ

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES prop{};
	prop.Type = D3D12_HEAP_TYPE_UPLOAD;	// ヒープのタイプ（アップロード用）
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// CPUページプロパティ（不明）
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// メモリプールの優先度（不明）
	prop.CreationNodeMask = 1;	// 作成ノードマスク（単一ノード）
	prop.VisibleNodeMask = 1;	// 可視ノードマスク（単一ノード）

	// リソースの設定
	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;	// リソースの次元（バッファ）
	desc.Alignment = 0;	// アライメント（0ならデフォルト）
	desc.Width = sizeAligned;	// リソースの幅（アライメントされたサイズ）
	desc.Height = 1;	// 高さ（1なら1Dバッファ）
	desc.DepthOrArraySize = 1;	// 深度または配列サイズ（1なら1Dバッファ）
	desc.MipLevels = 1;	// ミップレベル（1ならミップマップなし）
	desc.Format = DXGI_FORMAT_UNKNOWN;	// フォーマット（バッファなので不明）
	desc.SampleDesc.Count = 1;	// サンプル数（1ならマルチサンプリングなし）
	desc.SampleDesc.Quality = 0;	// サンプル品質（0ならデフォルト）
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;	// レイアウト（行メジャー）
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;	// リソースフラグ（なし）

	// リソースを生成
	HRESULT hr = pDevice->CreateCommittedResource(
		&prop,								// ヒーププロパティ
		D3D12_HEAP_FLAG_NONE,				// ヒープフラグ（なし）
		&desc,								// リソースの設定
		D3D12_RESOURCE_STATE_GENERIC_READ,	// 初期状態（汎用読み取り）
		nullptr,							// クリア値（なし）
		IID_PPV_ARGS(m_pCB.GetAddressOf())); // リソースのポインタ
	if (FAILED(hr))
	{
		MessageBox(nullptr, "定数バッファの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	// マッピング
	hr = m_pCB->Map(0, nullptr, &m_pMappedPtr);
	if (FAILED(hr) || !m_pMappedPtr)
	{
		MessageBox(nullptr, "定数バッファのマッピングに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	m_Desc.BufferLocation = m_pCB->GetGPUVirtualAddress(); // GPU仮想アドレスを取得
	m_Desc.SizeInBytes = static_cast<UINT>(sizeAligned); // サイズを設定
	m_pHandle = m_pPool->AllocHandle(); // ディスクリプタハンドルをプールから取得

	pDevice->CreateConstantBufferView(
		&m_Desc,					// 定数バッファビューの設定
		m_pHandle->HandleCPU);		// CPUディスクリプタハンドル

	return true; // 初期化成功
}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetAddress() const
{
	return D3D12_GPU_VIRTUAL_ADDRESS();
}

D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer::GetHandleCPU() const
{
	return D3D12_CPU_DESCRIPTOR_HANDLE();
}

D3D12_GPU_DESCRIPTOR_HANDLE ConstantBuffer::GetHandleGPU() const
{
	return D3D12_GPU_DESCRIPTOR_HANDLE();
}

void* ConstantBuffer::GetPtr() const
{
	return nullptr;
}
