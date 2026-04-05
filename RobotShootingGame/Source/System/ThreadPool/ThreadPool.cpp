/*+===================================================================
	File: ThreadPool.cpp
	Summary: スレッドプールクラスの実装ファイル
	Author: AT13C192 23 林田イジ
	Date: 2026/04/06 0:34:01 新規作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "ThreadPool.hpp"

void ThreadPool::Init(uint32_t threadCount)
{
	// スレッド数が0の場合はCPU論理コア数-1（最低1）
	if (threadCount == 0)
	{
		threadCount = std::thread::hardware_concurrency();
		if (threadCount > 1) threadCount--;	// メインスレッド分を引く
		if (threadCount == 0) threadCount = 1;
	}

	// ワーカースレッドを生成
	for (uint32_t i = 0; i < threadCount; ++i)
	{
		m_Workers.emplace_back(&ThreadPool::WorkerMain, this);
	}
}

void ThreadPool::Submit(std::function<void()> job)
{
	// ジョブをキューに追加
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_ActiveJobs++;
		m_JobQueue.push(std::move(job));
	}

	// ワーカースレッドを1つ起こす
	m_JobCV.notify_one();
}

void ThreadPool::WaitAll()
{
	// 全ジョブ完了まで待機
	std::unique_lock<std::mutex> lock(m_Mutex);
	m_DoneCV.wait(lock, [this]()
	{
		return m_ActiveJobs == 0 && m_JobQueue.empty();
	});
}

void ThreadPool::WorkerMain()
{
	while (true)
	{
		std::function<void()> job;

		// ジョブを取り出す
		{
			std::unique_lock<std::mutex> lock(m_Mutex);

			// ジョブが来るか終了フラグが立つまで待機
			m_JobCV.wait(lock, [this]()
			{
				return !m_JobQueue.empty() || m_Stop;
			});

			// 終了フラグが立っていてキューが空なら終了
			if (m_Stop && m_JobQueue.empty()) return;

			// キューからジョブを取り出す
			job = std::move(m_JobQueue.front());
			m_JobQueue.pop();
		}

		// ジョブを実行
		job();

		// 完了カウンタを減らして通知
		m_ActiveJobs--;
		m_DoneCV.notify_one();
	}
}

ThreadPool::~ThreadPool()
{
	// 終了フラグを立てて全ワーカーを起こす
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Stop = true;
	}
	m_JobCV.notify_all();

	// 全ワーカースレッドの終了を待つ
	for (auto& worker : m_Workers)
	{
		if (worker.joinable())
		{
			worker.join();
		}
	}
}
