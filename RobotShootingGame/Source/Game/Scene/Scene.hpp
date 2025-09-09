/*+===================================================================
	File: Scene.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/02 14:54:10 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "../Object/Object.hpp"
#include "../Camera/Camera.hpp"
#include "../../Utility/Sprite/Sprite.hpp"
#include "../../Utility/Model/Model.hpp"
#include "../../Utility/Line/Line.hpp"

/**
 * @brief Sceneクラス
 */
class Scene
{
public:
	/**
	 * コンストラクタ
	 */
	Scene() = default;

	/**
	 * デストラクタ
	 */
	~Scene() = default;

	void Init();
	void Update();
	void Draw();
	void UnInit();

private:
	Object* m_pObject{}; // Object
	Camera* m_pCamera{}; // カメラ
	Sprite* m_pSprite{}; // スプライト
	Model* m_pModel{}; // モデル
	Line* m_pLine{}; // ライン
};
