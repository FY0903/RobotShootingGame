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
};
