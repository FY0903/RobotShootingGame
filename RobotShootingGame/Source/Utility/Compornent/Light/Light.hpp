/*+===================================================================
	File: Light.hpp
	Summary: Lightクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/18 14:54 初回作成
			26/01/15 17:52 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/Compornent/Component.hpp"

/**
 * @brief Lightクラス
 */
class Light : public Component
{
public:
	/**
	 * @brief ライトの種類
	 */
	enum Type
	{
		DIRECTIONAL,	// 平行光源（太陽光源）
		POINT,			// 点光源
		SPOT,			// スポットライト
		MAX,
	};

	using Component::Component;

	/**
	 * @brief 指定されたパラメータでオブジェクトを初期化します。
	 * @param type 初期化する対象のタイプを指定します。
	 * @param color RGBA 形式の色を指定します。
	 * @param range 影響範囲または到達距離を指定します。
	 * @param angle 角度をラジアン単位で指定します。
	 */
	void Init(Type type,
		DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		float range = 1.0f, float angle = DirectX::XMConvertToRadians(60.0f));

	/**
	 * @brief 更新処理
	 */
	void Update() override final {}

	/**
	 * @brief 描画処理
	 */
	void Draw() override final {}

	/**
	 * @brief 終了処理
	 */
	void Uninit() override final {}

	/**
	 * @brief オブジェクトの型を返す const メンバ関数。
	 * @return Type 型の値。メンバ変数 m_Type に格納されている型を返します。
	 */
	inline Type GetType() const { return m_Type; }

	/**
	 * @brief オブジェクトの位置を取得する const メンバ関数。
	 * @return DirectX::XMFLOAT3 型の値。オブジェクトの位置を表します。
	 */
	DirectX::XMFLOAT3 GetPosition() const;

	/**
	 * @brief ライトの届く距離を取得する const メンバ関数。
	 * @return float 型の値。メンバ変数 m_Range に格納されているライトの届く距離を返します。
	 */
	inline float GetRange() const { return m_Range; }

	/**
	 * @brief スポットライトの角度を取得する const メンバ関数。
	 * @return float 型の値。メンバ変数 m_Angle に格納されているスポットライトの角度を返します。
	 */
	inline float GetAngle() const { return m_Angle; }

	/**
	 * @brief オブジェクトの向きを取得する const メンバ関数。
	 * @return DirectX::XMFLOAT3 型の値。オブジェクトの向きを表します。
	 */
	DirectX::XMFLOAT3 GetDirection() const;

	/**
	 * @brief オブジェクトの色を取得する const メンバ関数。
	 * @return DirectX::XMFLOAT4 型の値。メンバ変数 m_Color に格納されている色を返します。
	 */
	inline DirectX::XMFLOAT4 GetColor() const { return m_Color; }

	/**
	 * @brief オブジェクトの色を設定するメンバ関数。
	 * @param color DirectX::XMFLOAT4 型の値。設定する色を指定します。
	 */
	inline void SetColor(const DirectX::XMFLOAT4& color) { m_Color = color; }

private:
	Type m_Type{};					// ライトの種類
	DirectX::XMFLOAT4 m_Color{};	// ライトの色
	float m_Range{};				// ライトの届く距離
	float m_Angle{};				// スポットライトの角度
};
