/*+===================================================================
	File: Singleton.hpp
	Summary: Singletonクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/03 13:39 初回作成
            26/01/16 11:26 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Singleton.hpp"

namespace
{
	std::mutex g_Mutex;	// 終了処理関数リストの保護用ミューテックス
	std::vector<SingletonFinalizer::FinalizerFunc> g_Finalizers;	// 終了処理関数リスト
}

void SingletonFinalizer::AddFinalizer(FinalizerFunc func)
{
	std::lock_guard<std::mutex> lock(g_Mutex);
	
	// 最大登録数に達していない場合のみ登録
	if (g_Finalizers.size() < g_Finalizers.max_size())
	{
		g_Finalizers.push_back(func);
	}
}

void SingletonFinalizer::Finalize()
{
	std::lock_guard<std::mutex> lock(g_Mutex);

	// 登録された終了処理関数を逆順で実行
	for (auto it = g_Finalizers.rbegin(); it != g_Finalizers.rend(); ++it)
	{
		// nullptrチェック
		if (*it)
		{
			(*it)();	// 終了処理関数の実行
		}
	}
}
