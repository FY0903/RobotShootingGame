/*+===================================================================
	File: AnimationManager.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/01 20:16:06 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/Singleton/Singleton.hpp"
#include <string>
#include <unordered_map>

/**
 * @brief AnimationManagerクラス
 */
class AnimationManager : public Singleton<AnimationManager>
{
public:
	class Animation* LoadAnimation(const std::string& animePath);

private:
	friend class Singleton<AnimationManager>;

	/**
	 * コンストラクタ
	 */
	AnimationManager() = default;

	/**
	 * デストラクタ
	 */
	~AnimationManager();


	std::unordered_map<std::string, class Animation*> m_animationMap;	// アニメーションマップ
};
