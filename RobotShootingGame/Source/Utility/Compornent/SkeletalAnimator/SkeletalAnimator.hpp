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

	void AddAnimation(const std::string& name, Animation* animation);

	void PlayAnimation(const std::string& name);
	void StopAnimation();

private:
	void UpdateBoneMatrix(const aiNode* node, DirectX::XMMATRIX matrix);
	unsigned int FindKeyIndex(const aiVectorKey* pKeys, unsigned int keyCount, float animationTime);
	unsigned int FindKeyIndex(const aiQuatKey* pKeys, unsigned int keyCount, float animationTime);

	Model* m_pModel{};

	float m_ticksPerSecond{};
	float m_animationTime{};
	float m_animeTimeTicks{};
	float m_animationDuration{};
	int m_flameCount{};

	Animation* m_pPlayAnimation{};
	std::unordered_map<std::string, Animation*> m_Animations{};
};
