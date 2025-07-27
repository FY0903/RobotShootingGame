/*+===================================================================
	File: Fence.cpp
	Summary: フェンスのソースファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/27 17:24 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Fence.hpp"

Fence::~Fence()
{
	Term();
}

bool Fence::Init(ID3D12Device* pDevice)
{
	if (!pDevice) return false; // pDeviceがnullptrならfalseを返す

	// イベントを生成
	m_hEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);

	// フェンスを生成
	HRESULT hr = pDevice->CreateFence(
		0, // 初期値は0
		D3D12_FENCE_FLAG_NONE, // フェンスフラグはなし
		IID_PPV_ARGS(m_pFence.ReleaseAndGetAddressOf()) // IID_PPV_ARGSを使用してフェンスのポインタを取得
	);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "フェンスの初期化に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // フェンスの初期化に失敗したらfalseを返す
	}

	// カウンタ設定
	m_Counter = 1; // カウンタを1に初期化

	return true; // 初期化成功
}

void Fence::Term()
{
	// ハンドルを閉じる
	if (m_hEvent)
	{
		CloseHandle(m_hEvent);
		m_hEvent = nullptr; // ハンドルをnullptrに設定
	}

	// フェンスオブジェクトを破棄
	m_pFence.Reset(); // フェンスをリセット（破棄）

	// カウンターリセット
	m_Counter = 0; // カウンターを0にリセット
}

void Fence::Wait(ID3D12CommandQueue* pQueue, UINT timeout)
{
	if (!pQueue) return; // pQueueがnullptrなら何もしない

	const auto fenceValue = m_Counter; // 現在のフェンスカウンター値を取得

	// シグナル処理
	HRESULT hr = pQueue->Signal(m_pFence.Get(), fenceValue);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "フェンスのシグナルに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return; // シグナルに失敗したら何もしない
	}

	// カウンターを増やす
	++m_Counter; // フェンスカウンターをインクリメント

	// 次のフレームの描画準備がまだであれば待機する
	if (m_pFence->GetCompletedValue() < fenceValue)
	{
		// フェンスの完了を待機
		HRESULT hr = m_pFence->SetEventOnCompletion(fenceValue, m_hEvent);
		if (FAILED(hr))
		{
			MessageBox(nullptr, "フェンスの完了待機に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
			return; // 完了待機に失敗したら何もしない
		}

		// 待機処理
		if (WAIT_OBJECT_0 != WaitForSingleObjectEx(m_hEvent, timeout, FALSE))
		{
			MessageBox(nullptr, "フェンスの待機中にタイムアウトしました。", "エラー", MB_OK | MB_ICONERROR);
			return; // タイムアウトしたら何もしない
		}
	}
}

void Fence::Sync(ID3D12CommandQueue* pQueue)
{
	if (!pQueue) return; // pQueueがnullptrなら何もしない

	// シグナル処理
	HRESULT hr = pQueue->Signal(m_pFence.Get(), m_Counter);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "フェンスのシグナルに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return; // シグナルに失敗したら何もしない
	}
	
	// 完了時にイベントを設定
	HRESULT hr = m_pFence->SetEventOnCompletion(m_Counter, m_hEvent);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "フェンスの完了待機に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return; // 完了待機に失敗したら何もしない
	}

	// 待機処理
	if (WAIT_OBJECT_0 != WaitForSingleObjectEx(m_hEvent, INFINITE, FALSE))
	{
		MessageBox(nullptr, "フェンスの待機中にエラーが発生しました。", "エラー", MB_OK | MB_ICONERROR);
		return; // 待機中にエラーが発生したら何もしない
	}

	// カウンターをインクリメント
	++m_Counter; // フェンスカウンターをインクリメント
}
