/*+===================================================================
	File: VertexBuffer.cpp
	Summary: VertexBufferのソースファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/26 12:23 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "VertexBuffer.hpp"

VertexBuffer::~VertexBuffer()
{
	Term();
}

bool VertexBuffer::Init(ID3D12Device* pDevice, size_t size, size_t stride, const void* pInitData)
{
	if (!pDevice || size == 0 || stride == 0) return false;

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES prop{};
	prop.Type = D3D12_HEAP_TYPE_UPLOAD; // アップロードヒープを使用
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// CPUページプロパティは不明
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN; // メモリプールは不明
	prop.CreationNodeMask = 1; // 作成ノードマスク
	prop.VisibleNodeMask = 1; // 可視ノードマスク

	// リソースの設定
	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; // バッファリソース
	desc.Alignment = 0; // アライメントは0
	desc.Width = static_cast<UINT64>(size); // バッファの幅（サイズ）
	desc.Height = 1; // 高さは1（バッファなので）
	desc.DepthOrArraySize = 1; // 深さまたは配列サイズは1
	desc.MipLevels = 1; // ミップレベルは1
	desc.Format = DXGI_FORMAT_UNKNOWN; // フォーマットは不明
	desc.SampleDesc.Count = 1; // サンプル数は1
	desc.SampleDesc.Quality = 0; // クオリティは0
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR; // レイアウトは行メジャー
	desc.Flags = D3D12_RESOURCE_FLAG_NONE; // リソースフラグはなし

	// リソースを生成
	HRESULT hr = pDevice->CreateCommittedResource(
		&prop,	 // ヒーププロパティ
		D3D12_HEAP_FLAG_NONE,	 // ヒープフラグはなし
		&desc,	 // リソースの設定
		D3D12_RESOURCE_STATE_GENERIC_READ,	 // リソース状態は汎用読み取り
		nullptr,	 // 初期クリア値はなし
		IID_PPV_ARGS(m_pVB.GetAddressOf())	 // リソースのポインタを取得
	);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "VertexBufferの作成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false;
	}

	// 頂点バッファービューの設定
	m_view.BufferLocation = m_pVB->GetGPUVirtualAddress(); // GPU仮想アドレスを取得
	m_view.StrideInBytes = static_cast<UINT>(stride); // ストライドを設定
	m_view.SizeInBytes = static_cast<UINT>(size); // サイズを設定

	// 初期化データがある場合は書き込む
	if (pInitData)
	{
		void* ptr = Map();
		if (!ptr) return false; // マッピングに失敗した場合は終了
	
		memcpy(ptr, pInitData, size); // 初期化データをコピー
		
		m_pVB->Unmap(0, nullptr); // マッピングを解除
	}

	return true; // 成功
}

void VertexBuffer::Term()
{
	m_pVB.Reset(); // リソースをリセット
	m_view = {}; // ビューをリセット
}

void* VertexBuffer::Map() const
{
	void* ptr;
	HRESULT hr = m_pVB->Map(0, nullptr, &ptr);
	if (FAILED(hr) || !ptr)
	{
		MessageBox(nullptr, "VertexBufferのマッピングに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return nullptr; // マッピングに失敗した場合はnullptrを返す
	}

	return ptr; // マッピング成功時はポインタを返す
}
