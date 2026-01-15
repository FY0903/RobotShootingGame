/*+===================================================================
	File: AnimationManager.hpp
	Summary: アニメーション管理クラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/01 20:16 初回作成
			26/01/15 16:46 コメント記載
===================================================================+*/
#pragma once

/**
 * @brief AnimationManagerクラス
 */
class AnimationManager : public Singleton<AnimationManager>
{
public:
	/**
	 * @brief 指定したパスからアニメーションを読み込み、Animation オブジェクトへのポインタを返します。
	 * @param animePath 読み込むアニメーションファイルのパスを表す文字列（const std::string&）。
	 * @return 読み込んだアニメーションを指す Animation クラスへのポインタを返します。
	 */
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
