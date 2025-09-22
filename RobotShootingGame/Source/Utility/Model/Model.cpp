/*+===================================================================
	File: Model.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/08 9:11:08 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Model.hpp"
#include "../../Game/Camera/Camera.hpp"
#include "../../Utility/Texture/Texture.hpp"

Model::Model(ModelData ModelData, std::vector<Animation> animations, Camera& Camera)
	: m_ModelData(ModelData), m_AnimationsData(animations), m_Camera(Camera)
{
	size_t meshCount = m_ModelData.Meshes.size();
	std::vector<Mesh>& meshes = m_ModelData.Meshes;

	// メッシュの数だけ頂点バッファを用意する
	m_pVertexBuffers.reserve(meshCount);
	for (size_t i = 0; i < meshCount; ++i)
	{
		auto size = sizeof(Vertex::Mesh) * meshes[i].Vertices.size();
		auto stride = sizeof(Vertex::Mesh);
		auto vertices = meshes[i].Vertices.data();
		auto pVB = new VertexBuffer(size, stride, vertices);
		assert(pVB);	// nullptrチェック

		m_pVertexBuffers.push_back(pVB);
	}

	// メッシュの数だけインデックスバッファを用意する
	m_pIndexBuffers.reserve(meshCount);
	for (size_t i = 0; i < meshCount; ++i)
	{
		auto size = sizeof(uint32_t) * meshes[i].Indices.size();
		auto indices = meshes[i].Indices.data();
		auto pIB = new IndexBuffer(size, indices);
		assert(pIB);	// nullptrチェック

		m_pIndexBuffers.push_back(pIB);
	}

	// マテリアル用のディスクリプタヒープを生成
	m_pDescriptorHeap = new DescriptorHeap();
	m_pMaterialHandles.clear();
	for (size_t i = 0; i < ModelData.Meshes.size(); ++i)
	{
		if (ModelData.Meshes[i].DiffuseMap == "") continue; // テクスチャがなければスキップ

		auto texture = new Texture();
		assert(texture);	// nullptrチェック

		if (texture->Load(meshes[i].DiffuseMap) == S_OK)
		{
			auto handle = m_pDescriptorHeap->Register(texture->Resource(), texture->ViewDesc());
			m_pMaterialHandles.push_back(handle);
		}
	}

	// 定数バッファの生成
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_pTransformBuffer[i] = new ConstantBuffer(sizeof(Transform));
		assert(m_pTransformBuffer[i]);	// nullptrチェック

		m_pBoneBuffer[i] = new ConstantBuffer(sizeof(BoneTransform));
		assert(m_pBoneBuffer[i]);	// nullptrチェック

		Transform* ptr = m_pTransformBuffer[i]->GetPtr<Transform>();
		ptr->World = DirectX::XMMatrixIdentity();
		ptr->View = m_Camera.GetViewMatrix();
		ptr->Proj = m_Camera.GetProjectionMatrix();
	}

	// ルートシグネチャの生成
	m_pRootSignature = new RootSignature();
	assert(m_pRootSignature);	// nullptrチェック
	m_pRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX); // 定数バッファ
	m_pRootSignature->AddRootParameter(1, D3D12_SHADER_VISIBILITY_VERTEX); // ボーン用定数バッファ
	m_pRootSignature->AddDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, D3D12_SHADER_VISIBILITY_PIXEL); // テクスチャ
	m_pRootSignature->AddStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR); // スタティックサンプラー
	m_pRootSignature->Create();

	// パイプラインステートの生成
	m_pPipelineState = new PipelineState();
	assert(m_pPipelineState);	// nullptrチェック
	m_pPipelineState->SetInputLayout(Vertex::Mesh::InputLayout);
	m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
	m_pPipelineState->SetVS(L"Assets/Shader/ModelVS.cso");
	m_pPipelineState->SetPS(L"Assets/Shader/ModelPS.cso");
	m_pPipelineState->Create();
}

Model::~Model()
{
	// 頂点バッファの解放
	for (auto vb : m_pVertexBuffers)
	{
		delete vb;
	}
	m_pVertexBuffers.clear();

	// インデックスバッファの解放
	for (auto ib : m_pIndexBuffers)
	{
		delete ib;
	}
	m_pIndexBuffers.clear();

	// 定数バッファの解放
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		delete m_pTransformBuffer[i];
		m_pTransformBuffer[i] = nullptr;
	}

	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		delete m_pBoneBuffer[i];
		m_pBoneBuffer[i] = nullptr;
	}

	// ルートシグネチャの解放
	if (m_pRootSignature)
	{
		delete m_pRootSignature;
		m_pRootSignature = nullptr;
	}

	// パイプラインステートの解放
	if (m_pPipelineState)
	{
		delete m_pPipelineState;
		m_pPipelineState = nullptr;
	}

	// ディスクリプタヒープの解放
	if (m_pDescriptorHeap)
	{
		delete m_pDescriptorHeap;
		m_pDescriptorHeap = nullptr;
	}

	// ディスクリプタハンドルの解放
	m_pMaterialHandles.clear();
	m_pMaterialHandles.shrink_to_fit();
}

void Model::Update()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得
	Transform* ptr = m_pTransformBuffer[currentIndex]->GetPtr<Transform>();	// 定数バッファのポインタを取得

	ptr->View = m_Camera.GetViewMatrix();
	ptr->Proj = m_Camera.GetProjectionMatrix();
}

void Model::Update(int AnimeNo, float flame)
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得
	Transform* ptr = m_pTransformBuffer[currentIndex]->GetPtr<Transform>();	// 定数バッファのポインタを取得

	ptr->View = m_Camera.GetViewMatrix();
	ptr->Proj = m_Camera.GetProjectionMatrix();

	if (m_AnimationsData.size() < AnimeNo) return;

	const Animation& anim = m_AnimationsData[AnimeNo - 1];

	std::vector<DirectX::SimpleMath::Matrix> boneTransforms(m_ModelData.Bones.size(), DirectX::SimpleMath::Matrix::Identity);

	for (size_t i = 0; i < anim.BoneAnimations.size(); ++i)
	{
		const BoneAnimation& boneAnim = anim.BoneAnimations[i];
		int boneIdx = FindBoneIndexByName(boneAnim.BoneName);
		if (boneIdx < 0) continue;

		KeyFrame interpolated = InterpolateKeyFrame(boneAnim, flame);
		DirectX::SimpleMath::Matrix local =
			DirectX::SimpleMath::Matrix::CreateScale(interpolated.Scale) *
			DirectX::SimpleMath::Matrix::CreateFromQuaternion(interpolated.Rotation) *
			DirectX::SimpleMath::Matrix::CreateTranslation(interpolated.Position);

		boneTransforms[boneIdx] = local;
	}

	for (size_t i = 0; i < m_ModelData.Bones.size(); ++i)
	{
		int parentIdx = m_ModelData.Bones[i].ParentID;
		if (parentIdx >= 0)
		{
			boneTransforms[i] = boneTransforms[i] * boneTransforms[parentIdx];
		}
	}

	for (size_t i = 0; i < m_ModelData.Bones.size(); ++i)
	{
		boneTransforms[i] = boneTransforms[i] * m_ModelData.Bones[i].InverseBindPose;
	}

	UpdateBoneBuffer(boneTransforms);
}

//void Model::Update(std::string AnimationName_1, int frame_1, std::string AnimationName_2, int frame_2, float blendRate)
//{
//	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得
//	Transform* ptr = m_pTransformBuffer[currentIndex]->GetPtr<Transform>();	// 定数バッファのポインタを取得
//
//	ptr->View = m_Camera.GetViewMatrix();
//	ptr->Proj = m_Camera.GetProjectionMatrix();
//
//	// 該当するアニメーションが存在しなければ終了
//	if (!m_Animations.count(AnimationName_1)) return;
//	if (!m_Animations.count(AnimationName_2)) return;
//
//	// アニメーションが存在しなければ終了
//	if (!m_Animations[AnimationName_1]->HasAnimations()) return;
//	if (!m_Animations[AnimationName_2]->HasAnimations()) return;
//
//	aiAnimation* animation_1 = m_Animations[AnimationName_1]->mAnimations[0];
//	aiAnimation* animation_2 = m_Animations[AnimationName_2]->mAnimations[0];
//
//	int flame;
//	aiVector3D pos_1, pos_2;
//	aiQuaternion rot_1, rot_2;
//	aiVector3D pos;
//	aiQuaternion rot;
//
//	// ボーンの数だけループ
//	for (auto& pair : m_ModelData.BoneMap)
//	{
//		// ボーンの名前からボーンを取得
//		Bone& bone = m_ModelData.BoneMap[pair.first];
//
//		aiNodeAnim* nodeAnim_1 = nullptr;
//		aiNodeAnim* nodeAnim_2 = nullptr;
//
//		for (size_t i = 0; i < animation_1->mNumChannels; ++i)
//		{
//			if (animation_1->mChannels[i]->mNodeName == aiString(pair.first))
//			{
//				nodeAnim_1 = animation_1->mChannels[i];
//				break;
//			}
//		}
//
//		for (size_t i = 0; i < animation_2->mNumChannels; ++i)
//		{
//			if (animation_2->mChannels[i]->mNodeName == aiString(pair.first))
//			{
//				nodeAnim_2 = animation_2->mChannels[i];
//				break;
//			}
//		}
//
//		if (nodeAnim_1)
//		{
//			// フレーム数をキーフレームの数で割った余りを取得
//			flame = frame_1 % static_cast<int>(nodeAnim_1->mNumPositionKeys);
//			pos_1 = nodeAnim_1->mPositionKeys[flame].mValue;
//
//			flame = frame_1 % static_cast<int>(nodeAnim_1->mNumRotationKeys);
//			rot_1 = nodeAnim_1->mRotationKeys[flame].mValue;
//		}
//
//		if (nodeAnim_2)
//		{
//			// フレーム数をキーフレームの数で割った余りを取得
//			flame = frame_2 % static_cast<int>(nodeAnim_2->mNumPositionKeys);
//			pos_2 = nodeAnim_2->mPositionKeys[flame].mValue;
//
//			flame = frame_2 % static_cast<int>(nodeAnim_2->mNumRotationKeys);
//			rot_2 = nodeAnim_2->mRotationKeys[flame].mValue;
//		}
//
//		// 位置と回転を補間
//		pos = pos_1 * (1.0f - blendRate) + pos_2 * blendRate;		// 線形補間
//		aiQuaternion::Interpolate(rot, rot_1, rot_2, blendRate);	// 球面線形補間
//
//		// アニメーション行列を計算
//		bone.AnimationMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), rot, pos);
//	}
//
//	// ルートノードから順にボーン行列を更新
//	aiMatrix4x4 rootMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), aiQuaternion(static_cast<float>(AI_MATH_PI), 0.0f, 0.0f), aiVector3D(0.0f, 0.0f, 0.0f));
//	UpdateBoneMatrix(m_ModelData.AiScene->mRootNode, rootMatrix);
//
//	for (size_t i = 0; i < m_ModelData.Meshes.size(); ++i)
//	{
//		for (size_t j = 0; j < m_ModelData.Meshes[i].Vertices.size(); ++j)
//		{
//			DeformVertex& deformVertex = m_ModelData.Meshes[i].DeformVertices[j];
//
//			aiMatrix4x4 matrix[4];
//			aiMatrix4x4 finalMatrix = aiMatrix4x4();
//			matrix[0] = m_ModelData.BoneMap[deformVertex.BoneName[0]].Matrix;
//			matrix[1] = m_ModelData.BoneMap[deformVertex.BoneName[1]].Matrix;
//			matrix[2] = m_ModelData.BoneMap[deformVertex.BoneName[2]].Matrix;
//			matrix[3] = m_ModelData.BoneMap[deformVertex.BoneName[3]].Matrix;
//
//			finalMatrix.a1 = matrix[0].a1 * deformVertex.BoneWeight[0] + matrix[1].a1 * deformVertex.BoneWeight[1] + matrix[2].a1 * deformVertex.BoneWeight[2] + matrix[3].a1 * deformVertex.BoneWeight[3];
//			finalMatrix.a2 = matrix[0].a2 * deformVertex.BoneWeight[0] + matrix[1].a2 * deformVertex.BoneWeight[1] + matrix[2].a2 * deformVertex.BoneWeight[2] + matrix[3].a2 * deformVertex.BoneWeight[3];
//			finalMatrix.a3 = matrix[0].a3 * deformVertex.BoneWeight[0] + matrix[1].a3 * deformVertex.BoneWeight[1] + matrix[2].a3 * deformVertex.BoneWeight[2] + matrix[3].a3 * deformVertex.BoneWeight[3];
//			finalMatrix.a4 = matrix[0].a4 * deformVertex.BoneWeight[0] + matrix[1].a4 * deformVertex.BoneWeight[1] + matrix[2].a4 * deformVertex.BoneWeight[2] + matrix[3].a4 * deformVertex.BoneWeight[3];
//
//			finalMatrix.b1 = matrix[0].b1 * deformVertex.BoneWeight[0] + matrix[1].b1 * deformVertex.BoneWeight[1] + matrix[2].b1 * deformVertex.BoneWeight[2] + matrix[3].b1 * deformVertex.BoneWeight[3];
//			finalMatrix.b2 = matrix[0].b2 * deformVertex.BoneWeight[0] + matrix[1].b2 * deformVertex.BoneWeight[1] + matrix[2].b2 * deformVertex.BoneWeight[2] + matrix[3].b2 * deformVertex.BoneWeight[3];
//			finalMatrix.b3 = matrix[0].b3 * deformVertex.BoneWeight[0] + matrix[1].b3 * deformVertex.BoneWeight[1] + matrix[2].b3 * deformVertex.BoneWeight[2] + matrix[3].b3 * deformVertex.BoneWeight[3];
//			finalMatrix.b4 = matrix[0].b4 * deformVertex.BoneWeight[0] + matrix[1].b4 * deformVertex.BoneWeight[1] + matrix[2].b4 * deformVertex.BoneWeight[2] + matrix[3].b4 * deformVertex.BoneWeight[3];
//
//			finalMatrix.c1 = matrix[0].c1 * deformVertex.BoneWeight[0] + matrix[1].c1 * deformVertex.BoneWeight[1] + matrix[2].c1 * deformVertex.BoneWeight[2] + matrix[3].c1 * deformVertex.BoneWeight[3];
//			finalMatrix.c2 = matrix[0].c2 * deformVertex.BoneWeight[0] + matrix[1].c2 * deformVertex.BoneWeight[1] + matrix[2].c2 * deformVertex.BoneWeight[2] + matrix[3].c2 * deformVertex.BoneWeight[3];
//			finalMatrix.c3 = matrix[0].c3 * deformVertex.BoneWeight[0] + matrix[1].c3 * deformVertex.BoneWeight[1] + matrix[2].c3 * deformVertex.BoneWeight[2] + matrix[3].c3 * deformVertex.BoneWeight[3];
//			finalMatrix.c4 = matrix[0].c4 * deformVertex.BoneWeight[0] + matrix[1].c4 * deformVertex.BoneWeight[1] + matrix[2].c4 * deformVertex.BoneWeight[2] + matrix[3].c4 * deformVertex.BoneWeight[3];
//
//			finalMatrix.d1 = matrix[0].d1 * deformVertex.BoneWeight[0] + matrix[1].d1 * deformVertex.BoneWeight[1] + matrix[2].d1 * deformVertex.BoneWeight[2] + matrix[3].d1 * deformVertex.BoneWeight[3];
//			finalMatrix.d2 = matrix[0].d2 * deformVertex.BoneWeight[0] + matrix[1].d2 * deformVertex.BoneWeight[1] + matrix[2].d2 * deformVertex.BoneWeight[2] + matrix[3].d2 * deformVertex.BoneWeight[3];
//			finalMatrix.d3 = matrix[0].d3 * deformVertex.BoneWeight[0] + matrix[1].d3 * deformVertex.BoneWeight[1] + matrix[2].d3 * deformVertex.BoneWeight[2] + matrix[3].d3 * deformVertex.BoneWeight[3];
//			finalMatrix.d4 = matrix[0].d4 * deformVertex.BoneWeight[0] + matrix[1].d4 * deformVertex.BoneWeight[1] + matrix[2].d4 * deformVertex.BoneWeight[2] + matrix[3].d4 * deformVertex.BoneWeight[3];
//
//			deformVertex.Position *= finalMatrix;
//
//			finalMatrix.a4 = 0.0f;
//			finalMatrix.b4 = 0.0f;
//			finalMatrix.c4 = 0.0f;
//
//			deformVertex.Normal *= finalMatrix;
//
//			// 頂点データの更新
//			m_ModelData.Meshes[i].Vertices[j].Position = { deformVertex.Position.x, deformVertex.Position.y, deformVertex.Position.z };
//			m_ModelData.Meshes[i].Vertices[j].Normal = { deformVertex.Normal.x, deformVertex.Normal.y, deformVertex.Normal.z };
//		}
//
//		// 頂点バッファの更新
//		auto size = sizeof(Vertex::Mesh) * m_ModelData.Meshes[i].Vertices.size();
//		auto vertices = m_ModelData.Meshes[i].Vertices.data();
//		m_pVertexBuffers[i]->Update(size, vertices);
//	}
//}

//void Model::Update(std::string AnimationName, int frame)
//{
//	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得
//	Transform* ptr = m_pTransformBuffer[currentIndex]->GetPtr<Transform>();	// 定数バッファのポインタを取得
//	ptr->View = m_Camera.GetViewMatrix();
//	ptr->Proj = m_Camera.GetProjectionMatrix();
//
//	// 該当するアニメーションが存在しなければ終了
//	if (!m_Animations.count(AnimationName)) return;
//
//	// アニメーションが存在しなければ終了
//	if (!m_Animations[AnimationName]->HasAnimations()) return;
//
//	aiAnimation* animation = m_Animations[AnimationName]->mAnimations[0];
//
//	int flame;
//	aiVector3D pos;
//	aiQuaternion rot;
//
//	// ボーンの数だけループ
//	for (auto& pair : m_ModelData.BoneMap)
//	{
//		// ボーンの名前からボーンを取得
//		Bone& bone = m_ModelData.BoneMap[pair.first];
//		aiNodeAnim* nodeAnim = nullptr;
//
//		for (size_t i = 0; i < animation->mNumChannels; ++i)
//		{
//			if (animation->mChannels[i]->mNodeName == aiString(pair.first))
//			{
//				nodeAnim = animation->mChannels[i];
//				break;
//			}
//		}
//
//		if (nodeAnim)
//		{
//			// フレーム数をキーフレームの数で割った余りを取得
//			flame = frame % static_cast<int>(nodeAnim->mNumPositionKeys);
//			pos = nodeAnim->mPositionKeys[flame].mValue;
//			flame = frame % static_cast<int>(nodeAnim->mNumRotationKeys);
//			rot = nodeAnim->mRotationKeys[flame].mValue;
//
//			// アニメーション行列を計算
//			bone.AnimationMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), rot, pos);
//		}
//	}
//
//	// ルートノードから順にボーン行列を更新
//	aiMatrix4x4 rootMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), aiQuaternion(static_cast<float>(AI_MATH_PI), 0.0f, 0.0f), aiVector3D(0.0f, 0.0f, 0.0f));
//	UpdateBoneMatrix(m_ModelData.AiScene->mRootNode, rootMatrix);
//
//	for (size_t i = 0; i < m_ModelData.Meshes.size(); ++i)
//	{
//		for (size_t j = 0; j < m_ModelData.Meshes[i].Vertices.size(); ++j)
//		{
//			DeformVertex& deformVertex = m_ModelData.Meshes[i].DeformVertices[j];
//			aiMatrix4x4 matrix[4];
//			aiMatrix4x4 finalMatrix = aiMatrix4x4();
//
//			matrix[0] = m_ModelData.BoneMap[deformVertex.BoneName[0]].Matrix;
//			matrix[1] = m_ModelData.BoneMap[deformVertex.BoneName[1]].Matrix;
//			matrix[2] = m_ModelData.BoneMap[deformVertex.BoneName[2]].Matrix;
//			matrix[3] = m_ModelData.BoneMap[deformVertex.BoneName[3]].Matrix;
//
//			finalMatrix.a1 = matrix[0].a1 * deformVertex.BoneWeight[0] + matrix[1].a1 * deformVertex.BoneWeight[1] + matrix[2].a1 * deformVertex.BoneWeight[2] + matrix[3].a1 * deformVertex.BoneWeight[3];
//			finalMatrix.a2 = matrix[0].a2 * deformVertex.BoneWeight[0] + matrix[1].a2 * deformVertex.BoneWeight[1] + matrix[2].a2 * deformVertex.BoneWeight[2] + matrix[3].a2 * deformVertex.BoneWeight[3];
//			finalMatrix.a3 = matrix[0].a3 * deformVertex.BoneWeight[0] + matrix[1].a3 * deformVertex.BoneWeight[1] + matrix[2].a3 * deformVertex.BoneWeight[2] + matrix[3].a3 * deformVertex.BoneWeight[3];
//			finalMatrix.a4 = matrix[0].a4 * deformVertex.BoneWeight[0] + matrix[1].a4 * deformVertex.BoneWeight[1] + matrix[2].a4 * deformVertex.BoneWeight[2] + matrix[3].a4 * deformVertex.BoneWeight[3];
//
//			finalMatrix.b1 = matrix[0].b1 * deformVertex.BoneWeight[0] + matrix[1].b1 * deformVertex.BoneWeight[1] + matrix[2].b1 * deformVertex.BoneWeight[2] + matrix[3].b1 * deformVertex.BoneWeight[3];
//			finalMatrix.b2 = matrix[0].b2 * deformVertex.BoneWeight[0] + matrix[1].b2 * deformVertex.BoneWeight[1] + matrix[2].b2 * deformVertex.BoneWeight[2] + matrix[3].b2 * deformVertex.BoneWeight[3];
//			finalMatrix.b3 = matrix[0].b3 * deformVertex.BoneWeight[0] + matrix[1].b3 * deformVertex.BoneWeight[1] + matrix[2].b3 * deformVertex.BoneWeight[2] + matrix[3].b3 * deformVertex.BoneWeight[3];
//			finalMatrix.b4 = matrix[0].b4 * deformVertex.BoneWeight[0] + matrix[1].b4 * deformVertex.BoneWeight[1] + matrix[2].b4 * deformVertex.BoneWeight[2] + matrix[3].b4 * deformVertex.BoneWeight[3];
//
//			finalMatrix.c1 = matrix[0].c1 * deformVertex.BoneWeight[0] + matrix[1].c1 * deformVertex.BoneWeight[1] + matrix[2].c1 * deformVertex.BoneWeight[2] + matrix[3].c1 * deformVertex.BoneWeight[3];
//			finalMatrix.c2 = matrix[0].c2 * deformVertex.BoneWeight[0] + matrix[1].c2 * deformVertex.BoneWeight[1] + matrix[2].c2 * deformVertex.BoneWeight[2] + matrix[3].c2 * deformVertex.BoneWeight[3];
//			finalMatrix.c3 = matrix[0].c3 * deformVertex.BoneWeight[0] + matrix[1].c3 * deformVertex.BoneWeight[1] + matrix[2].c3 * deformVertex.BoneWeight[2] + matrix[3].c3 * deformVertex.BoneWeight[3];
//			finalMatrix.c4 = matrix[0].c4 * deformVertex.BoneWeight[0] + matrix[1].c4 * deformVertex.BoneWeight[1] + matrix[2].c4 * deformVertex.BoneWeight[2] + matrix[3].c4 * deformVertex.BoneWeight[3];
//
//			finalMatrix.d1 = matrix[0].d1 * deformVertex.BoneWeight[0] + matrix[1].d1 * deformVertex.BoneWeight[1] + matrix[2].d1 * deformVertex.BoneWeight[2] + matrix[3].d1 * deformVertex.BoneWeight[3];
//			finalMatrix.d2 = matrix[0].d2 * deformVertex.BoneWeight[0] + matrix[1].d2 * deformVertex.BoneWeight[1] + matrix[2].d2 * deformVertex.BoneWeight[2] + matrix[3].d2 * deformVertex.BoneWeight[3];
//			finalMatrix.d3 = matrix[0].d3 * deformVertex.BoneWeight[0] + matrix[1].d3 * deformVertex.BoneWeight[1] + matrix[2].d3 * deformVertex.BoneWeight[2] + matrix[3].d3 * deformVertex.BoneWeight[3];
//			finalMatrix.d4 = matrix[0].d4 * deformVertex.BoneWeight[0] + matrix[1].d4 * deformVertex.BoneWeight[1] + matrix[2].d4 * deformVertex.BoneWeight[2] + matrix[3].d4 * deformVertex.BoneWeight[3];
//
//			deformVertex.Position *= finalMatrix;
//			finalMatrix.a4 = 0.0f;
//			finalMatrix.b4 = 0.0f;
//			finalMatrix.c4 = 0.0f;
//
//			deformVertex.Normal *= finalMatrix;
//
//			// 頂点データの更新
//			m_ModelData.Meshes[i].Vertices[j].Position = { deformVertex.Position.x, deformVertex.Position.y, deformVertex.Position.z };
//			m_ModelData.Meshes[i].Vertices[j].Normal = { deformVertex.Normal.x, deformVertex.Normal.y, deformVertex.Normal.z };
//		}
//
//		// 頂点バッファの更新
//		auto size = sizeof(Vertex::Mesh) * m_ModelData.Meshes[i].Vertices.size();
//		auto vertices = m_ModelData.Meshes[i].Vertices.data();
//		m_pVertexBuffers[i]->Update(size, vertices);
//	}
//}

void Model::Draw()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得
	auto commandList = Engine::GetInstance().GetCommandList();				// コマンドリストを取得
	auto materialHeap = m_pDescriptorHeap->GetHeap();						// ディスクリプタヒープを取得

	for (size_t i = 0; i < m_ModelData.Meshes.size(); ++i)
	{
		auto vbView = m_pVertexBuffers[i]->GetView();	// 頂点バッファビューを取得
		auto ibView = m_pIndexBuffers[i]->GetView();	// インデックスバッファビューを取得

		commandList->SetGraphicsRootSignature(m_pRootSignature->Get());			// ルートシグネチャを設定
		commandList->SetPipelineState(m_pPipelineState->Get());					// パイプラインステートを設定
		commandList->SetGraphicsRootConstantBufferView(0, m_pTransformBuffer[currentIndex]->GetAddress());	// 定数バッファを設定
		commandList->SetGraphicsRootConstantBufferView(1, m_pBoneBuffer[currentIndex]->GetAddress());		// 定数バッファを設定

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// プリミティブトポロジーを設定
		commandList->IASetVertexBuffers(0, 1, &vbView);								// 頂点バッファを設定
		commandList->IASetIndexBuffer(&ibView);										// インデックスバッファを設定

		if (m_ModelData.Meshes[i].DiffuseMap != "")
		{
			commandList->SetDescriptorHeaps(1, &materialHeap);									// ディスクリプタヒープを設定
			commandList->SetGraphicsRootDescriptorTable(1, m_pMaterialHandles[i]->HandleGPU);	// ディスクリプタテーブルを設定
		}
		commandList->DrawIndexedInstanced(static_cast<UINT>(m_ModelData.Meshes[i].Indices.size()), 1, 0, 0, 0);	// 描画
	}
}

int Model::FindBoneIndexByName(const std::string& BoneName) const
{
	for (size_t i = 0; i < m_ModelData.Bones.size(); ++i)
	{
		if (m_ModelData.Bones[i].Name == BoneName) return static_cast<int>(i);
	}

	return -1;
}

KeyFrame Model::InterpolateKeyFrame(const BoneAnimation& BoneAnim, float flame)
{
	if (BoneAnim.KeyFrames.empty()) return {};

	size_t idx{};
	for (size_t i = 1; i < BoneAnim.KeyFrames.size(); ++i)
	{
		if (BoneAnim.KeyFrames[i].Time > flame)
		{
			idx = i - 1;
			break;
		}
	}

	return BoneAnim.KeyFrames[idx];
}

void Model::UpdateBoneBuffer(const std::vector<DirectX::SimpleMath::Matrix>& boneTransforms)
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得

	BoneTransform* ptr = m_pBoneBuffer[currentIndex]->GetPtr<BoneTransform>();	// 定数バッファのポインタを取得
	size_t count = std::min(boneTransforms.size(), static_cast<size_t>(MAX_BONES));
	for (size_t i = 0; i < count; ++i)
	{
		ptr->BoneMatrices[i] = boneTransforms[i];
	}
	for (size_t i = count; i < MAX_BONES; ++i)
	{
		ptr->BoneMatrices[i] = DirectX::SimpleMath::Matrix::Identity;
	}
}

//void Model::AddAnimation(const aiScene* Scene, const std::string& AnimationName)
//{
//	m_Animations[AnimationName] = Scene;
//}

//void Model::UpdateBoneMatrix(aiNode* node, aiMatrix4x4 matrix)
//{
//	Bone& bone = m_ModelData.BoneMap[node->mName.C_Str()];
//
//	// 親の行列と自身の行列を掛け合わせる
//	aiMatrix4x4 worldMatrix;
//
//	worldMatrix *= matrix;					// 親の行列
//	worldMatrix *= bone.AnimationMatrix;	// 自身の行列
//
//	bone.Matrix = worldMatrix;				// ボーンの行列を更新
//	bone.Matrix *= bone.OffsetMatrix;		// オフセット行列を掛け合わせる
//
//	// 子ノードに対して再帰的に処理を行う
//	for (size_t i = 0; i < node->mNumChildren; ++i)
//	{
//		UpdateBoneMatrix(node->mChildren[i], worldMatrix);
//	}
//}
