/*+===================================================================
	File: SceneBase.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/21 15:33:17 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <array>
#include "Game/Actor/Actor.hpp"

enum class Layer : int
{
	DEFAULT,
	PLAYER,
	ENEMY,
	UI,
	MAX,
};

/**
 * @brief SceneBaseクラス
 */
class SceneBase
{
public:
	/**
	 * コンストラクタ
	 */
	SceneBase() = default;

	/**
	 * デストラクタ
	 */
	virtual ~SceneBase() = default;

	virtual void Init() = 0;
	virtual void Update();
	virtual void Draw();
	virtual void Uninit();

	template<typename T = Actor>
	inline T* AddGameObject(Layer layer = Layer::DEFAULT)
	{
		T* actor = new T();
		m_Actors[layer].emplace_back(actor);
		actor->Init();
		return actor;
	}

	template<typename T>
	inline T* GetGameObject()
	{
		for (auto& actors : m_Actors)
		{
			for (Actor* actor : actors)
			{
				T* castedActor = dynamic_cast<T*>(actor);
				if (castedActor)
				{
					return castedActor;
				}
			}
		}
		return nullptr;
	}

protected:
	std::array<std::list<Actor*>, static_cast<size_t>(Layer::MAX)> m_Actors{}; // アクターコンテナ
};
