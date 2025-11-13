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
#include "System/Engine/Engine.hpp"
#include "../DirectXTex/d3dx12.h"

VertexBuffer::VertexBuffer(size_t size, size_t stride, const void* pInitData)
{
	CD3DX12_HEAP_PROPERTIES prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); // ヒープのプロパティ
	CD3DX12_RESOURCE_DESC resc = CD3DX12_RESOURCE_DESC::Buffer(size); // リソースの設定

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
		MessageBox(nullptr, "VertexBufferの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return; // エラー終了
	}

	// 頂点バッファビューの設定
	m_View.BufferLocation = m_pBuffer->GetGPUVirtualAddress();	// バッファの仮想アドレス
	m_View.SizeInBytes = static_cast<UINT>(size);				// バッファのサイズ
	m_View.StrideInBytes = static_cast<UINT>(stride);			// 頂点のサイズ

	// マッピング(書き込み)
	if (pInitData)
	{
		void* ptr = nullptr;
		hr = m_pBuffer->Map(0, nullptr, &ptr);
		if (FAILED(hr))
		{
			MessageBox(nullptr, "VertexBufferのマッピングに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return; // エラー終了
		}

		// 頂点データをマッピング先に設定
		memcpy(ptr, pInitData, size);

		// マッピング解除
		m_pBuffer->Unmap(0, nullptr);
	}
}