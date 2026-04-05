/*+===================================================================
	File: ThreadPool.hpp
	Summary: スレッドプールクラスのヘッダーファイル
	Author: AT13C192 23 林田イジ
	Date: 2026/04/06 0:34:01 新規作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <atomic>
#include <vector>

/**
 * @brief ThreadPoolクラス
 * @details ワーカースレッドを管理し、ジョブを並列実行するスレッドプール。
 */
class ThreadPool : public Singleton<ThreadPool>
{
public:
	/**
	 * @brief スレッドプールを初期化します。
	 * @param threadCount ワーカースレッド数。0の場合はCPU論理コア数-1。
	 */
	void Init(uint32_t threadCount = 0);

	/**
	 * @brief ジョブをキューに投入します。
	 * @param job 実行するジョブ（ラムダ式など）。
	 */
	void Submit(std::function<void()> job);

	/**
	 * @brief 全てのジョブが完了するまで待機します。
	 */
	void WaitAll();

	/**
	 * @brief ワーカースレッド数を取得します。
	 * @return ワーカースレッド数。
	 */
	uint32_t GetThreadCount() const { return static_cast<uint32_t>(m_Workers.size()); }

private:
	friend class Singleton<ThreadPool>;

	/**
	 * コンストラクタ
	 */
	ThreadPool() = default;

	/**
	 * デストラクタ
	 */
	~ThreadPool();

	/**
	 * @brief ワーカースレッドのメインループ。
	 */
	void WorkerMain();

	std::vector<std::thread> m_Workers{};			// ワーカースレッド配列
	std::queue<std::function<void()>> m_JobQueue{};	// ジョブキュー
	std::mutex m_Mutex{};							// キュー保護用ミューテックス
	std::condition_variable m_JobCV{};				// ジョブ到着通知用
	std::condition_variable m_DoneCV{};				// 全完了通知用
	std::atomic<int> m_ActiveJobs{ 0 };				// 実行中+待機中ジョブ数
	bool m_Stop{ false };							// 終了フラグ
};
