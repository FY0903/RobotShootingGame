/*+===================================================================
	File: Pool.hpp
	Summary: 汎用的なプールクラスのヘッダファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/24 13:47 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <cstdint>
#include <mutex>
#include <cassert>
#include <functional>

/**
 * @brief Poolクラス
 */
template <typename T>
class Pool
{
public:
	/**
	 * コンストラクタ
	 */
	Pool() = default;

	/**
	 * デストラクタ
	 */
	~Pool()
	{
		Term();
	}

	/**
	 * @brief 指定された数のアイテム用バッファを初期化し、アクティブおよびフリーアイテムリストをセットアップします。
	 * @param [count] 初期化するアイテムの総数。
	 * @return 初期化が成功した場合は true、失敗した場合は false を返します。
	 */
	inline bool Init(uint32_t count)
	{
		std::lock_guard<std::mutex> guard(m_Mutex);

		m_pBuffer = static_cast<uint8_t*>(malloc(sizeof(Item) * (count + 2)));	// Itemのサイズ分だけバッファを確保
		if (!m_pBuffer) return false;

		m_Capacity = count;	// 総アイテム数を設定

		// インデックスを振る
		for (uint32_t i = 2, j = 2; i < m_Capacity + 2; ++i, ++j)
		{
			auto item = GetItem(i);	// インデックスに対応するItemオブジェクトを取得
			item->m_Index = j;	// インデックスを設定
		}

		m_pActive = GetItem(0);	// アクティブなアイテムの先頭を設定
		m_pActive->m_pPrev = m_pActive->m_pNext = m_pActive; // アクティブなアイテムの前後ポインタを自身に設定
		m_pActive->m_Index = uint32_t(-1);	// アクティブなアイテムのインデックスを無効に設定

		m_pFree = GetItem(1);	// フリーアイテムの先頭を設定
		m_pFree->m_Index = uint32_t(-2);	// フリーアイテムのインデックスを無効に設定

		for (uint32_t i = 1; i < m_Capacity + 2; ++i)
		{
			GetItem(i)->m_pPrev = nullptr;	// フリーアイテムの前ポインタをnullptrに設定
			GetItem(i)->m_pNext = GetItem(i + 1);	// フリーアイテムの次ポインタを次のアイテムに設定
		}

		GetItem(m_Capacity + 1)->m_pPrev = m_pFree;	// 最後のフリーアイテムの前ポインタをフリーアイテムの先頭に設定

		m_Count = 0;	// 確保したアイテム数を初期化
		
		return true;
	}

	/**
	 * @brief バッファと関連リソースを安全に解放し、内部状態をリセットします。
	 */
	inline void Term()
	{
		std::lock_guard<std::mutex> guard(m_Mutex);

		if (m_pBuffer)
		{
			free(m_pBuffer);	// バッファを解放
			m_pBuffer = nullptr;	// ポインタをnullptrに設定
		}

		m_pActive = nullptr;	// アクティブなアイテムのポインタをnullptrに設定
		m_pFree = nullptr;		// フリーアイテムのポインタをnullptrに設定
		m_Capacity = 0;		// 総アイテム数を0に設定
		m_Count = 0;		// 確保したアイテム数を0に設定
	}

	/**
	 * @brief 新しいアイテムのメモリを割り当て、必要に応じて初期化関数を呼び出します。
	 * @param [func] アイテムの初期化に使用する関数。uint32_t型のインデックスとT型のポインタを受け取ります。省略可能です。
	 * @return 割り当てられたT型アイテムのポインタ。割り当てに失敗した場合はnullptrを返します。
	 */
	inline T* Alloc(std::function<void(uint32_t, T*)> func = nullptr)
	{
		std::lock_guard<std::mutex> guard(m_Mutex);

		if (m_pFree->m_pNext == m_pFree || m_Count + 1 > m_Capacity) return nullptr;	// フリーアイテムがない、または総アイテム数を超える場合はnullptrを返す

		auto item = m_pFree->m_pNext;	// フリーアイテムの次のアイテムを取得
		m_pFree->m_pNext = item->m_pNext;	// フリーアイテムの次ポインタを更新

		item->m_pPrev = m_pActive->m_pPrev;	// アクティブなアイテムの前ポインタを取得
		item->m_pNext = m_pActive;	// アクティブなアイテムを次ポインタに設定
		item->m_pPrev->m_pNext = item->m_pNext->m_pPrev = item;	// アクティブなアイテムの前後ポインタを更新

		++m_Count;	// 確保したアイテム数を増やす

		// メモリ割り当て
		auto val = new((void*)item) T();	// Itemのメモリを確保せずにコンストラクタを呼び出す

		// 初期化の必要があれば呼び出す
		if (func)
		{
			func(item->m_Index, val);
		}

		return val;	// 割り当てたアイテムのポインタを返す
	}

	/**
	 * @brief 指定されたアイテムをフリーリストに戻します。
	 * @param [pItem] フリーリストに戻すT型のポインタ。nullptrの場合は何もしません。
	 */
	inline void Free(T* pItem)
	{
		if (!pItem) return;	// nullptrの場合は何もしない
		std::lock_guard<std::mutex> guard(m_Mutex);

		auto item = reinterpret_cast<Item*>(pItem);	// T型のポインタをItem型にキャスト

		item->m_pPrev->m_pNext = item->m_pNext;	// フリーアイテムの前ポインタを更新
		item->m_pNext->m_pPrev = item->m_pPrev;	// フリーアイテムの次ポインタを更新

		item->m_pPrev = nullptr;	// フリーアイテムの前ポインタをnullptrに設定
		item->m_pNext = m_pFree->m_pNext;	// フリーアイテムの次ポインタをフリーアイテムの先頭に設定

		m_pFree->m_pNext = item;	// フリーアイテムの先頭を更新
		--m_Count;	// 確保したアイテム数を減らす
	}

	/**
	 * @brief 現在の容量を取得します。
	 * @return オブジェクトの容量を表す 32 ビット符号なし整数値。
	 */
	inline uint32_t GetSize() const { return m_Capacity; } // 総アイテム数を返す

	/**
	 * @brief 使用済みのカウント値を取得します。
	 * @return 現在の使用済みカウント値（uint32_t型）を返します。
	 */
	inline uint32_t GetUsedCount() const { return m_Count; }	// 使用中のアイテム数を返す

	/**
	 * @brief 利用可能な数を取得します。
	 * @return 利用可能な数（m_Capacity から m_Count を引いた値）を返します。
	 */
	inline uint32_t GetAvailableCount() const { return m_Capacity - m_Count; }	// 使用可能なアイテム数を返す

private:
	struct Item
	{
		T m_Value{};		// アイテムの値
		uint32_t m_Index{};	// アイテムのインデックス（0からm_Capacity + 2までの値）
		Item* m_pNext{};	// 次のアイテムへのポインタ
		Item* m_pPrev{};	// 前のアイテムへのポインタ

		Item() = default;
		~Item() = default;
	};

	uint8_t* m_pBuffer{};	// バッファ
	Item* m_pActive{};		// アクティブなアイテムの先頭
	Item* m_pFree{};		// フリーアイテムの先頭
	uint32_t m_Capacity{};	// 総アイテム数
	uint32_t m_Count{};		// 確保したアイテム数
	std::mutex m_Mutex{};	// ミューテックス

	/**
	 * @brief 指定されたインデックスに対応するItemオブジェクトへのポインタを取得します。
	 * @param [index] 取得したいItemのインデックス。
	 * @return 指定されたインデックスのItemオブジェクトへのポインタ。
	 */
	inline Item* GetItem(uint32_t index)
	{
		assert(0 <= index && index <= m_Capacity + 2);

		return reinterpret_cast<Item*>(m_pBuffer + index * sizeof(Item));	// Itemのサイズ分だけポインタを進める
	}

	/**
	 * @brief 指定されたインデックスにアイテムを割り当て、バッファ上に配置されたItemオブジェクトへのポインタを返します。
	 * @param [index] 割り当てるアイテムのインデックス。0以上、m_Capacity + 2以下である必要があります。
	 * @return バッファ上に配置されたItemオブジェクトへのポインタ。
	 */
	inline Item* AssignItem(uint32_t index)
	{
		assert(0 <= index && index <= m_Capacity + 2);

		auto buf = (m_pBuffer + sizeof(Item) * index);	// Itemのサイズ分だけポインタを進める
		return new(buf)Item;	// メモリを確保せずにコンストラクタを呼び出す
	}

	Pool(const Pool&) = delete;				// コピーコンストラクタを削除
	void operator=(const Pool&) = delete;	// コピー代入演算子を削除
};
