/*+===================================================================
	File: AnimatorController.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/27 11:51:37 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/Compornent/Component.hpp"
#include "Utility/Animation/Animation.hpp"
#include <unordered_map>
#include <string>
#include <memory>
#include <functional>

// ==============================
//	前方宣言
// ==============================
template<typename T>
class StateMachine;

template<typename T>
class StateBase
{
protected:
	friend class StateMachine<T>;

	virtual void OnStart(T* owner) {}
	virtual void OnUpdate(T* owner) {}
	virtual void OnExit(T* owner) {}

	StateMachine<T>* m_pMachine{};
	T* m_pOwner{};

private:
	void SetMachine(StateMachine<T>* machine) { m_pMachine = machine; }

	void Start(T* owner)
	{
		if (!m_pMachine || !m_pOwner) return;
		OnStart(owner);
	}

	void Update(T* owner)
	{
		if (!m_pMachine || !m_pOwner) return;
		OnUpdate(owner);
	}

	void Exit(T* owner)
	{
		if (!m_pMachine || !m_pOwner) return;
		OnExit(owner);
	}
};

template<typename T>
class StateMachine
{
public:
	StateMachine()
		: m_fcChangeState([]() {})
	{
	}

	void Start(T* pOwner)
	{
		m_pOwner = pOwner;
		m_fcChangeState = []() {};
	}

	template<typename StateType, typename... Args>
	void ChangeState(Args...args)
	{
		m_fcChangeState = [&]()
		{
			if (!m_pOwner) return;

			if (m_pCurrentState)
			{
				m_pCurrentState->Exit(m_pOwner);
				m_pCurrentState = nullptr;
			}

			m_pCurrentState = std::make_shared<StateType>(args...);
			if (!m_pCurrentState) return;
			m_pCurrentState->SetMachine(this);
			m_pCurrentState->Start(m_pOwner);
		}
	}

	void Update()
	{
		m_fcChangeState();
		m_fcChangeState = []() {};
		if (m_pCurrentState)
		{
			m_pCurrentState->Update(m_pOwner);
		}
	}

private:
	T* m_pOwner{};
	std::shared_ptr<StateBase<T>> m_pCurrentState{};
	std::function<void()> m_fcChangeState{};
};

/**
 * @brief AnimatorControllerクラス
 */
class AnimatorController : public Component
{
public:
	using Component::Component;

	/**
	 * コンストラクタ
	 */
	AnimatorController() = default;

	/**
	 * デストラクタ
	 */
	~AnimatorController() = default;

	void Update() override final;
	void Draw() override final;
	void Uninit() override final;
	
private:
};
