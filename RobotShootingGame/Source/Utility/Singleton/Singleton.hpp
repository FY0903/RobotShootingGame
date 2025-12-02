# pragma once

class SingletonFinalizer final
{
public:
    using FinalizerFunc = void(*)();
    static void AddFinalizer(FinalizerFunc func);
    static void Finalize();
};

template <typename T>
class Singleton
{
public:
    inline static T& GetInstance()
    {
        std::call_once(initFlag, Create);
        assert(instance);
        return *instance;
    }

protected:
    // デフォルトコンストラクタ、デストラクタ
    Singleton() = default;
    virtual ~Singleton() = default;

    // コピー・代入禁止
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    inline static void Create()
    {
        instance = new T;
        SingletonFinalizer::AddFinalizer(&Singleton<T>::Destroy);
    }

    inline static void Destroy()
    {
        delete instance;
        instance = nullptr;
    }

    static std::once_flag initFlag;
    static T* instance;
};

template <typename T> std::once_flag Singleton<T>::initFlag;
template <typename T> T* Singleton<T>::instance = nullptr;
