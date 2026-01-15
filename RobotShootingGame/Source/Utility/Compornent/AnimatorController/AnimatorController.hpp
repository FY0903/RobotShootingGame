/*+===================================================================
	File: AnimatorController.hpp
	Summary: AnimatorControllerクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/27 11:51 初回作成
			26/01/15 16:59 コメント記載
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

	/**
	 * @brief 初期化処理
	 */
	void Init() {};

	/**
	 * @brief 更新処理
	 */
	void Update() override final {};

	/**
	 * @brief 描画処理
	 */
	void Draw() override final {};

	/**
	 * @brief 終了処理
	 */
	void Uninit() override final;

	/**
	 * @brief 指定した名前でアニメーションを追加します。
	 * @param name 追加するアニメーションの名前。
	 * @param animation 追加するアニメーションへのポインタ。
	 */
	void AddAnimation(const std::string& name, Animation* animation);

	/**
	 * @brief 指定した名前に対応する Animation オブジェクトへのポインタを返します。
	 * @param name 取得するアニメーションの名前（const 参照）。
	 * @return 指定した名前に対応する Animation*。該当エントリが存在しない場合はデフォルト値（通常は nullptr）が返されます。
	 */
	inline Animation* GetAnimation(const std::string& name) { return m_Animations[name]; }
	
	/**
	 * @brief 所有する Actor へのポインタを返します（内部メンバ m_Owner の値）。
	 * @return 所有者を指す Actor*（内部メンバ m_Owner の値）。所有者が設定されていない場合は nullptr である可能性があります。
	 */
	inline Actor* GetOwner() { return m_Owner; }
	
private:
	std::unordered_map<std::string, Animation*> m_Animations{};	// アニメーションマップ
};
