/*+===================================================================
	File: Actor.hpp
	Summary: アクターの基底クラスヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/14 11:45 初回作成
			26/01/14 18:53 コメント記載
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
#include "Utility/MaterialManager/MaterialManager.hpp"

/**
 * @brief Actorクラス
 */
class Actor
{
public:
	/**
	 * @brief アクターのタグ
	 */
	enum Tags
	{
		None,	// タグ無し
		Player,	// プレイヤー
		Enemy,	// 敵
		Max,
	};

	/**
	 * コンストラクタ
	 */
	Actor() = default;

	/**
	 * デストラクタ
	 */
	virtual ~Actor() = default;

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
	 * @brief エンティティに新しいコンポーネントを追加します。
	 * @tparam T 追加するコンポーネントの型。デフォルトは Component です。
	 * @return 新しく作成され追加されたコンポーネントへのポインタ。
	 */
	template<typename T = Component>
	inline T* AddComponent()
	{
		T* component = new T(this);
		m_Components.emplace_back(component);

		return component;
	}

	/**
	 * @brief コンポーネントのリストから指定された型のコンポーネントを取得します。
	 * @tparam T 取得するコンポーネントの型。デフォルトは Component です。
	 * @return 指定された型の最初のコンポーネントへのポインタ。見つからない場合は nullptr を返します。
	 */
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

	/**
	 * @brief 指定されたコンポーネントを削除します。
	 * @tparam T 削除するコンポーネントの型。デフォルトは Component です。
	 * @param component 削除するコンポーネントへのポインタ。
	 */
	template<typename T = Component>
	inline void RemoveComponent(T* component)
	{
		m_Components.remove_if([component](Component* comp) { return comp == component; });
		delete component;
	}

	/**
	 * @brief 子アクターを作成し、子アクターリストに追加します。
	 * @tparam T 作成する子アクターの型。デフォルトは Actor です。
	 * @return 作成され初期化された子アクターへのポインタ。
	 */
	template<typename T = Actor>
	inline T* AddChildActor()
	{
		T* actor = new T();
		m_ChildActors.emplace_back(actor);
		actor->Init();
		return actor;
	}

	/**
	 * @brief 子アクターリストから指定された型の子アクターを取得します。
	 * @tparam T 取得する子アクターの型。デフォルトは Actor です。
	 * @return 指定された型の最初の子アクターへのポインタ。見つからない場合は nullptr を返します。
	 */
	template<typename T = Actor>
	inline T* GetChildActor()
	{
		for (Actor* actor : m_ChildActors)
		{
			T* castedActor = dynamic_cast<T*>(actor);
			if (castedActor)
			{
				return castedActor;
			}
		}
		return nullptr;
	}

	/**
	 * @brief マテリアルを取得します。
	 * @return マテリアルへのポインタ。
	 */
	inline Material* GetMaterial() { return m_pMaterial; }

	/**
	 * @brief Transformへの参照を取得します。
	 * @return m_Transformへの参照。
	 */
	inline Transform& GetTransform() { return m_Transform; }

	/**
	 * @brief タグを取得します。
	 * @return タグへの const 参照。
	 */
	inline const Tags& GetTag() { return m_Tag; }

protected:
	virtual void OnInit() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnFixedUpdate() {}
	virtual void OnUninit() = 0;

	std::list<Component*> m_Components{};	// コンポーネントリスト
	std::vector<Actor*> m_ChildActors{};	// 子アクターリスト

	Transform m_Transform{};	// 変換情報
	Tags m_Tag{};				// アクターのタグ
	Material* m_pMaterial{};	// マテリアル
};
