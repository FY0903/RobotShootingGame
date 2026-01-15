/*+===================================================================
	File: Animation.hpp
	Summary: アニメーションクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/25 14:38 初回作成
			26/01/15 16:39 コメント記載
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

	/**
	 * @brief 指定したファイルからデータを読み込みます。
	 * @param fileName 読み込むファイルのパスまたは名前。
	 * @return HRESULT。成功した場合は S_OK を返し、失敗した場合はエラーを示す HRESULT コードを返します。
	 */
	HRESULT Load(const std::string& fileName);

	/**
	 * @brief 指定したインデックスに対応するアニメーションを取得します。
	 * @param index 取得するアニメーションのインデックス。
	 * @return 指定したインデックスの aiAnimation へのポインタ。該当するアニメーションが存在しない場合は nullptr を返すことがあります。
	 */
	aiAnimation* GetAnimation(int index);

	/**
	 * @brief オブジェクトが保持するアニメーションのシーンへのポインタを取得します。
	 * @return アニメーションのシーンを指す const aiScene*。アニメーションが存在しない場合は nullptr を返すことがあり、所有権は呼び出し元に移動しません。
	 */
	inline const aiScene* GetAnimations() const { return m_Animations; }

private:
	Assimp::Importer m_Importer;	// インポーターオブジェクト
	const aiScene* m_Animations{};	// アニメーションのシーンポインタ
};
