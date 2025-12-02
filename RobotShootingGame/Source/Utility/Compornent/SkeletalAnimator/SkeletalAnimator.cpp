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

	if (!m_pBlendAnimation)
	{
		// ブレンド中でなければ単一アニメーション更新
		UpdateSingleBoneAnimation();
	}
	else
	{
		// ブレンド中であればブレンドアニメーション更新
		UpdateBlendBoneAnimation();
	}
}

void SkeletalAnimator::Draw()
{
}

void SkeletalAnimator::Uninit()
{
	m_pPlayAnimation = nullptr;
	m_pBlendAnimation = nullptr;

	m_pModel = nullptr;
}

void SkeletalAnimator::PlayAnimation(Animation* pAnimation)
{
	m_pPlayAnimation = pAnimation;
	m_animeTimeTicks = 0.0f;
	m_blendAnimeTimeTicks = 0.0f;
	m_blendElapsedSeconds = 0.0f;

	// アニメーション情報を取得
	aiAnimation* pAiAnimation = m_pPlayAnimation->GetAnimation(0);
	m_ticksPerSecond = static_cast<float>((pAiAnimation->mTicksPerSecond != 0.0) ? pAiAnimation->mTicksPerSecond : DEFAULT_TICKS_PER_SECOND);
	m_animationDuration = static_cast<float>(pAiAnimation->mDuration > 0.0 ? pAiAnimation->mDuration : 0.0);
}

void SkeletalAnimator::BlendAnimation(Animation* pBlendAnimation, float blendTime)
{
	m_pBlendAnimation = pBlendAnimation;
	m_blendTime = blendTime;
	m_blendElapsedSeconds = 0.0f;
	m_blendAnimeTimeTicks = 0.0f;

	// ブレンド先アニメーション情報を取得（ticks/durationを保存）
	if (m_pBlendAnimation)
	{
		aiAnimation* pAiBlend = m_pBlendAnimation->GetAnimation(0);
		m_blendTicksPerSecond = static_cast<float>((pAiBlend->mTicksPerSecond != 0.0) ? pAiBlend->mTicksPerSecond : DEFAULT_TICKS_PER_SECOND);
		m_blendAnimationDuration = static_cast<float>(pAiBlend->mDuration > 0.0 ? pAiBlend->mDuration : 0.0);
	}
}

void SkeletalAnimator::StopAnimation()
{
	m_pPlayAnimation = nullptr;
	m_pBlendAnimation = nullptr;
}

void SkeletalAnimator::UpdateSingleBoneAnimation()
{
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
}

void SkeletalAnimator::UpdateBlendBoneAnimation()
{
	aiAnimation* pPlayAnimation = m_pPlayAnimation->GetAnimation(0);
	aiAnimation* pBlendAnimation = m_pBlendAnimation->GetAnimation(0);
	if (!pPlayAnimation || !pBlendAnimation) return;	// アニメーションが存在しなければ終了

	// 経過時間を取得 TODO:後でUtilityに移す
	static auto lastTime = std::chrono::steady_clock::now();
	auto now = std::chrono::steady_clock::now();
	std::chrono::duration<float> delta = now - lastTime;
	lastTime = now;
	float deltaSeconds = delta.count();

	// 再生中アニメの時間更新
	m_animeTimeTicks += deltaSeconds * m_ticksPerSecond;
	float playTime = m_animationDuration > 0.0 ? fmod(m_animeTimeTicks, m_animationDuration) : m_animeTimeTicks;

	// ブレンド先アニメの時間更新
	m_blendAnimeTimeTicks += deltaSeconds * m_blendTicksPerSecond;
	float blendTimeTicks = m_blendAnimationDuration > 0.0 ? fmod(m_blendAnimeTimeTicks, m_blendAnimationDuration) : m_blendAnimeTimeTicks;

	// ブレンドの進行（0..1）
	m_blendElapsedSeconds += deltaSeconds;
	float alpha = (m_blendTime > 0.0f) ? std::clamp(m_blendElapsedSeconds / m_blendTime, 0.0f, 1.0f) : 1.0f;

	std::unordered_map<std::string, Model::Bone>& bones = m_pModel->GetBones();

	for (auto& [boneName, bone] : bones)
	{
		// 各アニメからボーン変換を取得（存在しない場合はデフォルト）
		aiNodeAnim* pNodePlay = FindChannel(pPlayAnimation, boneName);
		aiNodeAnim* pNodeBlend = FindChannel(pBlendAnimation, boneName);

		// Play アニメからの補間値
		DirectX::SimpleMath::Vector3 posPlay = DirectX::SimpleMath::Vector3::Zero;
		DirectX::SimpleMath::Vector3 scalePlay = DirectX::SimpleMath::Vector3::One;
		DirectX::SimpleMath::Quaternion rotPlay = DirectX::SimpleMath::Quaternion::Identity;

		if (pNodePlay)
		{
			// 位置
			if (pNodePlay->mNumPositionKeys > 0)
			{
				unsigned int posIndex = FindKeyIndex(pNodePlay->mPositionKeys, pNodePlay->mNumPositionKeys, playTime);
				unsigned int nextPosIndex = posIndex + 1 < pNodePlay->mNumPositionKeys ? posIndex + 1 : posIndex;
				double t0 = pNodePlay->mPositionKeys[posIndex].mTime;
				double t1 = pNodePlay->mPositionKeys[nextPosIndex].mTime;
				aiVector3D v0 = pNodePlay->mPositionKeys[posIndex].mValue;
				aiVector3D v1 = pNodePlay->mPositionKeys[nextPosIndex].mValue;
				double factor = (t1 - t0) > ai_epsilon ? (playTime - t0) / (t1 - t0) : 0.0;
				factor = std::clamp(factor, 0.0, 1.0);
				DirectX::SimpleMath::Vector3 P0{ v0.x, v0.y, v0.z }, P1{ v1.x, v1.y, v1.z };
				posPlay = P0 + (P1 - P0) * static_cast<float>(factor);
			}
			// スケール
			if (pNodePlay->mNumScalingKeys > 0)
			{
				unsigned int sIndex = FindKeyIndex(pNodePlay->mScalingKeys, pNodePlay->mNumScalingKeys, playTime);
				unsigned int nextSIndex = sIndex + 1 < pNodePlay->mNumScalingKeys ? sIndex + 1 : sIndex;
				double t0 = pNodePlay->mScalingKeys[sIndex].mTime;
				double t1 = pNodePlay->mScalingKeys[nextSIndex].mTime;
				aiVector3D s0 = pNodePlay->mScalingKeys[sIndex].mValue;
				aiVector3D s1 = pNodePlay->mScalingKeys[nextSIndex].mValue;
				double factor = (t1 - t0) > ai_epsilon ? (playTime - t0) / (t1 - t0) : 0.0;
				factor = std::clamp(factor, 0.0, 1.0);
				DirectX::SimpleMath::Vector3 S0{ s0.x, s0.y, s0.z }, S1{ s1.x, s1.y, s1.z };
				scalePlay = S0 + (S1 - S0) * static_cast<float>(factor);
			}
			// 回転
			if (pNodePlay->mNumRotationKeys > 0)
			{
				unsigned int rIndex = FindKeyIndex(pNodePlay->mRotationKeys, pNodePlay->mNumRotationKeys, playTime);
				unsigned int nextRIndex = rIndex + 1 < pNodePlay->mNumRotationKeys ? rIndex + 1 : rIndex;
				double t0 = pNodePlay->mRotationKeys[rIndex].mTime;
				double t1 = pNodePlay->mRotationKeys[nextRIndex].mTime;
				aiQuaternion q0 = pNodePlay->mRotationKeys[rIndex].mValue;
				aiQuaternion q1 = pNodePlay->mRotationKeys[nextRIndex].mValue;
				double factor = (t1 - t0) > ai_epsilon ? (playTime - t0) / (t1 - t0) : 0.0;
				factor = std::clamp(factor, 0.0, 1.0);
				DirectX::SimpleMath::Quaternion Q0{ q0.x, q0.y, q0.z, q0.w }, Q1{ q1.x, q1.y, q1.z, q1.w };
				rotPlay = DirectX::SimpleMath::Quaternion::Slerp(Q0, Q1, static_cast<float>(factor));
				rotPlay.Normalize();
			}
		}

		// Blend アニメからの補間値
		DirectX::SimpleMath::Vector3 posBlend = DirectX::SimpleMath::Vector3::Zero;
		DirectX::SimpleMath::Vector3 scaleBlend = DirectX::SimpleMath::Vector3::One;
		DirectX::SimpleMath::Quaternion rotBlend = DirectX::SimpleMath::Quaternion::Identity;

		if (pNodeBlend)
		{
			// 位置
			if (pNodeBlend->mNumPositionKeys > 0)
			{
				unsigned int posIndex = FindKeyIndex(pNodeBlend->mPositionKeys, pNodeBlend->mNumPositionKeys, blendTimeTicks);
				unsigned int nextPosIndex = posIndex + 1 < pNodeBlend->mNumPositionKeys ? posIndex + 1 : posIndex;
				double t0 = pNodeBlend->mPositionKeys[posIndex].mTime;
				double t1 = pNodeBlend->mPositionKeys[nextPosIndex].mTime;
				aiVector3D v0 = pNodeBlend->mPositionKeys[posIndex].mValue;
				aiVector3D v1 = pNodeBlend->mPositionKeys[nextPosIndex].mValue;
				double factor = (t1 - t0) > ai_epsilon ? (blendTimeTicks - t0) / (t1 - t0) : 0.0;
				factor = std::clamp(factor, 0.0, 1.0);
				DirectX::SimpleMath::Vector3 P0{ v0.x, v0.y, v0.z }, P1{ v1.x, v1.y, v1.z };
				posBlend = P0 + (P1 - P0) * static_cast<float>(factor);
			}
			// スケール
			if (pNodeBlend->mNumScalingKeys > 0)
			{
				unsigned int sIndex = FindKeyIndex(pNodeBlend->mScalingKeys, pNodeBlend->mNumScalingKeys, blendTimeTicks);
				unsigned int nextSIndex = sIndex + 1 < pNodeBlend->mNumScalingKeys ? sIndex + 1 : sIndex;
				double t0 = pNodeBlend->mScalingKeys[sIndex].mTime;
				double t1 = pNodeBlend->mScalingKeys[nextSIndex].mTime;
				aiVector3D s0 = pNodeBlend->mScalingKeys[sIndex].mValue;
				aiVector3D s1 = pNodeBlend->mScalingKeys[nextSIndex].mValue;
				double factor = (t1 - t0) > ai_epsilon ? (blendTimeTicks - t0) / (t1 - t0) : 0.0;
				factor = std::clamp(factor, 0.0, 1.0);
				DirectX::SimpleMath::Vector3 S0{ s0.x, s0.y, s0.z }, S1{ s1.x, s1.y, s1.z };
				scaleBlend = S0 + (S1 - S0) * static_cast<float>(factor);
			}
			// 回転
			if (pNodeBlend->mNumRotationKeys > 0)
			{
				unsigned int rIndex = FindKeyIndex(pNodeBlend->mRotationKeys, pNodeBlend->mNumRotationKeys, blendTimeTicks);
				unsigned int nextRIndex = rIndex + 1 < pNodeBlend->mNumRotationKeys ? rIndex + 1 : rIndex;
				double t0 = pNodeBlend->mRotationKeys[rIndex].mTime;
				double t1 = pNodeBlend->mRotationKeys[nextRIndex].mTime;
				aiQuaternion q0 = pNodeBlend->mRotationKeys[rIndex].mValue;
				aiQuaternion q1 = pNodeBlend->mRotationKeys[nextRIndex].mValue;
				double factor = (t1 - t0) > ai_epsilon ? (blendTimeTicks - t0) / (t1 - t0) : 0.0;
				factor = std::clamp(factor, 0.0, 1.0);
				DirectX::SimpleMath::Quaternion Q0{ q0.x, q0.y, q0.z, q0.w }, Q1{ q1.x, q1.y, q1.z, q1.w };
				rotBlend = DirectX::SimpleMath::Quaternion::Slerp(Q0, Q1, static_cast<float>(factor));
				rotBlend.Normalize();
			}
		}

		// 最終ブレンド
		DirectX::SimpleMath::Vector3 finalPos = posPlay + (posBlend - posPlay) * alpha;
		DirectX::SimpleMath::Vector3 finalScale = scalePlay + (scaleBlend - scalePlay) * alpha;
		DirectX::SimpleMath::Quaternion finalRot = DirectX::SimpleMath::Quaternion::Slerp(rotPlay, rotBlend, alpha);
		finalRot.Normalize();

		Transform boneTransform{};
		boneTransform.Position = finalPos;
		boneTransform.Rotation = finalRot;
		boneTransform.Scale = finalScale;
		bone.AnimationMatrix = boneTransform.GetWorldMatrix();
	}

	// ボーン行列更新
	UpdateBoneMatrix(m_pModel->GetScene()->mRootNode, DirectX::XMMatrixIdentity());

	// CB 用配列更新
	std::vector<DirectX::XMMATRIX>& boneMatCB = m_pModel->GetBoneMatCB();
	for (auto& bone : bones)
	{
		boneMatCB[bone.second.index] = bone.second.Matrix;
	}

	// ブレンド終了処理
	if (alpha >= 1.0f)
	{
		// ブレンド先を再生中アニメに切り替え、ブレンド状態をクリア
		m_pPlayAnimation = m_pBlendAnimation;
		m_pBlendAnimation = nullptr;

		// 時間情報をブレンド先の状態に引き継ぐ
		m_ticksPerSecond = m_blendTicksPerSecond;
		m_animationDuration = m_blendAnimationDuration;
		m_animeTimeTicks = m_blendAnimeTimeTicks;

		// クリア
		m_blendElapsedSeconds = 0.0f;
		m_blendAnimeTimeTicks = 0.0f;
		m_blendTicksPerSecond = 0.0f;
		m_blendAnimationDuration = 0.0f;
	}
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

aiNodeAnim* SkeletalAnimator::FindChannel(aiAnimation* anim, const std::string& name)
{
	if (!anim) return nullptr;

	// チャネル配列を走査して、名前が一致するチャネルを探す
	for (unsigned int i = 0; i < anim->mNumChannels; ++i)
	{
		aiNodeAnim* ch = anim->mChannels[i];
		if (ch && name == ch->mNodeName.C_Str()) return ch;
	}
	return nullptr;
}
