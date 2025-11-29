/*+===================================================================
	File: SkeletalAnimator.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/26 13:49:38 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/Compornent/Component.hpp"
#include "Utility/Animation/Animation.hpp"
#include "Utility/Model/Model.hpp"
#include <unordered_map>
#include <DirectXMath.h>

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

	void Init(Model* pModel);
	void Update() override final;
	void Draw() override final;
	void Uninit() override final;

	void PlayAnimation(Animation* pAnimation);
	void BlendAnimation(Animation* pBlendAnimation, float blendTime);
	void StopAnimation();

private:
	void UpdateSingleBoneAnimation();
	void UpdateBlendBoneAnimation();

	void UpdateBoneMatrix(const aiNode* node, DirectX::XMMATRIX matrix);
	unsigned int FindKeyIndex(const aiVectorKey* pKeys, unsigned int keyCount, float animationTime);
	unsigned int FindKeyIndex(const aiQuatKey* pKeys, unsigned int keyCount, float animationTime);
	aiNodeAnim* FindChannel(aiAnimation* anim, const std::string& name);

	Model* m_pModel{};

	float m_ticksPerSecond{};
	float m_animationTime{};
	float m_animeTimeTicks{};
	float m_animationDuration{};
	float m_blendTime{};

	// ブレンド用タイミング（追加）
	float m_blendTicksPerSecond{};        // ブレンド先アニメの ticks/sec
	float m_blendAnimationDuration{};     // ブレンド先アニメの duration
	float m_blendAnimeTimeTicks{};        // ブレンド先アニメの現在ticks
	float m_blendElapsedSeconds{};        // ブレンド経過秒

	Animation* m_pPlayAnimation{};
	Animation* m_pBlendAnimation{};
};
