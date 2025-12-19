/*+===================================================================
	File: LightManager.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/19 12:49:59 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================

/**
 * @brief LightManagerクラス
 */
class LightManager : public Singleton<LightManager>
{
public:
	void RegisterLight(class Light* pLight);

	const std::vector<class Light*>& GetLights() const;

private:
	friend class Singleton<LightManager>;

	/**
	 * コンストラクタ
	 */
	LightManager() = default;

	/**
	 * デストラクタ
	 */
	~LightManager() = default;

	std::vector<class Light*> m_Lights{};	// ライトコンポーネント配列
};
