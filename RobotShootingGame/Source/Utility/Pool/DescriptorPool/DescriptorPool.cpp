/*+===================================================================
	File: DiscriptorPool.cpp
	Summary: ディスクリプタプールのソースファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/24 14:22 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "DescriptorPool.hpp"

bool DiscriptorPool::Create(ID3D12Device* pDevice, const D3D12_DESCRIPTOR_HEAP_DESC* desc, DiscriptorPool** ppPool)
{
	// 引数チェック
	if (!pDevice || !desc || !ppPool) return false;	// pDevice, desc, ppPoolのいずれかがnullptrならfalseを返す

	// インスタンスを生成
	auto instance = new(std::nothrow) DiscriptorPool();	// 例外を投げないようにnewを使用
	if (!instance) return false;	// インスタンスの生成に失敗したらfalseを返す

	// ディスクリプタヒープを作成
	HRESULT hr = pDevice->CreateDescriptorHeap(desc, IID_PPV_ARGS(instance->m_pHeap.GetAddressOf()));	// ディスクリプタヒープを作成
	if (FAILED(hr))
	{
		MessageBox(nullptr, "ディスクリプタヒープの作成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);	// エラーメッセージを表示
		instance->Release();	// インスタンスを解放
		return false;	// 失敗したらfalseを返す
	}

	// ディスクリプタサイズを取得
	instance->m_DescriptorSize = pDevice->GetDescriptorHandleIncrementSize(desc->Type);	// ディスクリプタサイズを取得

	// インスタンスを設定
	*ppPool = instance;	// ppPoolにインスタンスを設定

	return true;	// 成功したらtrueを返す
}

void DiscriptorPool::AddRef()
{
	++m_RefCount;	// 参照カウントを増やす
}

void DiscriptorPool::Release()
{
	--m_RefCount;	// 参照カウントを減らす
	if (m_RefCount == 0) delete this;	// 参照カウントが0になったら自分自身を削除
}

DescriptorHandle* DiscriptorPool::AllocHandle()
{
	auto func = [&](uint32_t index, DescriptorHandle* pHandle)
	{
			auto handleCPU = m_pHeap->GetCPUDescriptorHandleForHeapStart();	// CPUディスクリプタハンドルを取得
			handleCPU.ptr += index * m_DescriptorSize;	// インデックスに基づいてオフセットを計算

			auto handleGPU = m_pHeap->GetGPUDescriptorHandleForHeapStart();	// GPUディスクリプタハンドルを取得
			handleGPU.ptr += index * m_DescriptorSize;	// インデックスに基づいてオフセットを計算

			pHandle->HandleCPU = handleCPU;	// CPUディスクリプタハンドルを設定
			pHandle->HandleGPU = handleGPU;	// GPUディスクリプタハンドルを設定
	};

	return m_Pool.Alloc(func);	// プールからディスクリプタハンドルを割り当てる
}

void DiscriptorPool::FreeHandle(DescriptorHandle* pHandle)
{
	if (pHandle)
	{
		m_Pool.Free(pHandle);	// プールにディスクリプタハンドルを戻す
		pHandle = nullptr;	// ポインタをnullptrに設定
	}
}

DiscriptorPool::~DiscriptorPool()
{
	m_Pool.Term();	// プールを解放
	m_pHeap.Reset();	// ディスクリプタヒープをリセット
	m_DescriptorSize = 0;	// ディスクリプタサイズをリセット
}
