/*+===================================================================
	File: SceneBase.hpp
	Summary: シーンの基底クラスヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/21 15:33 初回作成
			26/01/14 19:14 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Game/Actor/Actor.hpp"

/**
 * @brief シーンのレイヤー
 */
enum class Layer : int
{
	DEFAULT,	// デフォルト
	PLAYER,		// プレイヤー
	ENEMY,		// 敵
	UI,			// UI
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

	/**
	 * @brief 初期化処理
	 */
	void Init();

	/**
	 * @brief 更新処理
	 */
	void Update();

	/**
	 * @brief 固定更新処理
	 */
	void FixedUpdate();

	/**
	 * @brief 描画処理
	 */
	void Draw();

	/**
	 * @brief 終了処理
	 */
	void Uninit();

	/**
	 * @brief 指定されたレイヤーに新しいゲームオブジェクトを追加します。
	 * @tparam T 追加するゲームオブジェクトの型。デフォルトは Actor です。
	 * @param layer ゲームオブジェクトを追加するレイヤー。デフォルトは Layer::DEFAULT です。
	 * @return 新しく作成され初期化されたゲームオブジェクトへのポインタ。
	 */
	template<typename T = Actor>
	inline T* AddGameObject(Layer layer = Layer::DEFAULT)
	{
		T* actor = new T();
		m_Actors[static_cast<size_t>(layer)].emplace_back(actor);
		actor->Init();
		return actor;
	}

	/**
	 * @brief 指定された型のゲームオブジェクトを検索して返します。
	 * @tparam T 検索するゲームオブジェクトの型。
	 * @return 見つかった場合は指定された型のゲームオブジェクトへのポインタ、見つからない場合はnullptr。
	 */
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
	virtual void OnInit() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnFixedUpdate() = 0;
	virtual void OnDraw() = 0;
	virtual void OnUninit() = 0;

	std::array<std::list<Actor*>, static_cast<size_t>(Layer::MAX)> m_Actors{}; // アクターコンテナ
};
