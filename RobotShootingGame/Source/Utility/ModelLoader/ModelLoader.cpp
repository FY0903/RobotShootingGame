/*+===================================================================
	File: ModelLoader.cpp
	Summary: モデルの読み取りをするユーティリティ
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/23 15:48 初回作成
			  /07/24 13:03 モデル読み込み機能を追加
			  /07/26 14:36 ファイル名の変更
===================================================================+*/

// ==============================
//	include
// ==============================
#include "ModelLoader.hpp"
#include "../../../DirectXTex/d3dx12.h"
#include <filesystem>
#include <cassert>

namespace fs = std::filesystem;

std::string GetDirectoryPath(const std::string& filepath)
{
	fs::path path = filepath;

	return path.remove_filename().string();
}

ModelData ModelLoader::Load(const std::string& FileName, bool inverseU, bool inverseV)
{
	Assimp::Importer importer;
	int flag = 0;
	flag |= aiProcess_Triangulate;		// 三角形化
	flag |= aiProcess_FlipUVs;			// UV反転
	flag |= aiProcess_MakeLeftHanded;	// 左手座標系

	auto scene = importer.ReadFile(FileName, flag);

	if (!scene)
	{
		auto error = importer.GetErrorString();
		OutputDebugStringA(error);
		MessageBox(NULL, error, "モデルの読み込みに失敗", MB_OK | MB_ICONERROR);
		return ModelData{};	// 空のモデルデータを返す
	}

	ModelData model{};

	auto& meshes = model.Meshes;
	auto& bones = model.Bones;
	model.RootNode = scene->mRootNode->mName.C_Str();
	meshes.resize(scene->mNumMeshes);

	// ボーンの生成
	CreateBone(scene->mRootNode, bones);

	// メッシュの読み込み
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		const auto pMesh = scene->mMeshes[i];
		LoadMesh(meshes[i], pMesh, inverseU, inverseV);
		LoadDeformVertex(meshes[i], pMesh);
		LoadBone(meshes[i], pMesh, bones);
		const auto pMaterial = scene->mMaterials[i];
		LoadTexture(FileName, meshes[i], pMaterial);
	}

	scene = nullptr;

	return model;
}

HRESULT ModelLoader::LoadAnimation(const std::string& FileName, ModelData& modelData, std::string name)
{
	Assimp::Importer importer;
	int flag = 0;
	flag |= aiProcess_Triangulate;		// 三角形化
	flag |= aiProcess_FlipUVs;			// UV反転
	flag |= aiProcess_MakeLeftHanded;	// 左手座標系

	auto scene = importer.ReadFile(FileName, flag);
	if (!scene)
	{
		auto error = importer.GetErrorString();
		OutputDebugStringA(error);
		MessageBox(NULL, error, "アニメーションの読み込みに失敗", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// アニメーションの読み込み
	for (size_t i = 0; i < scene->mNumAnimations; ++i)
	{
		auto animation = scene->mAnimations[i];

		// チャネルの読み込み
		for (size_t j = 0; j < animation->mNumChannels; ++j)
		{
			auto channel = animation->mChannels[j];
			
			Channel animChannel{};
			animChannel.BoneName = channel->mNodeName.C_Str();
			animChannel.KeyFrames.resize(channel->mNumPositionKeys);

			// キーフレームの読み込み
			for (size_t k = 0; k < channel->mNumPositionKeys; ++k)
			{
				auto positionKey = channel->mPositionKeys[k];
				auto rotationKey = channel->mRotationKeys[k];
				auto scalingKey = channel->mScalingKeys[k];
			
				KeyFrame keyFrame{};
				keyFrame.Position = positionKey.mValue;
				keyFrame.Rotation = rotationKey.mValue;
				keyFrame.Scaling = scalingKey.mValue;
				animChannel.KeyFrames[k] = keyFrame;
			}
			modelData.Animations[name].Channels.push_back(animChannel);
		}
	}
}

void ModelLoader::LoadMesh(Mesh& dst, const aiMesh* src, bool inverseU, bool inverseV)
{
	aiVector3D zero3D(0.0f, 0.0f, 0.0f);
	aiColor4D zeroColor(0.0f, 0.0f, 0.0f, 0.0f);

	dst.Vertices.resize(src->mNumVertices);

	// 頂点の数だけループ
	for (size_t i = 0; i < src->mNumVertices; ++i)
	{
		auto position = &(src->mVertices[i]);
		auto normal = &(src->mNormals[i]);
		auto uv = (src->HasTextureCoords(0)) ? &(src->mTextureCoords[0][i]) : &zero3D;
		auto tangent = (src->HasTangentsAndBitangents()) ? &(src->mTangents[i]) : &zero3D;
		auto color = (src->HasVertexColors(0)) ? &(src->mColors[0][i]) : &zeroColor;

		// 反転オプションが有効であれば反転させる
		if (inverseU) uv->x = 1.0f - uv->x;
		if (inverseV) uv->y = 1.0f - uv->y;

		Vertex::Mesh vertex{};
		vertex.Position = { position->x, position->y, position->z };
		vertex.Normal = { normal->x, normal->y, normal->z };
		vertex.UV = { uv->x, uv->y };
		vertex.Tangent = { tangent->x, tangent->y, tangent->z };
		vertex.Color = { color->r, color->g, color->b, color->a };

		dst.Vertices[i] = vertex;
	}

	dst.Indices.resize(src->mNumFaces * 3);	// 三角形化しているので3倍

	for (size_t i = 0; i < src->mNumFaces; ++i)
	{
		auto face = &(src->mFaces[i]);
		assert(face->mNumIndices == 3);	// 三角形化しているので3であるはず

		for (size_t j = 0; j < face->mNumIndices; ++j)
		{
			dst.Indices[i * 3 + j] = face->mIndices[j];
		}
	}
}

void ModelLoader::LoadDeformVertex(Mesh& dst, const aiMesh* src)
{
	// 頂点の数だけループ
	for (size_t i = 0; i < src->mNumVertices; ++i)
	{
		// 変形頂点の追加
		DeformVertex deformVertex{};
		deformVertex.Position = src->mVertices[i];
		deformVertex.Normal = src->mNormals[i];
		deformVertex.BoneNum = 0;

		// ボーンの影響度を格納
		for (size_t j = 0; j < 4; ++j)
		{
			deformVertex.BoneName[j].clear();
			deformVertex.BoneWeight[j] = 0.0f;
		}

		dst.DeformVertices.push_back(deformVertex);	// 変形頂点の追加
	}
}

void ModelLoader::LoadBone(Mesh& dst, const aiMesh* src, std::unordered_map<std::string, Bone>& bones)
{
	// ボーンの数だけループ
	for (size_t i = 0; i < src->mNumBones; ++i)
	{
		// ボーンデータの取得
		auto bone = src->mBones[i];				// ボーン
		auto boneName = bone->mName.C_Str();	// ボーン名

		// ボーンのオフセット行列を格納
		if (bones.find(boneName) != bones.end())
		{
			bones[boneName].OffsetMatrix = bone->mOffsetMatrix;
		}

		// ボーンの影響度を格納
		for (size_t j = 0; j < bone->mNumWeights; ++j)
		{
			auto weight = bone->mWeights[j];					// ボーンの影響度
			auto vertexId = weight.mVertexId;					// 頂点番号
			auto vertexWeight = weight.mWeight;					// 頂点の影響度
			int boneNum = dst.DeformVertices[vertexId].BoneNum;	// ボーンの数

			// 4つまでしか格納できない
			if (boneNum < 4)
			{
				dst.DeformVertices[vertexId].BoneName[boneNum] = boneName;
				dst.DeformVertices[vertexId].BoneWeight[boneNum] = vertexWeight;
				dst.DeformVertices[vertexId].BoneNum++;
			}
		}
	}
}

void ModelLoader::LoadTexture(std::string FileName, Mesh& dst, const aiMaterial* src)
{
	aiString path;
	if (src->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)
	{
		auto dir = GetDirectoryPath(FileName);	// ディレクトリ名
		auto file = std::string(path.C_Str());	// ファイル名
		size_t idx = file.find_last_of('\\');	// 区切り文字を探す
		if (idx != std::string::npos)
		{
			file = file.substr(idx + 1);	// 区切り文字以降を取得
			dst.DiffuseMap = dir + file;	// フルパスを設定
		}
	}
	else
	{
		dst.DiffuseMap.clear();
	}
}

void ModelLoader::CreateBone(aiNode* node, std::unordered_map<std::string, Bone>& bones)
{
	Bone bone{};
	bones[node->mName.C_Str()] = bone;	// ボーンの追加
	bones[node->mName.C_Str()].NumChildren = node->mNumChildren; // 子ボーンの数を格納

	// 子ノードの探索
	for (size_t i = 0; i < node->mNumChildren; ++i)
	{
		bones[node->mName.C_Str()].ChildBoneName = node->mChildren[i]->mName.C_Str(); // 子ボーンの名前を格納
		CreateBone(node->mChildren[i], bones);	// 再帰呼び出し
	}
}
