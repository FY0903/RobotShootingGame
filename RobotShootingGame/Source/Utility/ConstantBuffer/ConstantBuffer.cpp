/*+===================================================================
	File: ConstantBuffer.cpp
	Summary: 定数バッファのソースファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/25 9:52 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "ConstantBuffer.hpp"
#include "../DirectXTex/d3dx12.h"
#include "System/Engine/Engine.hpp"

ConstantBuffer::ConstantBuffer(size_t size)
{
	size_t align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
	UINT64 sizeAligned = (size + (align - 1)) & ~(align - 1);	// 256バイトアライメントに調整

	assert(alignof(sizeAligned) == align);	// アライメントの確認
	assert(sizeAligned % align == 0);		// サイズがアライメントの倍数であることを確認

	CD3DX12_HEAP_PROPERTIES prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); // ヒープのプロパティ
	CD3DX12_RESOURCE_DESC resc = CD3DX12_RESOURCE_DESC::Buffer(sizeAligned); // リソースの設定

	// リソースを生成
	HRESULT hr = Engine::GetInstance().GetDevice()->CreateCommittedResource(
		&prop,								// ヒープのプロパティ
		D3D12_HEAP_FLAG_NONE,				// ヒープのフラグ
		&resc,								// リソースの設定
		D3D12_RESOURCE_STATE_GENERIC_READ,	// リソースの初期状態
		nullptr,							// 最適化されたクリア値
		IID_PPV_ARGS(m_pBuffer.ReleaseAndGetAddressOf())); // バッファのポインタ

	if (FAILED(hr))
	{
		MessageBox(nullptr, "ConstantBufferの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return; // エラー終了
	}

	// 定数バッファビューの設定
	m_Desc.BufferLocation = m_pBuffer->GetGPUVirtualAddress();	// バッファの仮想アドレス
	m_Desc.SizeInBytes = static_cast<UINT>(sizeAligned);		// バッファのサイズ

	// マッピング(書き込み)
	hr = m_pBuffer->Map(0, nullptr, &m_pMappedPtr);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "ConstantBufferのマッピングに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return; // エラー終了
	}

	// 頂点データと異なり、マッピング解除しない
	// 理由：定数バッファは頻繁に更新されるため、マッピング状態を維持することでパフォーマンスが向上するため
}
