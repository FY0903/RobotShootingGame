/*+===================================================================
	File: ShadowMapManager.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2026/01/20 12:56:02 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/ShadowMap/ShadowMap.hpp"

/**
 * @brief ShadowMapManagerクラス
 */
class ShadowMapManager : public Singleton<ShadowMapManager>
{
public:
	/**
	 * @brief シャドウマップを作成します。
	 * @param pLight ライトコンポーネントへのポインタ
	 */
	void CreateShadowMap(Light* pLight, ShadowMap::Quality quality = ShadowMap::MIDDLE);

	/**
	 * @brief すべてのシャドウマップを描画します。
	 */
	void DrawAllShadowMaps(std::vector<Render::RenderItem> renderItem);

	/**
	 * @brief 指定されたライトコンポーネントに関連付けられたシャドウマップを破棄します。
	 * @param pLight ライトコンポーネントへのポインタ
	 */
	void DestroyShadowMap(Light* pLight);

	/**
	 * @brief すべてのシャドウマップを破棄します。
	 */
	void DestroyAllShadowMaps();

	/**
	 * @brief ShadowMap オブジェクトへのポインタを格納した std::vector への参照を取得します。
	 * @return std::vector<ShadowMap*>&（ShadowMap* を要素とするベクター）への参照。
	 */
	inline std::vector<ShadowMap*>& GetShadowMaps() { return m_pShadowMaps; }

private:
	friend class Singleton<ShadowMapManager>;

	/**
	 * コンストラクタ
	 */
	ShadowMapManager() = default;

	/**
	 * デストラクタ
	 */
	~ShadowMapManager();

	std::vector<ShadowMap*> m_pShadowMaps{};			// シャドウマップ配列
};
