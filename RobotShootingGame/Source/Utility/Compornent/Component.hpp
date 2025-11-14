/*+===================================================================
	File: Component.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/14 14:34:53 初回作成
	（これ以降下に更新日時と更新内容を書く）
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

	Component(class Actor* owner);

	/**
	 * デストラクタ
	 */
	virtual ~Component();

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Uninit() = 0;

protected:
	class Actor* m_Owner{};
};
