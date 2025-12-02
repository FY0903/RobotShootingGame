/*+===================================================================
	File: Animation.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/25 14:38:44 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

// ==============================
//	前方宣言
// ==============================
struct aiScene;
struct aiAnimation;

/**
 * @brief Animationクラス
 */
class Animation
{
public:
	/**
	 * コンストラクタ
	 */
	Animation() = default;

	/**
	 * デストラクタ
	 */
	~Animation() = default;

	HRESULT Load(const std::string& fileName);

	aiAnimation* GetAnimation(int index);
	inline const aiScene* GetAnimations() const { return m_Animations; }

private:
	Assimp::Importer m_Importer;
	const aiScene* m_Animations{};
};
