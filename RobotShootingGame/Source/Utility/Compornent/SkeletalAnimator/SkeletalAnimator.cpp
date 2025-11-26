/*+===================================================================
	File: SkeletalAnimator.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/26 13:49:38 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SkeletalAnimator.hpp"
#include "Utility/Transform/Transform.hpp"
#include <chrono>

// ==============================
//	constcxpr
// ==============================
constexpr float DEFAULT_TICKS_PER_SECOND = 25.0f;

void SkeletalAnimator::Init(Model* pModel)
{
	m_pModel = pModel;
}

void SkeletalAnimator::Update()
{
	if (!m_pPlayAnimation || !m_pModel) return;

	// アニメーション時間管理（実時間に基づく補間を行う）
	static double animeTimeTicks{}; // アニメーションの経過時間（ticks単位）
	static auto lastTime = std::chrono::steady_clock::now();

	auto now = std::chrono::steady_clock::now();
	std::chrono::duration<double> delta = now - lastTime;
	lastTime = now;
	double deltaSeconds = delta.count();

	DirectX::XMMATRIX rootMat = DirectX::XMMatrixIdentity();
	Transform transform{};

	rootMat *= transform.GetWorldMatrix();

	aiAnimation* pAnimation = m_pPlayAnimation->GetAnimation(0);
	if (!pAnimation) return;

	std::unordered_map<std::string, Model::Bone>& bones = m_pModel->GetBones();

	// アニメーション時間をticks単位で更新し、durationでループ
	animeTimeTicks += deltaSeconds * m_ticksPerSecond;
	double animationDuration = pAnimation->mDuration > 0.0 ? pAnimation->mDuration : 0.0;
	double animationTime = animationDuration > 0.0 ? fmod(animeTimeTicks, animationDuration) : animeTimeTicks;

	// ヘルパー: キー配列から現在のキーインデックスを探す
	auto FindKeyIndex = [&](const auto* keys, unsigned int keyCount) -> unsigned int
	{
		if (keyCount == 0) return 0;
		if (keyCount == 1) return 0;
		for (unsigned int i = 0; i < keyCount - 1; ++i)
		{
			if (animationTime < keys[i + 1].mTime)
			{
				return i;
			}
		}
		return keyCount - 2;
	};

	for (unsigned int i = 0; i < pAnimation->mNumChannels; ++i)
	{
		aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
		if (!pNodeAnim) continue;

		auto it = bones.find(pNodeAnim->mNodeName.C_Str());
		if (it == bones.end()) continue;
		Model::Bone& pBone = it->second;

		// 位置（線形補間）
		DirectX::SimpleMath::Vector3 interpPos = DirectX::SimpleMath::Vector3::Zero;
		if (pNodeAnim->mNumPositionKeys > 0)
		{
			unsigned int posIndex = FindKeyIndex(pNodeAnim->mPositionKeys, pNodeAnim->mNumPositionKeys);
			unsigned int nextPosIndex = posIndex + 1 < pNodeAnim->mNumPositionKeys ? posIndex + 1 : posIndex;
			double t0 = pNodeAnim->mPositionKeys[posIndex].mTime;
			double t1 = pNodeAnim->mPositionKeys[nextPosIndex].mTime;
			aiVector3D v0 = pNodeAnim->mPositionKeys[posIndex].mValue;
			aiVector3D v1 = pNodeAnim->mPositionKeys[nextPosIndex].mValue;
			double factor = (t1 - t0) > 1e-6 ? (animationTime - t0) / (t1 - t0) : 0.0;
			factor = std::clamp(factor, 0.0, 1.0);
			DirectX::SimpleMath::Vector3 p0{ v0.x, v0.y, v0.z };
			DirectX::SimpleMath::Vector3 p1{ v1.x, v1.y, v1.z };
			interpPos = p0 + (p1 - p0) * static_cast<float>(factor);
		}

		// スケール（線形補間）
		DirectX::SimpleMath::Vector3 interpScale = DirectX::SimpleMath::Vector3::One;
		if (pNodeAnim->mNumScalingKeys > 0)
		{
			unsigned int sIndex = FindKeyIndex(pNodeAnim->mScalingKeys, pNodeAnim->mNumScalingKeys);
			unsigned int nextSIndex = sIndex + 1 < pNodeAnim->mNumScalingKeys ? sIndex + 1 : sIndex;
			double t0 = pNodeAnim->mScalingKeys[sIndex].mTime;
			double t1 = pNodeAnim->mScalingKeys[nextSIndex].mTime;
			aiVector3D s0 = pNodeAnim->mScalingKeys[sIndex].mValue;
			aiVector3D s1 = pNodeAnim->mScalingKeys[nextSIndex].mValue;
			double factor = (t1 - t0) > 1e-6 ? (animationTime - t0) / (t1 - t0) : 0.0;
			factor = std::clamp(factor, 0.0, 1.0);
			DirectX::SimpleMath::Vector3 S0{ s0.x, s0.y, s0.z };
			DirectX::SimpleMath::Vector3 S1{ s1.x, s1.y, s1.z };
			interpScale = S0 + (S1 - S0) * static_cast<float>(factor);
		}

		// 回転（球面線形補間：Slerp）
		DirectX::SimpleMath::Quaternion interpRot = DirectX::SimpleMath::Quaternion::Identity;
		if (pNodeAnim->mNumRotationKeys > 0)
		{
			unsigned int rIndex = FindKeyIndex(pNodeAnim->mRotationKeys, pNodeAnim->mNumRotationKeys);
			unsigned int nextRIndex = rIndex + 1 < pNodeAnim->mNumRotationKeys ? rIndex + 1 : rIndex;
			double t0 = pNodeAnim->mRotationKeys[rIndex].mTime;
			double t1 = pNodeAnim->mRotationKeys[nextRIndex].mTime;
			aiQuaternion q0 = pNodeAnim->mRotationKeys[rIndex].mValue;
			aiQuaternion q1 = pNodeAnim->mRotationKeys[nextRIndex].mValue;
			double factor = (t1 - t0) > 1e-6 ? (animationTime - t0) / (t1 - t0) : 0.0;
			factor = std::clamp(factor, 0.0, 1.0);

			DirectX::SimpleMath::Quaternion Q0{ q0.x, q0.y, q0.z, q0.w };
			DirectX::SimpleMath::Quaternion Q1{ q1.x, q1.y, q1.z, q1.w };
			interpRot = DirectX::SimpleMath::Quaternion::Slerp(Q0, Q1, static_cast<float>(factor));
			interpRot.Normalize();
		}

		// ボーン変換を適用
		Transform boneTransform{};
		boneTransform.Position = interpPos;
		boneTransform.Rotation = interpRot;
		boneTransform.Scale = interpScale;
		pBone.AnimationMatrix = boneTransform.GetWorldMatrix();
	}

	UpdateBoneMatrix(m_pModel->GetScene()->mRootNode, rootMat);

	std::vector<DirectX::XMMATRIX>& boneMatCB = m_pModel->GetBoneMatCB();
	for (auto& bone : bones)
	{
		boneMatCB[bone.second.index] = bone.second.Matrix;
	}
}

void SkeletalAnimator::Draw()
{
}

void SkeletalAnimator::Uninit()
{
	for (auto& animation : m_Animations)
	{
		animation.second = nullptr;
	}
	m_Animations.clear();

	m_pPlayAnimation = nullptr;

	m_pModel = nullptr;
}

void SkeletalAnimator::AddAnimation(const std::string& name, Animation* animation)
{
	m_Animations[name] = animation;
}

void SkeletalAnimator::PlayAnimation(const std::string& name)
{
	m_pPlayAnimation = m_Animations[name];
	aiAnimation* pAnimation = m_pPlayAnimation->GetAnimation(0);
	m_ticksPerSecond = (pAnimation->mTicksPerSecond != 0.0) ? pAnimation->mTicksPerSecond : DEFAULT_TICKS_PER_SECOND;
}

void SkeletalAnimator::StopAnimation()
{
	m_pPlayAnimation = nullptr;
}

void SkeletalAnimator::UpdateBoneMatrix(const aiNode* node, DirectX::XMMATRIX matrix)
{
	Model::Bone& bone = m_pModel->GetBones()[node->mName.C_Str()];

	bone.Matrix = bone.OffsetMatrix * bone.AnimationMatrix * matrix;

	for (size_t i = 0; i < node->mNumChildren; ++i)
	{
		UpdateBoneMatrix(node->mChildren[i], bone.AnimationMatrix * matrix);
	}
}
