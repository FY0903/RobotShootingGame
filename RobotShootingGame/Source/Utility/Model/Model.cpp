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
		auto texture = new Texture();
		assert(texture);	// nullptrチェック

		if (texture->Load(meshes[i].DiffuseMap) == S_OK)
		{
			auto handle = m_pDescriptorHeap->Register(texture->Resource(), texture->ViewDesc());
			m_pMaterialHandles.push_back(handle);
			m_pTextures.push_back(texture);
		}
		else
		{
			m_pMaterialHandles.push_back(nullptr);
			delete texture;
			texture = nullptr;
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

	m_pLine = new Line(Camera);
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

	// テクスチャの解放
	for (auto tex : m_pTextures)
	{
		delete tex;
		tex = nullptr;
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

	// ボーン変形行列の計算
	std::vector<DirectX::SimpleMath::Matrix> boneTransforms(m_ModelData.Bones.size(), DirectX::SimpleMath::Matrix::Identity);

	// 各ボーンの変形行列を計算
	for (size_t i = 0; i < anim.BoneAnimations.size(); ++i)
	{
		const BoneAnimation& boneAnim = anim.BoneAnimations[i];
		int boneIdx = FindBoneIndexByName(boneAnim.BoneName);			// ボーン名からインデックスを取得
		if (boneIdx < 0) continue;

		KeyFrame interpolated = InterpolateKeyFrame(boneAnim, flame);	// キーフレームの補間
		DirectX::SimpleMath::Matrix local =
			DirectX::SimpleMath::Matrix::CreateScale(interpolated.Scale) *
			DirectX::SimpleMath::Matrix::CreateFromQuaternion(interpolated.Rotation) *
			DirectX::SimpleMath::Matrix::CreateTranslation(interpolated.Position);

		boneTransforms[boneIdx] = local;	// ローカル変形行列を保存
	}

	// 階層構造を考慮してグローバル変形行列を計算
	for (size_t i = 0; i < m_ModelData.Bones.size(); ++i)
	{
		int parentIdx = m_ModelData.Bones[i].ParentID;
		if (parentIdx >= 0)
		{
			boneTransforms[i] = boneTransforms[i] * boneTransforms[parentIdx];
		}
	}

	std::function<void(int, DirectX::XMFLOAT3)> FuncDrawBone = [&](int BoneIdx, DirectX::XMFLOAT3 ParentPos) {
		Bone& bone = m_ModelData.Bones[BoneIdx];
		DirectX::SimpleMath::Vector3 pos;
		DirectX::XMStoreFloat3(&pos, DirectX::XMVector3TransformCoord(DirectX::XMVectorZero(), boneTransforms[BoneIdx]));

		// 親ボーンから子ボーンへ線を引く
		m_pLine->AddPoint(ParentPos, pos, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));

		// 子ボーンに対して再帰的に線を引く
		for (int childIdx : bone.ChildIDs)
		{
			FuncDrawBone(childIdx, pos);
		}
	};

	// ルートボーンから再帰的に線を引く
	m_pLine->Clear();
	FuncDrawBone(0, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pLine->Create();

	m_pLine->Update();
	m_pLine->Draw();

	VertexSkinning(boneTransforms);				// 頂点スキニング
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
		commandList->SetGraphicsRootConstantBufferView(1, m_pBoneBuffer[currentIndex]->GetAddress());		// 定数バッファを設定

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// プリミティブトポロジーを設定
		commandList->IASetVertexBuffers(0, 1, &vbView);								// 頂点バッファを設定
		commandList->IASetIndexBuffer(&ibView);										// インデックスバッファを設定

		if (m_pMaterialHandles[i])
		{
			commandList->SetDescriptorHeaps(1, &materialHeap);									// ディスクリプタヒープを設定
			commandList->SetGraphicsRootDescriptorTable(2, m_pMaterialHandles[i]->HandleGPU);	// ディスクリプタテーブルを設定
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

void Model::VertexSkinning(std::vector<DirectX::SimpleMath::Matrix> boneMat)
{
	for (size_t i = 0; i < m_ModelData.Meshes.size(); ++i)
	{
		for (size_t j = 0; j < m_ModelData.Meshes[i].Vertices.size(); ++j)
		{
			// 頂点情報を取得
			const auto& vertex = m_ModelData.Meshes[i].Vertices[j];
			DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&vertex.Position);

			// ボーンの影響を受ける頂点の変形
			float weight0 = vertex.BoneWeights[0];
			float weight1 = vertex.BoneWeights[1];
			float weight2 = vertex.BoneWeights[2];
			float weight3 = vertex.BoneWeights[3];

			// ボーンのインデックスからボーン情報を取得
			Bone& bone0 = m_ModelData.Bones[vertex.BoneIndices[0]];
			Bone& bone1 = m_ModelData.Bones[vertex.BoneIndices[1]];
			Bone& bone2 = m_ModelData.Bones[vertex.BoneIndices[2]];
			Bone& bone3 = m_ModelData.Bones[vertex.BoneIndices[3]];

			DirectX::XMMATRIX m0 = DirectX::XMMatrixMultiply(bone0.InverseBindPose, boneMat[vertex.BoneIndices[0]]);
			DirectX::XMMATRIX m1 = DirectX::XMMatrixMultiply(bone1.InverseBindPose, boneMat[vertex.BoneIndices[1]]);
			DirectX::XMMATRIX m2 = DirectX::XMMatrixMultiply(bone2.InverseBindPose, boneMat[vertex.BoneIndices[2]]);
			DirectX::XMMATRIX m3 = DirectX::XMMatrixMultiply(bone3.InverseBindPose, boneMat[vertex.BoneIndices[3]]);

			DirectX::XMMATRIX mat = m0 * weight0 + m1 * weight1 + m2 * weight2 + m3 * weight3;

			// 頂点の変形
			pos = DirectX::XMVector3Transform(pos, mat);
			DirectX::XMStoreFloat3(&m_ModelData.Meshes[i].Vertices[j].Position, pos);
		}

		// 頂点バッファの更新
		auto size = sizeof(Vertex::Mesh) * m_ModelData.Meshes[i].Vertices.size();
		auto vertices = m_ModelData.Meshes[i].Vertices.data();
		m_pVertexBuffers[i]->Update(size, vertices);
	}
}
