/*+===================================================================
	File: MeshRenderer.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/14 17:22:58 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "MeshRenderer.hpp"
#include <SimpleMath.h>
#include "Game/Actor/Actor.hpp"
#include "Utility/CameraManager/CameraManager.hpp"
#include <chrono> // 追加: 時間差分取得

void MeshRenderer::Init(Model* pModel)
{
	m_pModel = pModel;
	std::vector<Model::Mesh> meshes = m_pModel->GetMeshes();

	m_pVertexBuffers.reserve(meshes.size());
	m_pIndexBuffers.reserve(meshes.size());
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		auto& mesh = meshes[i];

		// 頂点バッファの生成
		auto vertexSize = mesh.Vertices.size() * sizeof(Vertex::Mesh);
		auto vertexStride = sizeof(Vertex::Mesh);
		auto pVertexBuffer = new VertexBuffer(vertexSize, vertexStride, mesh.Vertices.data());
		assert(pVertexBuffer);	// nullptrチェック
		m_pVertexBuffers.push_back(pVertexBuffer);

		// インデックスバッファの生成
		auto indexSize = mesh.Indices.size() * sizeof(uint32_t);
		auto pIndexBuffer = new IndexBuffer(indexSize, mesh.Indices.data());
		assert(pIndexBuffer);	// nullptrチェック
		m_pIndexBuffers.push_back(pIndexBuffer);
	}

	m_pDescriptorHeap = new DescriptorHeap();
	m_pMaterialHandles.clear();
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		auto& mesh = meshes[i];
		auto handle = m_pDescriptorHeap->Register(mesh.DiffuseMap->Resource(), mesh.DiffuseMap->ViewDesc());
		assert(handle);	// nullptrチェック
		m_pMaterialHandles.push_back(handle);
	}

	// 定数バッファの生成
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_pWVPCB[i] = new ConstantBuffer(sizeof(CB::WVP));
		assert(m_pWVPCB[i]);	// nullptrチェック
		CB::WVP* WVPptr = m_pWVPCB[i]->GetPtr<CB::WVP>();
		WVPptr->WorldMat = m_Owner->GetTransform().GetWorldMatrixFloat4x4(false);
		WVPptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->GetViewMatrixFloat4x4(false);
		WVPptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->GetProjectionMatrixFloat4x4(false);

		m_pBoneMatrixCB[i] = new ConstantBuffer(sizeof(DirectX::XMMATRIX) * m_pModel->GetBones().size());
		assert(m_pBoneMatrixCB[i]); // nullptrチェック
		for (size_t j = 0; j < m_pModel->GetBones().size(); ++j)
		{
			CB::BoneMatrix* bonePtr = m_pBoneMatrixCB[i]->GetPtr<CB::BoneMatrix>();
			bonePtr->BoneMat[j] = DirectX::XMMatrixIdentity();
		}
	}

	// ルートシグネチャの生成
	m_pRootSignature = new RootSignature();
	assert(m_pRootSignature);	// nullptrチェック
	m_pRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
	m_pRootSignature->AddRootParameter(1, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
	m_pRootSignature->AddDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, D3D12_SHADER_VISIBILITY_PIXEL); // テクスチャ
	m_pRootSignature->AddStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR); // スタティックサンプラー
	m_pRootSignature->Create();

	// パイプラインステートの生成
	m_pPipelineState = new PipelineState();
	assert(m_pPipelineState);	// nullptrチェック
	m_pPipelineState->SetInputLayout(Vertex::Mesh::InputLayout);
	m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
	m_pPipelineState->SetVS(L"Assets/Shader/SkeletalMeshVS.cso");
	m_pPipelineState->SetPS(L"Assets/Shader/SimplePS.cso");
	m_pPipelineState->Create();
}

void MeshRenderer::Update()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();

	CB::WVP* ptr = m_pWVPCB[currentIndex]->GetPtr<CB::WVP>();
	ptr->WorldMat = m_Owner->GetTransform().GetWorldMatrixFloat4x4(false);
	ptr->ViewMat = CameraManager::GetInstance().GetMainCamera()->GetViewMatrixFloat4x4(false);
	ptr->ProjMat = CameraManager::GetInstance().GetMainCamera()->GetProjectionMatrixFloat4x4(false);

	if (!m_pAnimation) return;

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

	aiAnimation* pAnimation = m_pAnimation->GetAnimation(0);
	if (!pAnimation) return;

	std::unordered_map<std::string, Model::Bone>& bones = m_pModel->GetBones();

	// ticksPerSecondが0の場合は一般的な値(例:25)を使用
	double ticksPerSecond = (pAnimation->mTicksPerSecond != 0.0) ? pAnimation->mTicksPerSecond : 25.0;

	// アニメーション時間をticks単位で更新し、durationでループ
	animeTimeTicks += deltaSeconds * ticksPerSecond;
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

	std::vector<DirectX::XMMATRIX> boneMatrices{};
	boneMatrices.resize(bones.size());
	for (auto& bone : bones)
	{
		boneMatrices[bone.second.index] = bone.second.Matrix;
	}

	// animeTimeTicks はすでに更新済み

	CB::BoneMatrix* bonePtr = m_pBoneMatrixCB[currentIndex]->GetPtr<CB::BoneMatrix>();
	std::copy_n(boneMatrices.data(), boneMatrices.size(), bonePtr->BoneMat);
}

void MeshRenderer::Draw()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();
	auto commandList = Engine::GetInstance().GetCommandList();				// コマンドリストを取得
	auto materialHeap = m_pDescriptorHeap->GetHeap();						// ディスクリプタヒープを取得

	std::vector<Model::Mesh> meshes = m_pModel->GetMeshes();

	for (size_t i = 0; i < meshes.size(); ++i)
	{
		auto vbView = m_pVertexBuffers[i]->GetView();		// 頂点バッファビューを取得
		auto ibView = m_pIndexBuffers[i]->GetView();		// インデックスバッファビューを取得
		auto materialHandle = m_pMaterialHandles[i];		// マテリアルのディスクリプタハンドルを取得

		commandList->SetGraphicsRootSignature(m_pRootSignature->Get());			// ルートシグネチャを設定
		commandList->SetPipelineState(m_pPipelineState->Get());					// パイプラインステートを設定
		commandList->SetGraphicsRootConstantBufferView(0, m_pWVPCB[currentIndex]->GetAddress());		// 定数バッファを設定
		commandList->SetGraphicsRootConstantBufferView(1, m_pBoneMatrixCB[currentIndex]->GetAddress()); // ボーン行列用定数バッファを設定

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// プリミティブトポロジーを設定
		commandList->IASetVertexBuffers(0, 1, &vbView);								// 頂点バッファを設定
		commandList->IASetIndexBuffer(&ibView);										// インデックスバッファを設定

		commandList->SetDescriptorHeaps(1, &materialHeap);								// ディスクリプタヒープを設定
		commandList->SetGraphicsRootDescriptorTable(2, materialHandle->HandleGPU);	// ディスクリプタテーブルを設定

		commandList->DrawIndexedInstanced(static_cast<UINT>(meshes[i].Indices.size()), 1, 0, 0, 0);	// 描画
	}
}

void MeshRenderer::Uninit()
{
	for (auto& pVB : m_pVertexBuffers)
	{
		delete pVB;
	}

	for (auto& pIB : m_pIndexBuffers)
	{
		delete pIB;
	}

	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		delete m_pWVPCB[i];
		m_pWVPCB[i] = nullptr;
	}

	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		delete m_pBoneMatrixCB[i];
		m_pBoneMatrixCB[i] = nullptr;
	}

	delete m_pDescriptorHeap;
	m_pDescriptorHeap = nullptr;

	m_pMaterialHandles.clear();

	delete m_pRootSignature;
	m_pRootSignature = nullptr;

	delete m_pPipelineState;
	m_pPipelineState = nullptr;

	m_pModel = nullptr;
	m_pAnimation = nullptr;
}

void MeshRenderer::UpdateBoneMatrix(const aiNode* node, DirectX::XMMATRIX matrix)
{
	Model::Bone& bone = m_pModel->GetBones()[node->mName.C_Str()];

	bone.Matrix = bone.OffsetMatrix * bone.AnimationMatrix * matrix;

	for (size_t i = 0; i < node->mNumChildren; ++i)
	{
		UpdateBoneMatrix(node->mChildren[i], bone.AnimationMatrix * matrix);
	}
}

void MeshRenderer::SetAnimationData(Animation* pAnimation)
{
	m_pAnimation = pAnimation;
}
