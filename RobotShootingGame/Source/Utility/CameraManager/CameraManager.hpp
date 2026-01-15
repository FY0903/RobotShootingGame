/*+===================================================================
	File: CameraManager.hpp
	Summary: カメラ管理クラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/23 16:39 初回作成
			26/01/14 16:50 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/Compornent/Camera/Camera.hpp"

/**
 * @brief CameraManagerクラス
 */
class CameraManager : public Singleton<CameraManager>
{
public:
	/**
	 * @brief 指定した名前でカメラを登録します。
	 * @param name 登録するカメラの名前。
	 * @param pCamera 登録するカメラへのポインタ。
	 */
	void RegisterCamera(const std::string& name, Camera* pCamera);

	/**
	 * @brief 指定した名前のカメラをメインカメラとして設定します。
	 * @param name メインカメラに設定するカメラの名前（識別子）。
	 */
	void SetMainCamera(const std::string& name);

	/**
	 * @brief メインカメラへのポインタを取得します。
	 * @return メインカメラを指す Camera*。メインカメラが存在しない場合は nullptr を返す可能性があります。
	 */
	Camera* GetMainCamera() const { return m_pMainCamera; }

	/**
	 * @brief 現在のメインカメラの位置を取得します。
	 * @return メインカメラの位置を表す DirectX::XMFLOAT3 を返します。
	 */
	DirectX::XMFLOAT3 GetMainCameraPosition() const;

	/**
	 * @brief ワールド変換行列からメインカメラまでの距離を計算します。
	 * @param worldMat オブジェクトのワールド変換を表す DirectX::XMFLOAT4X4 の定数参照。位置成分を用いてメインカメラとの距離を算出します。
	 * @return メインカメラまでの距離を表す浮動小数点値（float）。
	 */
	float CalculateDistanceToMainCamera(const DirectX::XMFLOAT4X4& worldMat) const;

private:
	friend class Singleton<CameraManager>;

	/**
	 * コンストラクタ
	 */
	CameraManager() = default;

	/**
	 * デストラクタ
	 */
	~CameraManager();

	std::unordered_map<std::string, Camera*> m_pCameraData{};	// 登録されたカメラデータ
	Camera* m_pMainCamera{};        // メインカメラ
};
