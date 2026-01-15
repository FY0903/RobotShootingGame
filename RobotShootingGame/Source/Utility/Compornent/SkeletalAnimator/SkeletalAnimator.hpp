/*+===================================================================
	File: SkeletalAnimator.hpp
	Summary: SkeletalAnimatorクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/26 13:49 初回作成
			26/01/05 18:06 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/Compornent/Component.hpp"
#include "Utility/Animation/Animation.hpp"
#include "Utility/Model/Model.hpp"

// ==============================
//	前方宣言
// ==============================
struct aiNode;
struct aiVectorKey;
struct aiQuatKey;

/**
 * @brief SkeletalAnimatorクラス
 */
class SkeletalAnimator : public Component
{
public:
	using Component::Component;

	/**
	 * コンストラクタ
	 */
	SkeletalAnimator() = default;

	/**
	 * デストラクタ
	 */
	~SkeletalAnimator() = default;

	/**
	 * @brief Model オブジェクトを初期化します。
	 * @param pModel 初期化対象の Model を指すポインタ。
	 */
	void Init(Model* pModel);

	/**
	 * @brief 更新処理
	 */
	void Update() override final;

	/**
	 * @brief 描画処理
	 */
	void Draw() override final {}

	/**
	 * @brief 終了処理
	 */
	void Uninit() override final;

	/**
	 * @brief 指定したアニメーションを再生します。
	 * @param pAnimation 再生するアニメーションを指すポインタ。
	 */
	void PlayAnimation(Animation* pAnimation);

	/**
	 * @brief 指定したアニメーションにブレンドします。
	 * @param pBlendAnimation ブレンド先のアニメーションを指すポインタ。
	 * @param blendTime ブレンドにかける時間（秒）。
	 */
	void BlendAnimation(Animation* pBlendAnimation, float blendTime);

	/**
	 * @brief アニメーションを停止します。
	 */
	void StopAnimation();

private:
	/**
	 * @brief 単一アニメーションのボーン更新処理
	 */
	void UpdateSingleBoneAnimation();

	/**
	 * @brief ブレンドアニメーションのボーン更新処理
	 */
	void UpdateBlendBoneAnimation();

	/**
	 * @brief ボーン行列を更新します。
	 * @param node 対象のノードを指すポインタ。
	 * @param matrix 更新する行列。
	 */
	void UpdateBoneMatrix(const aiNode* node, DirectX::XMMATRIX matrix);

	/**
	 * @brief 指定したアニメーション時刻に対応するキーのインデックスを返します。
	 * @param pKeys aiVectorKey 構造体の配列へのポインタ。検索対象のキー列の先頭を指します。
	 * @param keyCount 配列内のキー数（要素数）。
	 * @param animationTime 検索するアニメーション時刻（時間値）。
	 * @return 指定した時刻に対応するキーのインデックス（通常は 0 から keyCount - 1 の範囲）。
	 */
	unsigned int FindKeyIndex(const aiVectorKey* pKeys, unsigned int keyCount, float animationTime);
	
	/**
	 * @brief 指定されたアニメーション時間に対応する回転キー（aiQuatKey）のインデックスを検索して返します。
	 * @param pKeys キー配列の先頭要素へのポインタ（aiQuatKey の配列）。
	 * @param keyCount 配列内のキーの数（要素数）。
	 * @param animationTime 検索対象のアニメーション時間（通常は秒やフレーム時間）。
	 * @return 該当するキーのインデックス（0 から keyCount - 1 の範囲の unsigned int）。
	 */
	unsigned int FindKeyIndex(const aiQuatKey* pKeys, unsigned int keyCount, float animationTime);
	
	/**
	 * @brief 指定した名前のアニメーションチャネルを取得します。
	 * @param anim アニメーションを指すポインタ。
	 * @param name チャネルの名前。
	 * @return 指定した名前のチャネルを指すポインタ。見つからない場合は nullptr を返します。
	 */
	aiNodeAnim* FindChannel(aiAnimation* anim, const std::string& name);

	Model* m_pModel{};					// モデルへのポインタ

	float m_ticksPerSecond{};			// アニメの ticks/sec
	float m_animationTime{};			// アニメの経過秒
	float m_animeTimeTicks{};			// アニメの現在ticks
	float m_animationDuration{};		// アニメの duration
	float m_blendTime{};				// ブレンド時間

	float m_blendTicksPerSecond{};		// ブレンド先アニメの ticks/sec
	float m_blendAnimationDuration{};	// ブレンド先アニメの duration
	float m_blendAnimeTimeTicks{};		// ブレンド先アニメの現在ticks
	float m_blendElapsedSeconds{};		// ブレンド経過秒

	Animation* m_pPlayAnimation{};		// 再生中のアニメーションへのポインタ
	Animation* m_pBlendAnimation{};		// ブレンド先のアニメーションへのポインタ
};
