/*+===================================================================
	File: IndexBuffer.cpp
	Summary: インデックスバッファのソースファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/26 11:04 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "IndexBuffer.hpp"

IndexBuffer::~IndexBuffer()
{
	Term();	// リソースの解放
}

bool IndexBuffer::Init(ID3D12Device* pDevice, size_t size, const uint32_t* pInitData)
{
	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES prop{};
	prop.Type = D3D12_HEAP_TYPE_UPLOAD; // アップロード用ヒープ
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// CPUページプロパティは不明
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// メモリプールの優先度は不明
	prop.CreationNodeMask = 1;	// 作成ノードマスク
	prop.VisibleNodeMask = 1;	// 可視ノードマスク

	// リソースの設定
	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; // バッファリソース
	desc.Alignment = 0; // アライメントは0
	desc.Width = static_cast<UINT64>(size);// バッファの幅（サイズ）
	desc.Height = 1; // 高さは1（バッファなので）
	desc.DepthOrArraySize = 1; // 深さまたは配列サイズは1
	desc.MipLevels = 1; // ミップレベルは1
	desc.Format = DXGI_FORMAT_UNKNOWN; // フォーマットは不明
	desc.SampleDesc.Count = 1; // サンプル数は1
	desc.SampleDesc.Quality = 0; // クオリティは0
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR; // レイアウトは行メジャー
	desc.Flags = D3D12_RESOURCE_FLAG_NONE; // リソースフラグはなし

	// リソースの作成
	HRESULT hr = pDevice->CreateCommittedResource(
		&prop, // ヒーププロパティ
		D3D12_HEAP_FLAG_NONE, // ヒープフラグはなし
		&desc, // リソースの設定
		D3D12_RESOURCE_STATE_GENERIC_READ, // 初期状態は汎用読み取り
		nullptr, // クリエーションデータはなし
		IID_PPV_ARGS(m_pIB.GetAddressOf()) // リソースのポインタを取得
	);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "IndexBufferの作成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false;
	}

	// インデックスバッファービューの設定
	m_view.BufferLocation = m_pIB->GetGPUVirtualAddress();	// GPU仮想アドレスを取得
	m_view.Format = DXGI_FORMAT_R32_UINT;	// インデックスフォーマットは32ビット整数
	m_view.SizeInBytes = static_cast<UINT>(size);	// サイズを設定

	// 初期化データがあるなら書き込む
	if (pInitData)
	{
		void* ptr = Map();
		if (!ptr) return false; // マッピングに失敗したら終了

		memcpy(ptr, pInitData, size); // 初期化データをコピー

		m_pIB->Unmap(0, nullptr); // アンマップ

		return true; // 成功
	}
}

void IndexBuffer::Term()
{
	m_pIB.Reset(); // リソースをリセット
	m_view = {}; // ビューをリセット
}

uint32_t* IndexBuffer::Map()
{
	uint32_t* ptr;
	HRESULT hr = m_pIB->Map(0, nullptr, reinterpret_cast<void**>(&ptr));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "インデックスバッファのマッピングに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return nullptr;
	}

	return ptr;
}
