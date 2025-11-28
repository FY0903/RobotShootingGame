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
constexpr float DEFAULT_TICKS_PER_SECOND = 25.0f;	// デフォルトのティック毎秒

void SkeletalAnimator::Init(Model* pModel)
{
	m_pModel = pModel;
}

void SkeletalAnimator::Update()
{
	// 再生中のアニメーションがなければ終了
	if (!m_pPlayAnimation || !m_pModel) return;

	aiAnimation* pAnimation = m_pPlayAnimation->GetAnimation(0);
	if (!pAnimation) return;	// アニメーションが存在しなければ終了

	// 経過時間を取得 TODO:後でUtilityに移す
	static auto lastTime = std::chrono::steady_clock::now();
	auto now = std::chrono::steady_clock::now();
	std::chrono::duration<float> delta = now - lastTime;
	lastTime = now;
	float deltaSeconds = delta.count();

	// アニメーション時間をticks単位で更新し、durationでループ
	m_animeTimeTicks += deltaSeconds * m_ticksPerSecond;
	float animationTime = m_animationDuration > 0.0 ? fmod(m_animeTimeTicks, m_animationDuration) : m_animeTimeTicks;

	std::unordered_map<std::string, Model::Bone>& bones = m_pModel->GetBones();
	for (unsigned int i = 0; i < pAnimation->mNumChannels; ++i)
	{
		// アニメーションチャネルを取得
		aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
		if (!pNodeAnim) continue;

		// 対応するボーンを取得
		auto it = bones.find(pNodeAnim->mNodeName.C_Str());
		if (it == bones.end()) continue;
		Model::Bone& pBone = it->second;

		// 位置（線形補間）
		DirectX::SimpleMath::Vector3 interpPos = DirectX::SimpleMath::Vector3::Zero;
		if (pNodeAnim->mNumPositionKeys > 0)
		{
			// 位置キーのインデックスを取得
			unsigned int posIndex = FindKeyIndex(pNodeAnim->mPositionKeys, pNodeAnim->mNumPositionKeys, animationTime);
			unsigned int nextPosIndex = posIndex + 1 < pNodeAnim->mNumPositionKeys ? posIndex + 1 : posIndex;
			
			// キーの時間と値を取得
			double t0 = pNodeAnim->mPositionKeys[posIndex].mTime;
			double t1 = pNodeAnim->mPositionKeys[nextPosIndex].mTime;
			aiVector3D v0 = pNodeAnim->mPositionKeys[posIndex].mValue;
			aiVector3D v1 = pNodeAnim->mPositionKeys[nextPosIndex].mValue;
			
			// 補間係数を計算
			double factor = (t1 - t0) > ai_epsilon ? (animationTime - t0) / (t1 - t0) : 0.0;
			factor = std::clamp(factor, 0.0, 1.0);	// 0.0~1.0に制限

			DirectX::SimpleMath::Vector3 p0{ v0.x, v0.y, v0.z };
			DirectX::SimpleMath::Vector3 p1{ v1.x, v1.y, v1.z };
			interpPos = p0 + (p1 - p0) * static_cast<float>(factor);	// 線形補間
		}

		// スケール（線形補間）
		DirectX::SimpleMath::Vector3 interpScale = DirectX::SimpleMath::Vector3::One;
		if (pNodeAnim->mNumScalingKeys > 0)
		{
			// スケーリングキーのインデックスを取得
			unsigned int sIndex = FindKeyIndex(pNodeAnim->mScalingKeys, pNodeAnim->mNumScalingKeys, animationTime);
			unsigned int nextSIndex = sIndex + 1 < pNodeAnim->mNumScalingKeys ? sIndex + 1 : sIndex;
			
			// キーの時間と値を取得
			double t0 = pNodeAnim->mScalingKeys[sIndex].mTime;
			double t1 = pNodeAnim->mScalingKeys[nextSIndex].mTime;
			aiVector3D s0 = pNodeAnim->mScalingKeys[sIndex].mValue;
			aiVector3D s1 = pNodeAnim->mScalingKeys[nextSIndex].mValue;
			
			// 補間係数を計算
			double factor = (t1 - t0) > ai_epsilon ? (animationTime - t0) / (t1 - t0) : 0.0;
			factor = std::clamp(factor, 0.0, 1.0);	// 0.0~1.0に制限

			DirectX::SimpleMath::Vector3 S0{ s0.x, s0.y, s0.z };
			DirectX::SimpleMath::Vector3 S1{ s1.x, s1.y, s1.z };
			interpScale = S0 + (S1 - S0) * static_cast<float>(factor);	// 線形補間
		}

		// 回転（球面線形補間：Slerp）
		DirectX::SimpleMath::Quaternion interpRot = DirectX::SimpleMath::Quaternion::Identity;
		if (pNodeAnim->mNumRotationKeys > 0)
		{
			// 回転キーのインデックスを取得
			unsigned int rIndex = FindKeyIndex(pNodeAnim->mRotationKeys, pNodeAnim->mNumRotationKeys, animationTime);
			unsigned int nextRIndex = rIndex + 1 < pNodeAnim->mNumRotationKeys ? rIndex + 1 : rIndex;
			
			// キーの時間と値を取得
			double t0 = pNodeAnim->mRotationKeys[rIndex].mTime;
			double t1 = pNodeAnim->mRotationKeys[nextRIndex].mTime;
			aiQuaternion q0 = pNodeAnim->mRotationKeys[rIndex].mValue;
			aiQuaternion q1 = pNodeAnim->mRotationKeys[nextRIndex].mValue;
			
			// 補間係数を計算
			double factor = (t1 - t0) > ai_epsilon ? (animationTime - t0) / (t1 - t0) : 0.0;
			factor = std::clamp(factor, 0.0, 1.0);

			DirectX::SimpleMath::Quaternion Q0{ q0.x, q0.y, q0.z, q0.w };
			DirectX::SimpleMath::Quaternion Q1{ q1.x, q1.y, q1.z, q1.w };
			interpRot = DirectX::SimpleMath::Quaternion::Slerp(Q0, Q1, static_cast<float>(factor));	// 球面線形補間
			interpRot.Normalize();
		}

		// ボーン変換を適用
		Transform boneTransform{};
		boneTransform.Position = interpPos;
		boneTransform.Rotation = interpRot;
		boneTransform.Scale = interpScale;
		pBone.AnimationMatrix = boneTransform.GetWorldMatrix();
	}

	UpdateBoneMatrix(m_pModel->GetScene()->mRootNode, DirectX::XMMatrixIdentity());

	std::vector<DirectX::XMMATRIX>& boneMatCB = m_pModel->GetBoneMatCB();
	for (auto& bone : bones)
	{
		boneMatCB[bone.second.index] = bone.second.Matrix;
	}

	++m_flameCount;
}

void SkeletalAnimator::Draw()
{
}

void SkeletalAnimator::Uninit()
{
	m_pPlayAnimation = nullptr;

	m_pModel = nullptr;
}

void SkeletalAnimator::PlayAnimation(Animation* pAnimation)
{
	m_pPlayAnimation = pAnimation;
	m_animeTimeTicks = 0.0f;
	m_flameCount = 0;

	// アニメーション情報を取得
	aiAnimation* pAiAnimation = m_pPlayAnimation->GetAnimation(0);
	m_ticksPerSecond = static_cast<float>((pAiAnimation->mTicksPerSecond != 0.0) ? pAiAnimation->mTicksPerSecond : DEFAULT_TICKS_PER_SECOND);
	m_animationDuration = static_cast<float>(pAiAnimation->mDuration > 0.0 ? pAiAnimation->mDuration : 0.0);
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

unsigned int SkeletalAnimator::FindKeyIndex(const aiVectorKey* pKeys, unsigned int keyCount, float animationTime)
{
	if (keyCount == 0) return 0;	// 安全対策
	if (keyCount == 1) return 0;	// キーが1つしかない場合は常に0を返す

	// キー配列を走査して、現在のアニメーション時間に対応するキーインデックスを見つける
	for (unsigned int i = 0; i < keyCount - 1; ++i)
	{
		// 次のキーの時間を超えたら現在のインデックスを返す
		if (animationTime < pKeys[i + 1].mTime)
		{
			return i;
		}
	}
	return keyCount - 2;	// 最後のキーの前のインデックスを返す
}

unsigned int SkeletalAnimator::FindKeyIndex(const aiQuatKey* pKeys, unsigned int keyCount, float animationTime)
{
	if (keyCount == 0) return 0;	// 安全対策
	if (keyCount == 1) return 0;	// キーが1つしかない場合は常に0を返す

	// キー配列を走査して、現在のアニメーション時間に対応するキーインデックスを見つける
	for (unsigned int i = 0; i < keyCount - 1; ++i)
	{
		// 次のキーの時間を超えたら現在のインデックスを返す
		if (animationTime < pKeys[i + 1].mTime)
		{
			return i;
		}
	}
	return keyCount - 2;	// 最後のキーの前のインデックスを返す
}
