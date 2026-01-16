/*+===================================================================
    File: Singleton.hpp
    Summary: Singletonクラスのヘッダーファイル
    Author: AT13C192 23 藤原佑埜
    Date: 2025/09/03 13:39 初回作成
            26/01/16 11:26 コメント記載
===================================================================+*/
#pragma once

/**
 * @brief シングルトンに登録された終了処理（ファイナライザ）を登録・実行するためのクラス。
 */
class SingletonFinalizer final
{
public:
    using FinalizerFunc = void(*)();

    /**
	 * @brief 終了処理関数を登録します。
	 * @param func 登録する終了処理関数へのポインタ。
     */
    static void AddFinalizer(FinalizerFunc func);

    /**
	 * @brief 登録された終了処理関数をすべて実行します。
     */
    static void Finalize();
};

/**
 * @brief 型Tのシングルトンインスタンスへの参照を返します。
 * @tparam T シングルトンとして管理する型。
 * @details Tはデフォルトコンストラクタを持ち、Singleton<T>のfriendクラスである必要があります。
 *          std::call_onceを使ってスレッド安全に一度だけ初期化され、SingletonFinalizerにより終了時に破棄されます。
 */
template <typename T>
class Singleton
{
public:
    /**
     * @brief シングルトンインスタンスへの参照を取得します。
     * @return T型のシングルトンインスタンスへの参照。
	 */
    inline static T& GetInstance()
    {
        std::call_once(initFlag, Create);
        assert(instance);
        return *instance;
    }

protected:
    /**
	 * @brief コンストラクタ
     */
    Singleton() = default;

    /**
	 * @brief デストラクタ
     */
    virtual ~Singleton() = default;

    // コピー・代入禁止
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    /**
	 * @brief シングルトンインスタンスを生成し、終了時に破棄されるよう登録します。
     */
    inline static void Create()
    {
        instance = new T;
        SingletonFinalizer::AddFinalizer(&Singleton<T>::Destroy);
    }

    /**
	 * @brief シングルトンインスタンスを破棄します。
     */
    inline static void Destroy()
    {
        delete instance;
        instance = nullptr;
    }

	static std::once_flag initFlag; // 初期化フラグ
	static T* instance;             // シングルトンインスタンスへのポインタ
};

template <typename T> std::once_flag Singleton<T>::initFlag;
template <typename T> T* Singleton<T>::instance = nullptr;
