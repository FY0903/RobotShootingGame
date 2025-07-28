/*+===================================================================
	File: CommandList.hpp
	Summary: コマンドリストのヘッダファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/28 12:51 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <d3d12.h>
#include "../../Utility/ComPtr.h"
#include <cstdint>
#include <vector>

/**
 * @brief CommandListクラス
 */
class CommandList
{
public:
	/**
	 * コンストラクタ
	 */
	CommandList() = default;

	/**
	 * デストラクタ
	 */
	~CommandList();

	/**
	 * @brief 初期化処理を行います。
	 * @param [pDevice] Direct3D 12 デバイスへのポインタ。
	 * @param [type] コマンドリストのタイプ。
	 * @param [count] コマンドアロケータの数。
	 * @return 初期化が成功した場合は true、失敗した場合は false を返します。
	 */
	bool Init(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type, uint32_t count);

	/**
	 * @brief 終了処理を行います。
	 */
	void Term();

	/**
	 * @brief コマンドリストを取得します。
	 * @return ID3D12GraphicsCommandListへのポインタ。
	 */
	ID3D12GraphicsCommandList* Reset();

private:
	CommandList(const CommandList&) = delete;	// コピーコンストラクタを禁止
	void operator = (const CommandList&) = delete;	// 代入演算子を禁止
	
	ComPtr<ID3D12GraphicsCommandList> m_pCmdList{};	// コマンドリスト
	std::vector<ComPtr<ID3D12CommandAllocator>> m_pAllocators{};	// コマンドアロケータの配列
	uint32_t m_index{};	// 現在のコマンドアロケータのインデックス
};
