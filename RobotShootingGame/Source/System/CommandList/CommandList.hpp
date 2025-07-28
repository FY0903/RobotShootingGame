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
#include "../App.hpp"
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

	bool Init(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type, uint32_t count);

	void Term();

	ID3D12GraphicsCommandList* Reset();

private:
	CommandList(const CommandList&) = delete;	// コピーコンストラクタを禁止
	void operator = (const CommandList&) = delete;	// 代入演算子を禁止
	
	ComPtr<ID3D12GraphicsCommandList> m_pCmdList{};	// コマンドリスト
	std::vector<ComPtr<ID3D12CommandAllocator>> m_pAllocators{};	// コマンドアロケータの配列
	uint32_t m_index{};	// 現在のコマンドアロケータのインデックス
};
