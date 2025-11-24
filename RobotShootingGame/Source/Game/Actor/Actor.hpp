/*+===================================================================
	File: Actor.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/14 11:45:12 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/Transform/Transform.hpp"
#include "Utility/Compornent/Component.hpp"
#include "System/Engine/Engine.hpp"
#include "Utility/ConstantBuffer/ConstantBuffer.hpp"
#include "Utility/RootSignature/RootSignature.hpp"
#include <list>

/**
 * @brief Actorクラス
 */
class Actor
{
public:
	enum Tags
	{
		None,
		Player,
		Enemy,
		Max,
	};

	/**
	 * コンストラクタ
	 */
	Actor();

	/**
	 * デストラクタ
	 */
	virtual ~Actor();

	void Init();
	void Update();
	void Draw();
	void Uninit();

	template<typename T = Component>
	inline T* AddComponent()
	{
		T* component = new T(this);
		m_Components.emplace_back(component);

		return component;
	}

	template<typename T = Component>
	inline T* GetComponent()
	{
		for (Component* component : m_Components)
		{
			T* castedComponent = dynamic_cast<T*>(component);
			if (castedComponent)
			{
				return castedComponent;
			}
		}
		return nullptr;
	}

	template<typename T = Component>
	inline void RemoveComponent(T* component)
	{
		m_Components.remove_if([component](Component* comp) { return comp == component; });
		delete component;
	}

	inline Transform& GetTransform() { return m_Transform; }
	inline const Tags& GetTag() { return m_Tag; }

protected:
	virtual void OnInit() = 0;
	virtual void OnUninit() = 0;

	std::list<Component*> m_Components{};

	Transform m_Transform{};
	Tags m_Tag{};
};
