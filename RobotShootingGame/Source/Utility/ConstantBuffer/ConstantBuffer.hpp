/*+===================================================================
	File: ConstantBuffer.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/25 9:52 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <d3d12.h>
#include "../../System/App.hpp"
#include <vector>

// ==============================
//	前方宣言
// ==============================
class DescriptorHandle;
class DescriptorPool;

/**
 * @brief ConstantBufferクラス
 */
class ConstantBuffer
{
public:
	/**
	 * コンストラクタ
	 */
	ConstantBuffer() = default;

	/**
	 * デストラクタ
	 */
	~ConstantBuffer();

	bool Init(ID3D12Device* pDevice, DescriptorPool* pPool, size_t size);

	void Term();

	D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;

	D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU() const;

	D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU() const;

	void* GetPtr() const;

	template<typename T>
	inline T* GetPtr() { return reinterpret_cast<T*>(GetPtr()); }

private:
	ConstantBuffer(const ConstantBuffer&) = delete; // コピーコンストラクタを禁止
	void operator=(const ConstantBuffer&) = delete; // コピー代入演算子を禁止

	ComPtr<ID3D12Resource> m_pCB{};	// 定数バッファリソース
	DescriptorHandle* m_pHandle{};	// ディスクリプタハンドル
	DescriptorPool* m_pPool{};	// ディスクリプタプール
	D3D12_CONSTANT_BUFFER_VIEW_DESC m_Desc{};	// 定数バッファビューの記述
	void* m_pMappedPtr{};	// マッピングされたデータへのポインタ
};
