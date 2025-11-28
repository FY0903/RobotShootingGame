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
#include "Game/Actor/Actor.hpp"
#include "Utility/Animation/Animation.hpp"
#include "Utility/Compornent/SkeletalAnimator/SkeletalAnimator.hpp"
#include "Utility/Input/Input.hpp"
#include <unordered_map>
#include <string>
#include <memory>
#include <functional>

template<typename OwnerType>
class StateMachine;

template<typename OwnerType>
class StateBase
{
protected:
	friend class StateMachine<OwnerType>;

	virtual void OnStart(OwnerType* pOwner) {}
	virtual void OnUpdate(OwnerType* pOwner) {}
	virtual void OnExit(OwnerType* pOwner) {}

private:

	void SetMachine(StateMachine<OwnerType>* pMachine)
	{
		m_pMachine = pMachine;
	}

	void CallStart(OwnerType* pOwner)
	{
		if (!m_pMachine || !pOwner) return;
		OnStart(pOwner);
	}

	void CallUpdate(OwnerType* pOwner)
	{
		if (!m_pMachine || !pOwner) return;
		OnUpdate(pOwner);
	}

	void CallExit(OwnerType* pOwner)
	{
		if (!m_pMachine || !pOwner) return;
		OnExit(pOwner);
	}

protected:
	StateMachine<OwnerType>* m_pMachine{};
	OwnerType* m_pOwner{};
};

template<typename OwnerType>
class StateMachine
{
public:
	StateMachine()
		:m_fnChangeState([]() {})
	{
	}

	void Start(OwnerType* pOwner)
	{
		m_pOwner = pOwner;
		m_fnChangeState = []() {};
	}

	template<typename StateType, typename...ArgType>
	void ChangeState(ArgType...args)
	{
		m_fnChangeState = [&]()
		{
			if (!m_pOwner) return;

			if (m_spNowState)
			{
				m_spNowState->CallExit(m_pOwner);
				m_spNowState = nullptr;
			}

			m_spNowState = std::make_shared<StateType>(args...);
			if (!m_spNowState) return;

			m_spNowState->SetMachine(this);
			m_spNowState->CallStart(m_pOwner);
		};
	}

	void Update()
	{
		m_fnChangeState();
		m_fnChangeState = []() {};

		if (m_spNowState)
		{
			m_spNowState->CallUpdate(m_pOwner);
		}

	}

private:
	OwnerType* m_pOwner{};
	std::shared_ptr<StateBase<OwnerType>> m_spNowState{};
	std::function<void()> m_fnChangeState{};
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

	void Init();
	void Update() override final;
	void Draw() override final;
	void Uninit() override final;

	void AddAnimation(const std::string& name, Animation* animation);

	inline Animation* GetAnimation(const std::string& name) { return m_Animations[name]; }
	inline Actor* GetOwner() { return m_Owner; }
	
private:
	std::unordered_map<std::string, Animation*> m_Animations{};
	StateMachine<AnimatorController> m_StateMachine{};
};

namespace AnimatorState
{
	class Run;
	class Walk;

	class Idle : public StateBase<AnimatorController>
	{
		void OnStart(AnimatorController* pOwner) override
		{
			Animation* pAnimation = pOwner->GetAnimation("Idle");
			if (!pAnimation) return;

			SkeletalAnimator* skeletalAnimator = pOwner->GetOwner()->GetComponent<SkeletalAnimator>();
			if (skeletalAnimator)
			{
				skeletalAnimator->PlayAnimation(pAnimation);
			}
		}

		void OnUpdate(AnimatorController* pOwner) override
		{
			if (Input::IsKeyTrigger(VK_RETURN))
			{
				m_pMachine->ChangeState<Walk>();
			}
		}
	};

	class Walk : public StateBase<AnimatorController>
	{
		void OnStart(AnimatorController* pOwner) override
		{
			Animation* pAnimation = pOwner->GetAnimation("Walk");
			if (!pAnimation) return;

			SkeletalAnimator* skeletalAnimator = pOwner->GetOwner()->GetComponent<SkeletalAnimator>();
			if (skeletalAnimator)
			{
				skeletalAnimator->PlayAnimation(pAnimation);
			}
		}

		void OnUpdate(AnimatorController* pOwner) override
		{
			if (Input::IsKeyTrigger(VK_RETURN))
			{
				m_pMachine->ChangeState<Run>();
			}
		}

		void OnExit(AnimatorController* pOwner) override
		{
			SkeletalAnimator* skeletalAnimator = pOwner->GetOwner()->GetComponent<SkeletalAnimator>();
			if (skeletalAnimator)
			{
				skeletalAnimator->StopAnimation();
			}
		}
	};

	class Run : public StateBase<AnimatorController>
	{
		void OnStart(AnimatorController* pOwner) override
		{
			Animation* pAnimation = pOwner->GetAnimation("Run");
			if (!pAnimation) return;

			SkeletalAnimator* skeletalAnimator = pOwner->GetOwner()->GetComponent<SkeletalAnimator>();
			if (skeletalAnimator)
			{
				skeletalAnimator->PlayAnimation(pAnimation);
			}
		}

		void OnUpdate(AnimatorController* pOwner) override
		{
			if (Input::IsKeyTrigger(VK_RETURN))
			{
				m_pMachine->ChangeState<Idle>();
			}
		}
	};
}
