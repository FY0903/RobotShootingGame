/*+===================================================================
	File: LightManager.hpp
	Summary: LightManagerクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/19 12:49 初回作成
			26/01/15 18:51 コメント記載
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
	/**
	 * @brief ライトオブジェクトを登録します。
	 * @param pLight 登録する Light オブジェクトへのポインタ。
	 */
	void RegisterLight(class Light* pLight);

	/**
	 * @brief Light オブジェクトへのポインタを格納した std::vector への const 参照を取得します。
	 * @return const std::vector<class Light*>&（Light* を要素とするベクター）への参照。
	 */
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
