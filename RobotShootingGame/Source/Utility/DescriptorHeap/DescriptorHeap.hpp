/*+===================================================================
	File: DescriptorHeap.hpp
	Summary: ディスクリプタヒープのヘッダファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/24 14:22 初回作成
			26/01/15 18:26 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "../DirectXTex/d3dx12.h"

// ==============================
//	前方宣言
// ==============================
class ConstantBuffer;
class Texture2D;
class Texture;

/**
 * @brief DescriptorHandleクラス
 */
class DescriptorHandle
{
public:
	D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU{};	// CPUディスクリプタハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU{};	// GPUディスクリプタハンドル
};

/**
 * @brief DescriptorHeapクラス
 */
class DescriptorHeap
{
public:
	/**
	 * @brief DescriptorHeap クラスのコンストラクタ。指定した種類とフラグでディスクリプタヒープを作成・初期化します。
	 * @param type 作成するディスクリプタヒープの種類。CBV/SRV/UAV やサンプラーなどを指定します。
	 * @param flag ディスクリプタヒープの作成オプションを指定するフラグ（例: シェーダーからの可視化を有効にするフラグなど）。
	 */
	DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flag);

	/**
	 * @brief 指定した種類・サイズ・フラグで DescriptorHeap を初期化するコンストラクタ。
	 * @param type 作成するディスクリプタヒープの種類（D3D12_DESCRIPTOR_HEAP_TYPE）。例：CBV/SRV/UAV、Sampler、RTV、DSV など。
	 * @param numDescriptors ヒープに含めるディスクリプタの個数（UINT）。
	 * @param flag ヒープの動作を指定するフラグ（D3D12_DESCRIPTOR_HEAP_FLAGS）。例：シェーダー可視性の有無など。
	 */
	DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flag);

	/**
	 * デストラクタ
	 */
	~DescriptorHeap();

	/**
	 * @brief ディスクリプタサイズを取得します。
	 * @return ディスクリプタサイズをUINT型で返します。
	 */
	inline UINT GetIncrementSize() const { return m_IncrementSize; }
	
	/**
	 * @brief ディスクリプタヒープを取得します。
	 * @return ディスクリプタヒープのポインタを返します。
	 */
	ID3D12DescriptorHeap* GetHeap() const;
	
	/**
	 * @brief リソースのシェーダーリソースビュー（SRV）を登録し、対応するディスクリプタハンドルを返します。
	 * @param resource 登録対象の ID3D12Resource へのポインタ。ビューを作成するリソースを指定します。
	 * @param desc 作成する SRV の設定を指定する D3D12_SHADER_RESOURCE_VIEW_DESC 構造体（フォーマット、範囲、ディメンションなど）。
	 * @return 作成されたディスクリプタハンドルへのポインタ。登録に失敗した場合は nullptr を返すことがあります。
	 */
	DescriptorHandle* Register(ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC desc);
	
	/**
	 * @brief 指定したID3D12Resourceを登録し、対応するDescriptorHandleへのポインタを返します。
	 * @param resource 登録対象のID3D12Resourceへのポインタ。
	 * @return 登録されたリソースに対応するDescriptorHandleへのポインタ。
	 */
	DescriptorHandle* Register(ID3D12Resource* resource);

private:
	UINT m_IncrementSize{};							// ディスクリプタサイズ
	ComPtr<ID3D12DescriptorHeap> m_pHeap{};			// ディスクリプタヒープ
	std::vector<DescriptorHandle*> m_pHandles{};	// ディスクリプタハンドル
};
