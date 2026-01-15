/*+===================================================================
	File: Component.hpp
	Summary: Compornentクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/14 14:34 初回作成
			26/01/15 18:13 コメント記載
===================================================================+*/
#pragma once

/**
 * @brief Compornentクラス
 */
class Component
{
public:
	/**
	 * コンストラクタ
	 */
	Component() = default;

	/**
	 * @brief 指定された Actor を所有者として Component を初期化するコンストラクタ。
	 * @param owner コンポーネントの所有者となる Actor へのポインタ。
	 */
	Component(class Actor* owner);

	/**
	 * デストラクタ
	 */
	virtual ~Component();

	virtual void Update() = 0;
	virtual void FixedUpdate() {}
	virtual void Draw() = 0;
	virtual void Uninit() = 0;

protected:
	class Actor* m_Owner{};	// 所有者のアクター
};
