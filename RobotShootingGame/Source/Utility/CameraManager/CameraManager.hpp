/*+===================================================================
	File: CameraManager.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/23 16:39:11 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/Singleton/Singleton.hpp"
#include "Utility/Compornent/Camera/Camera.hpp"
#include <unordered_map>

/**
 * @brief CameraManagerクラス
 */
class CameraManager : public Singleton<CameraManager>
{
public:
	void RegisterCamera(const std::string& name, Camera* pCamera);

	void SetMainCamera(const std::string& name);

	Camera* GetMainCamera() const { return m_pMainCamera; }

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

	std::unordered_map<std::string, Camera*> m_pCameraData{};
	Camera* m_pMainCamera{};        // メインカメラ
};
