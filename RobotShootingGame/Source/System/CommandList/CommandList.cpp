/*+===================================================================
	File: CommandList.cpp
	Summary: コマンドリストのソースファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/28 12:51 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "CommandList.hpp"

CommandList::~CommandList()
{
	Term();
}

bool CommandList::Init(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type, uint32_t count)
{
	if (!pDevice || !count) return false;	// デバイスが無効またはカウントが0の場合は失敗

	m_pAllocators.resize(count);	// コマンドアロケータの配列を確保

	for (uint32_t i = 0; i < count; ++i)
	{
		HRESULT hr = pDevice->CreateCommandAllocator(type, IID_PPV_ARGS(m_pAllocators[i].GetAddressOf()));
		if (FAILED(hr))
		{
			MessageBox(nullptr, "コマンドアロケータの作成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return false;	// コマンドアロケータの作成に失敗した場合はfalseを返す
		}
	}

	HRESULT hr = pDevice->CreateCommandList(
		1,// ノードマスク（通常は1）
		type,	// コマンドリストのタイプ
		m_pAllocators[0].Get(),	// 最初のコマンドアロケータ
		nullptr,	// 初期パイプラインステート（nullptrでOK）
		IID_PPV_ARGS(m_pCmdList.GetAddressOf())	// コマンドリストのインターフェースを取得
	);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "コマンドリストの作成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false;	// コマンドリストの作成に失敗した場合はfalseを返す
	}

	m_index = 0;	// コマンドアロケータのインデックスを初期化
	return true;	// 成功した場合はtrueを返す
}

void CommandList::Term()
{
	m_pCmdList.Reset();	// コマンドリストをリセット

	for (auto& allocator : m_pAllocators)
	{
		allocator.Reset();	// 各コマンドアロケータをリセット
	}

	m_pAllocators.clear();	// コマンドアロケータの配列をクリア
	m_pAllocators.shrink_to_fit();	// メモリを解放
}

ID3D12GraphicsCommandList* CommandList::Reset()
{
	HRESULT hr = m_pAllocators[m_index]->Reset();	// 現在のコマンドアロケータをリセット
	if (FAILED(hr))
	{
		MessageBox(nullptr, "コマンドアロケータのリセットに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return nullptr;	// リセットに失敗した場合はnullptrを返す
	}

	hr = m_pCmdList->Reset(m_pAllocators[m_index].Get(), nullptr);	// コマンドリストをリセット
	if (FAILED(hr))
	{
		MessageBox(nullptr, "コマンドリストのリセットに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return nullptr;	// リセットに失敗した場合はnullptrを返す
	}

	m_index = (m_index + 1) % static_cast<uint32_t>(m_pAllocators.size());	// 次のコマンドアロケータのインデックスを更新
	return m_pCmdList.Get();	// リセットしたコマンドリストを返す
}
