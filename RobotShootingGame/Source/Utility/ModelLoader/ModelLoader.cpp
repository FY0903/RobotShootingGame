// ==============================
//	include
// ==============================
#include "ModelLoader.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <filesystem>

namespace fs = std::filesystem;

std::string GetDirectoryPath(const std::string& filepath)
{
	fs::path path = filepath;

	return path.remove_filename().string();
}

std::string CreateLocalPath(const std::string& directory, const std::string& filename)
{
	auto dir = GetDirectoryPath(directory);	// ディレクトリ名
	size_t idx = filename.find_last_of('\\');	// 区切り文字を探す

	if (idx != std::string::npos)
	{
		return dir + filename.substr(idx + 1);	// フルパスを指定
	}
	else
		return std::string();	// 空白で返す
}

HRESULT ModelLoader::Load(const std::string& FilePath, ModelData& OutModel)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(FilePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_MakeLeftHanded);

	if (!scene)
	{
		auto error = importer.GetErrorString();
		OutputDebugStringA(error);
		MessageBox(NULL, error, "モデルの読み込みに失敗", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	OutModel.Meshes.clear();
	OutModel.Bones.clear();

	ProcessNode(scene->mRootNode, scene, OutModel, FilePath);

	ProcessBones(scene, OutModel);

	return S_OK;
}

HRESULT ModelLoader::LoadAnimation(const std::string& FilePath, std::vector<Animation>& OutAnimations)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(FilePath, aiProcess_FlipUVs | aiProcess_MakeLeftHanded);

	if (!scene)
	{
		auto error = importer.GetErrorString();
		OutputDebugStringA(error);
		MessageBox(NULL, error, "アニメーションの読み込みに失敗", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	ProcessAnimations(scene, OutAnimations);
}

void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, ModelData& model, const std::string& directory)
{
	// ノード内のメッシュを処理
	for (size_t i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		model.Meshes.push_back(ProcessMesh(mesh, scene, directory));
	}

	// 子ノードを再帰的に処理
	for (size_t i = 0; i < node->mNumChildren; ++i)
	{
		ProcessNode(node->mChildren[i], scene, model, directory);
	}
}

Mesh ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory)
{
	Mesh meshData{};

	// 頂点データの読み込み
	for (size_t i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex::Mesh vertex{};

		// 頂点座標
		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;

		// 法線ベクトル
		if (mesh->HasNormals())
		{
			vertex.Normal.x = mesh->mNormals[i].x;
			vertex.Normal.y = mesh->mNormals[i].y;
			vertex.Normal.z = mesh->mNormals[i].z;
		}

		// UV座標
		if (mesh->mTextureCoords[0])
		{
			vertex.UV.x = mesh->mTextureCoords[0][i].x;
			vertex.UV.y = mesh->mTextureCoords[0][i].y;
		}

		// 接線ベクトル
		if (mesh->HasTangentsAndBitangents())
		{
			vertex.Tangent.x = mesh->mTangents[i].x;
			vertex.Tangent.y = mesh->mTangents[i].y;
			vertex.Tangent.z = mesh->mTangents[i].z;
		}

		// 頂点カラー
		if (mesh->HasVertexColors(0))
		{
			vertex.Color.x = mesh->mColors[0][i].r;
			vertex.Color.y = mesh->mColors[0][i].g;
			vertex.Color.z = mesh->mColors[0][i].b;
			vertex.Color.w = mesh->mColors[0][i].a;
		}

		meshData.Vertices.push_back(vertex);
	}

	// インデックスデータの読み込み
	meshData.Indices.resize(mesh->mNumFaces * 3); // 三角形化しているので3倍

	for (size_t i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace* face = &mesh->mFaces[i];
		assert(face->mNumIndices == 3); // 三角形化しているので3であるはず

		for (size_t j = 0; j < face->mNumIndices; ++j)
		{
			meshData.Indices[i * 3 + j] = face->mIndices[j];
		}
	}

	// マテリアルの読み込み
	if (mesh->mMaterialIndex >= 0)
	{
		// マテリアルの取得
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// ディフューズマップの取得
		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString texPath;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
			{
				meshData.DiffuseMap = CreateLocalPath(directory, texPath.C_Str());
			}
			else
			{
				meshData.DiffuseMap = "";
			}
		}
	}

	return meshData;
}

void ModelLoader::ProcessBones(const aiScene* scene, ModelData& model)
{
	std::unordered_map<std::string, int> boneNameToIndex;

	// 階層をフラットな配列に変換
	std::function<void(aiNode*, int)> traverse = [&](aiNode* node, int parentIdx) {
		Bone bone;
		bone.Name = node->mName.C_Str();
		bone.ParentID = parentIdx;
		bone.InverseBindPose = DirectX::SimpleMath::Matrix::Identity; // 後で aiBone から取得
		int idx = static_cast<int>(model.Bones.size());
		boneNameToIndex[bone.Name] = idx;
		model.Bones.push_back(bone);
		for (unsigned i = 0; i < node->mNumChildren; ++i)
			traverse(node->mChildren[i], idx);
	};

	// ルートノードから開始
	traverse(scene->mRootNode, -1);

	// aiBone から逆バインドポーズ行列を取得
	for (unsigned m = 0; m < scene->mNumMeshes; ++m) {
		aiMesh* mesh = scene->mMeshes[m];
		for (unsigned b = 0; b < mesh->mNumBones; ++b) {
			aiBone* aiBone = mesh->mBones[b];
			auto it = boneNameToIndex.find(aiBone->mName.C_Str());
			if (it != boneNameToIndex.end()) {
				DirectX::SimpleMath::Matrix mat;
				memcpy(&mat, &aiBone->mOffsetMatrix, sizeof(mat));
				model.Bones[it->second].InverseBindPose = mat;
			}

			// 頂点にボーンの影響度を設定
			int boneIdx = it != boneNameToIndex.end() ? it->second : -1;
			if (boneIdx < 0) continue; // 見つからなければスキップ

			for (unsigned w = 0; w < aiBone->mNumWeights; ++w) {
				unsigned vIdx = aiBone->mWeights[w].mVertexId;
				float weight = aiBone->mWeights[w].mWeight;
				for (int k = 0; k < 4; ++k) {
					if (model.Meshes[m].Vertices[vIdx].BoneWeights[k] == 0.0f) {
						model.Meshes[m].Vertices[vIdx].BoneIndices[k] = boneIdx;
						model.Meshes[m].Vertices[vIdx].BoneWeights[k] = weight;
						break;
					}
				}
			}
		}
	}
}

void ModelLoader::ProcessAnimations(const aiScene* scene, std::vector<Animation>& animations)
{
	for (size_t i = 0; i < scene->mNumAnimations; ++i)
	{
		aiAnimation* aiAnim = scene->mAnimations[i];
		Animation anim{};
		anim.Name = aiAnim->mName.C_Str();
		anim.Duration = static_cast<float>(aiAnim->mDuration);
		anim.TicksPerSecond = static_cast<float>(aiAnim->mTicksPerSecond);

		// ボーンアニメーションの処理
		for (size_t j = 0; j < aiAnim->mNumChannels; ++j)
		{
			aiNodeAnim* channel = aiAnim->mChannels[j];
			BoneAnimation boneAnim{};
			boneAnim.BoneName = channel->mNodeName.C_Str();

			unsigned int keyCount = (std::max)({ channel->mNumPositionKeys, channel->mNumRotationKeys, channel->mNumScalingKeys });

			for (unsigned int k = 0; k < keyCount; ++k) {
				KeyFrame key;
				// 位置
				if (k < channel->mNumPositionKeys) {
					key.Time = channel->mPositionKeys[k].mTime;
					key.Position = DirectX::SimpleMath::Vector3(
						channel->mPositionKeys[k].mValue.x,
						channel->mPositionKeys[k].mValue.y,
						channel->mPositionKeys[k].mValue.z
					);
				}
				// 回転
				if (k < channel->mNumRotationKeys) {
					key.Rotation = DirectX::SimpleMath::Quaternion(
						channel->mRotationKeys[k].mValue.x,
						channel->mRotationKeys[k].mValue.y,
						channel->mRotationKeys[k].mValue.z,
						channel->mRotationKeys[k].mValue.w
					);
				}
				// スケール
				if (k < channel->mNumScalingKeys) {
					key.Scale = DirectX::SimpleMath::Vector3(
						channel->mScalingKeys[k].mValue.x,
						channel->mScalingKeys[k].mValue.y,
						channel->mScalingKeys[k].mValue.z
					);
				}
				boneAnim.KeyFrames.push_back(key);
			}
			anim.BoneAnimations.push_back(boneAnim);
		}
		animations.push_back(anim);
	}
}
