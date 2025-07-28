/*+===================================================================
	File: Fence.hpp
	Summary: フェンスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/27 17:24 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <d3d12.h>
#include "../../Utility/ComPtr.h"

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

	/**
	 * @brief ID3D12Device を使用して初期化を行います。
	 * @param [pDevice] 初期化に使用する ID3D12Device へのポインタ。
	 * @return 初期化に成功した場合は true、失敗した場合は false を返します。
	 */
	bool Init(ID3D12Device* pDevice);

	/**
	 * @brief 終了処理を実行します。
	 */
	void Term();

	/**
	 * @brief 指定したコマンドキューが完了するまで、またはタイムアウトまで待機します。
	 * @param [pQueue] 待機対象となるID3D12CommandQueueへのポインタ。
	 * @param [timeout] 待機する最大時間（ミリ秒単位）。
	 */
	void Wait(ID3D12CommandQueue* pQueue, UINT timeout);

	/**
	 * @brief 指定されたID3D12CommandQueueの完了を待機します。
	 * @param [pQueue] 同期を行う対象のID3D12CommandQueueへのポインタ。
	 */
	void Sync(ID3D12CommandQueue* pQueue);

private:
	Fence(const Fence&) = delete; // コピーコンストラクタを削除
	void operator=(const Fence&) = delete; // コピー代入演算子を削除

	ComPtr<ID3D12Fence> m_pFence{}; // フェンスオブジェクト
	HANDLE m_hEvent{}; // イベントハンドル
	UINT m_Counter{}; // フェンスのカウンター値
};
