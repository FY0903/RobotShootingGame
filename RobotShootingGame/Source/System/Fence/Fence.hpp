/*+===================================================================
	File: Fence.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/27 17:24 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <d3d12.h>
#include "../App.hpp"

/**
 * @brief Fenceクラス
 */
class Fence
{
public:
	/**
	 * コンストラクタ
	 */
	Fence() = default;

	/**
	 * デストラクタ
	 */
	~Fence();

	bool Init(ID3D12Device* pDevice);

	void Term();

	void Wait(ID3D12CommandQueue* pQueue, UINT timeout);

	void Sync(ID3D12CommandQueue* pQueue);

private:
	Fence(const Fence&) = delete; // コピーコンストラクタを削除
	void operator=(const Fence&) = delete; // コピー代入演算子を削除

	ComPtr<ID3D12Fence> m_pFence{}; // フェンスオブジェクト
	HANDLE m_hEvent{}; // イベントハンドル
	UINT m_Counter{}; // フェンスのカウンター値
};
