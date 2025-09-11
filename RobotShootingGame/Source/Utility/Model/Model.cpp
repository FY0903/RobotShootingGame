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

Model::Model(ModelData ModelData, Camera& Camera)
	: m_ModelData(ModelData), m_Camera(Camera)
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
	m_pTextures.resize(meshCount);
	for (size_t i = 0; i < meshCount; ++i)
	{
		m_pTextures[i] = new Texture();
		assert(m_pTextures[i]);	// nullptrチェック

		m_pTextures[i]->Load(meshes[i].DiffuseMap);
		auto handle = m_pDescriptorHeap->Register(m_pTextures[i]->Resource(), m_pTextures[i]->ViewDesc());
		m_pMaterialHandles.push_back(handle);
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
	m_pRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_ALL); // 定数バッファ
	m_pRootSignature->AddDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, D3D12_SHADER_VISIBILITY_PIXEL); // テクスチャ
	m_pRootSignature->AddStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR); // スタティックサンプラー
	m_pRootSignature->Create();

	// パイプラインステートの生成
	m_pPipelineState = new PipelineState();
	assert(m_pPipelineState);	// nullptrチェック
	m_pPipelineState->SetInputLayout(Vertex::Mesh::InputLayout);
	m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
	m_pPipelineState->SetVS(L"Assets/Shader/SimpleVS.cso");
	m_pPipelineState->SetPS(L"Assets/Shader/SimplePS.cso");
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

	// テクスチャの解放
	for (auto tex : m_pTextures)
	{
		delete tex;
	}
	m_pTextures.clear();

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

void Model::Update(std::string AnimationName_1, int frame_1, std::string AnimationName_2, int frame_2, float blendRate)
{
	if (m_ModelData.Animations.empty()) return;
	if (m_ModelData.Animations.find(AnimationName_1) == m_ModelData.Animations.end()) return;
	if (m_ModelData.Animations.find(AnimationName_2) == m_ModelData.Animations.end()) return;

	int flame;
	aiVector3D pos_1, pos_2;
	aiQuaternion rot_1, rot_2;
	aiVector3D pos;
	aiQuaternion rot;

	// ボーンの数だけループ
	for (auto& pair : m_ModelData.Bones)
	{
		// ボーンの名前からボーンを取得
		Bone& bone = m_ModelData.Bones[pair.first];

		auto animation_1 = m_ModelData.Animations[AnimationName_1];
		auto animation_2 = m_ModelData.Animations[AnimationName_2];
		Channel* nodeAnim_1 = nullptr;
		Channel* nodeAnim_2 = nullptr; 

		for (size_t i = 0; i < animation_1.Channels.size(); ++i)
		{
			if (animation_1.Channels[i].BoneName == aiString(pair.first).C_Str())
			{
				nodeAnim_1 = &animation_1.Channels[i];
				break;
			}
		}

		for (size_t i = 0; i < animation_2.Channels.size(); ++i)
		{
			if (animation_2.Channels[i].BoneName == aiString(pair.first).C_Str())
			{
				nodeAnim_2 = &animation_2.Channels[i];
				break;
			}
		}

		if (nodeAnim_1)
		{
			// キーフレームの数を取得
			size_t keyFrameCount_1 = nodeAnim_1->KeyFrames.size();
			// フレーム数をキーフレームの数で割った余りを取得
			flame = frame_1 % static_cast<int>(keyFrameCount_1);
			// キーフレームの位置と回転を取得
			pos_1 = nodeAnim_1->KeyFrames[flame].Position;
			rot_1 = nodeAnim_1->KeyFrames[flame].Rotation;
		}

		if (nodeAnim_2)
		{
			// キーフレームの数を取得
			size_t keyFrameCount_2 = nodeAnim_2->KeyFrames.size();
			// フレーム数をキーフレームの数で割った余りを取得
			flame = frame_2 % static_cast<int>(keyFrameCount_2);
			// キーフレームの位置と回転を取得
			pos_2 = nodeAnim_2->KeyFrames[flame].Position;
			rot_2 = nodeAnim_2->KeyFrames[flame].Rotation;
		}

		// 位置と回転を補間
		pos = pos_1 * (1.0f - blendRate) + pos_2 * blendRate;		// 線形補間
		aiQuaternion::Interpolate(rot, rot_1, rot_2, blendRate);	// 球面線形補間

		// アニメーション行列を計算
		bone.AnimationMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), rot, pos);
	}

	// ルートノードから順にボーン行列を更新
	aiMatrix4x4 rootMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), aiQuaternion(static_cast<float>(AI_MATH_PI), 0.0f, 0.0f), aiVector3D(0.0f, 0.0f, 0.0f));
	UpdateBoneMatrix(m_ModelData.RootNode, rootMatrix);

	for (size_t i = 0; i < m_ModelData.Meshes.size(); ++i)
	{
		for (size_t j = 0; j < m_ModelData.Meshes[i].DeformVertices.size(); ++j)
		{
			DeformVertex& deformVertex = m_ModelData.Meshes[i].DeformVertices[j];

			aiMatrix4x4 matrix[4];
			aiMatrix4x4 finalMatrix = aiMatrix4x4();
			matrix[0] = m_ModelData.Bones[deformVertex.BoneName[0]].Matrix;
			matrix[1] = m_ModelData.Bones[deformVertex.BoneName[1]].Matrix;
			matrix[2] = m_ModelData.Bones[deformVertex.BoneName[2]].Matrix;
			matrix[3] = m_ModelData.Bones[deformVertex.BoneName[3]].Matrix;

			finalMatrix.a1 = matrix[0].a1 * deformVertex.BoneWeight[0] + matrix[1].a1 * deformVertex.BoneWeight[1] + matrix[2].a1 * deformVertex.BoneWeight[2] + matrix[3].a1 * deformVertex.BoneWeight[3];
			finalMatrix.a2 = matrix[0].a2 * deformVertex.BoneWeight[0] + matrix[1].a2 * deformVertex.BoneWeight[1] + matrix[2].a2 * deformVertex.BoneWeight[2] + matrix[3].a2 * deformVertex.BoneWeight[3];
			finalMatrix.a3 = matrix[0].a3 * deformVertex.BoneWeight[0] + matrix[1].a3 * deformVertex.BoneWeight[1] + matrix[2].a3 * deformVertex.BoneWeight[2] + matrix[3].a3 * deformVertex.BoneWeight[3];
			finalMatrix.a4 = matrix[0].a4 * deformVertex.BoneWeight[0] + matrix[1].a4 * deformVertex.BoneWeight[1] + matrix[2].a4 * deformVertex.BoneWeight[2] + matrix[3].a4 * deformVertex.BoneWeight[3];

			finalMatrix.b1 = matrix[0].b1 * deformVertex.BoneWeight[0] + matrix[1].b1 * deformVertex.BoneWeight[1] + matrix[2].b1 * deformVertex.BoneWeight[2] + matrix[3].b1 * deformVertex.BoneWeight[3];
			finalMatrix.b2 = matrix[0].b2 * deformVertex.BoneWeight[0] + matrix[1].b2 * deformVertex.BoneWeight[1] + matrix[2].b2 * deformVertex.BoneWeight[2] + matrix[3].b2 * deformVertex.BoneWeight[3];
			finalMatrix.b3 = matrix[0].b3 * deformVertex.BoneWeight[0] + matrix[1].b3 * deformVertex.BoneWeight[1] + matrix[2].b3 * deformVertex.BoneWeight[2] + matrix[3].b3 * deformVertex.BoneWeight[3];
			finalMatrix.b4 = matrix[0].b4 * deformVertex.BoneWeight[0] + matrix[1].b4 * deformVertex.BoneWeight[1] + matrix[2].b4 * deformVertex.BoneWeight[2] + matrix[3].b4 * deformVertex.BoneWeight[3];

			finalMatrix.c1 = matrix[0].c1 * deformVertex.BoneWeight[0] + matrix[1].c1 * deformVertex.BoneWeight[1] + matrix[2].c1 * deformVertex.BoneWeight[2] + matrix[3].c1 * deformVertex.BoneWeight[3];
			finalMatrix.c2 = matrix[0].c2 * deformVertex.BoneWeight[0] + matrix[1].c2 * deformVertex.BoneWeight[1] + matrix[2].c2 * deformVertex.BoneWeight[2] + matrix[3].c2 * deformVertex.BoneWeight[3];
			finalMatrix.c3 = matrix[0].c3 * deformVertex.BoneWeight[0] + matrix[1].c3 * deformVertex.BoneWeight[1] + matrix[2].c3 * deformVertex.BoneWeight[2] + matrix[3].c3 * deformVertex.BoneWeight[3];
			finalMatrix.c4 = matrix[0].c4 * deformVertex.BoneWeight[0] + matrix[1].c4 * deformVertex.BoneWeight[1] + matrix[2].c4 * deformVertex.BoneWeight[2] + matrix[3].c4 * deformVertex.BoneWeight[3];

			finalMatrix.d1 = matrix[0].d1 * deformVertex.BoneWeight[0] + matrix[1].d1 * deformVertex.BoneWeight[1] + matrix[2].d1 * deformVertex.BoneWeight[2] + matrix[3].d1 * deformVertex.BoneWeight[3];
			finalMatrix.d2 = matrix[0].d2 * deformVertex.BoneWeight[0] + matrix[1].d2 * deformVertex.BoneWeight[1] + matrix[2].d2 * deformVertex.BoneWeight[2] + matrix[3].d2 * deformVertex.BoneWeight[3];
			finalMatrix.d3 = matrix[0].d3 * deformVertex.BoneWeight[0] + matrix[1].d3 * deformVertex.BoneWeight[1] + matrix[2].d3 * deformVertex.BoneWeight[2] + matrix[3].d3 * deformVertex.BoneWeight[3];
			finalMatrix.d4 = matrix[0].d4 * deformVertex.BoneWeight[0] + matrix[1].d4 * deformVertex.BoneWeight[1] + matrix[2].d4 * deformVertex.BoneWeight[2] + matrix[3].d4 * deformVertex.BoneWeight[3];
			
			deformVertex.Position *= finalMatrix;

			finalMatrix.a4 = 0.0f;
			finalMatrix.b4 = 0.0f;
			finalMatrix.c4 = 0.0f;

			deformVertex.Normal *= finalMatrix;

			// 頂点データの更新
			m_ModelData.Meshes[i].Vertices[j].Position = { deformVertex.Position.x, deformVertex.Position.y, deformVertex.Position.z };
			m_ModelData.Meshes[i].Vertices[j].Normal = { deformVertex.Normal.x, deformVertex.Normal.y, deformVertex.Normal.z };
		}
	}
}

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

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// プリミティブトポロジーを設定
		commandList->IASetVertexBuffers(0, 1, &vbView);								// 頂点バッファを設定
		commandList->IASetIndexBuffer(&ibView);										// インデックスバッファを設定

		commandList->SetDescriptorHeaps(1, &materialHeap);									// ディスクリプタヒープを設定
		commandList->SetGraphicsRootDescriptorTable(1, m_pMaterialHandles[i]->HandleGPU);	// ディスクリプタテーブルを設定

		commandList->DrawIndexedInstanced(static_cast<UINT>(m_ModelData.Meshes[i].Indices.size()), 1, 0, 0, 0);	// 描画
	}
}

void Model::UpdateBoneMatrix(std::string rootBone, aiMatrix4x4 matrix)
{
	Bone& bone = m_ModelData.Bones[rootBone];

	// 親の行列と自身の行列を掛け合わせる
	aiMatrix4x4 worldMatrix;

	worldMatrix *= matrix;					// 親の行列
	worldMatrix *= bone.AnimationMatrix;	// 自身の行列

	bone.Matrix = worldMatrix;				// ボーンの行列を更新
	bone.Matrix *= bone.OffsetMatrix;		// オフセット行列を掛け合わせる

	// 子ノードに対して再帰的に処理を行う
	for (size_t i = 0; i < bone.NumChildren; ++i)
	{
		UpdateBoneMatrix(bone.ChildBoneName, worldMatrix);
	}
}
