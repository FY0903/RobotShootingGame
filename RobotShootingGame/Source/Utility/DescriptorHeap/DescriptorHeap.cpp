/*+===================================================================
	File: DescriptorHeap.cpp
	Summary: ディスクリプタヒープのソースファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/24 14:22 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "DescriptorHeap.hpp"
#include "../../System/Engine/Engine.hpp"

// ==============================
//	constexpr
// ==============================
constexpr UINT HANDLE_MAX = 512;	// 最大ディスクリプタ数

DescriptorHeap::DescriptorHeap()
{
	m_pHandles.clear();
	m_pHandles.reserve(HANDLE_MAX);

	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.NodeMask = 1;										// ノードマスク
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;		// ヒープのタイプ
	desc.NumDescriptors = HANDLE_MAX;						// ディスクリプタ数
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	// シェーダーから見えるようにする

	HRESULT hr = Engine::GetInstance().GetDevice()->CreateDescriptorHeap(
		&desc,												// ヒープの設定
		IID_PPV_ARGS(m_pHeap.ReleaseAndGetAddressOf()));	// ヒープのポインタ

	if (FAILED(hr))
	{
		MessageBox(nullptr, "ディスクリプタヒープの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return; // エラー終了
	}

	m_IncrementSize = Engine::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(desc.Type); // ディスクリプタサイズを取得
}

DescriptorHeap::~DescriptorHeap()
{
	for (auto& handle : m_pHandles)
	{
		delete handle;
		handle = nullptr;
	}

	m_pHandles.clear();
}

DescriptorHandle* DescriptorHeap::Register(ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC desc)
{
	auto count = m_pHandles.size();
	if (count >= HANDLE_MAX) return nullptr;

	DescriptorHandle* pHandle = new DescriptorHandle();

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_pHeap->GetCPUDescriptorHandleForHeapStart();	// ヒープの先頭ハンドルを取得
	cpuHandle.ptr += m_IncrementSize * count;	// 登録されている数だけオフセット

	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = m_pHeap->GetGPUDescriptorHandleForHeapStart();	// ヒープの先頭ハンドルを取得
	gpuHandle.ptr += m_IncrementSize * count;	// 登録されている数だけオフセット

	pHandle->HandleCPU = cpuHandle;
	pHandle->HandleGPU = gpuHandle;

	Engine::GetInstance().GetDevice()->CreateShaderResourceView(
		resource,				// テクスチャ
		&desc,					// SRVの設定（デフォルト）
		pHandle->HandleCPU);	// CPUディスクリプタハンドル

	m_pHandles.push_back(pHandle);
	return pHandle;
}
