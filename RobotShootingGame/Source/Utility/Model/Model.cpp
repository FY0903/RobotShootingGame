/*+===================================================================
	File: Model.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/24 10:31:21 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Model.hpp"
#include <filesystem>

namespace fs = std::filesystem;

std::string GetDirectoryPath(const std::string& filepath)
{
	fs::path path = filepath;

	return path.remove_filename().string();
}

Model::~Model()
{
	for (auto& mesh : m_Meshes)
	{
		if (mesh.DiffuseMap)
		{
			delete mesh.DiffuseMap;
			mesh.DiffuseMap = nullptr;
		}
	}

	m_Meshes.clear();
}

HRESULT Model::Load(const std::string& fileName, bool inverseU, bool inverseV)
{
	int flag = 0;
	flag |= aiProcess_Triangulate;		// 三角形化
	flag |= aiProcess_MakeLeftHanded;	// 左手座標系に変換
	flag |= aiProcess_FlipUVs;			// UV反転

	auto scene = m_Importer.ReadFile(fileName, flag);

	if (!scene)
	{
		auto error = m_Importer.GetErrorString();
		OutputDebugStringA(error);
		assert(0 && "モデルの読み込みに失敗");
		return E_FAIL;
	}

	CreateBone(scene->mRootNode);

	unsigned int boneIndex = 0;
	for (auto& bone : m_Bones)
	{
		bone.second.index = boneIndex;
		++boneIndex;
	}

	std::vector<Mesh> meshes;

	meshes.clear();
	meshes.resize(scene->mNumMeshes);
	m_Meshes.clear();
	m_Meshes.resize(scene->mNumMeshes);

	m_ModelOtherInfo.clear();
	m_ModelOtherInfo.resize(scene->mNumMeshes);

	// メッシュの読み込み
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		const auto pMesh = scene->mMeshes[i];
		LoadMesh(meshes[i], pMesh, inverseU, inverseV);
		GetBoneInfo(pMesh);

		const auto pMaterial = scene->mMaterials[pMesh->mMaterialIndex];
		LoadTexture(fileName, meshes[i], pMaterial);

		m_ModelOtherInfo[i].MeshName = std::string(pMesh->mName.C_Str());
		m_ModelOtherInfo[i].VertexNum = meshes[i].Vertices.size();
		m_ModelOtherInfo[i].IndexNum = meshes[i].Indices.size();
		m_ModelOtherInfo[i].MaterialIndex = pMesh->mMaterialIndex;
	}

	CalcMeshBaseIndex(m_ModelOtherInfo);
	SetBoneDataToVertex(meshes, m_ModelOtherInfo, m_MeshBones);

	m_BoneMatCB.resize(m_Bones.size());

	m_Meshes = meshes;
	m_pScene = scene;

	return S_OK;
}

void Model::LoadMesh(Mesh& dst, const aiMesh* src, bool inverseU, bool inverseV)
{
	aiVector3D zero3D(0.0f, 0.0f, 0.0f);
	aiColor4D zeroColor(0.0f, 0.0f, 0.0f, 0.0f);

	dst.Vertices.resize(src->mNumVertices);

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

void Model::LoadTexture(const std::string& fineName, Mesh& dst, const aiMaterial* src)
{
	aiString path{};
	src->GetTexture(aiTextureType_DIFFUSE, 0, &path);

	if (path.length)
	{
		auto file = std::string(path.C_Str());
		size_t idx = file.find_last_of('\\');	// 区切り文字を探す

		if (idx == std::string::npos)
		{
			file = std::string('\\' + file);	// 区切り文字が無ければ追加
		}

		auto dir = GetDirectoryPath(fineName);			// ディレクトリ名
		file = std::string(dir + file.substr(idx + 1));	// フルパスを設定

		dst.DiffuseMap = new Texture();
		if (FAILED(dst.DiffuseMap->Load(file)))	// テクスチャの読み込み
		{
			delete dst.DiffuseMap;
			dst.DiffuseMap = nullptr;
		}
	}
	else
	{
		dst.DiffuseMap = nullptr;
	}
}

void Model::CreateBone(const aiNode* node)
{
	Bone bone{};
	m_Bones[node->mName.C_Str()] = bone;

	for (size_t i = 0; i < node->mNumChildren; ++i)
	{
		CreateBone(node->mChildren[i]);
	}
}

void Model::GetBoneInfo(const aiMesh* src)
{
	std::vector<Bone> bones{};

	for (unsigned int i = 0; i < src->mNumBones; ++i)
	{
		Bone bone{};
		
		bone.BoneName = std::string(src->mBones[i]->mName.C_Str());
		bone.OffsetMatrix = AiToXmMatrix(src->mBones[i]->mOffsetMatrix);

		bone.Weights.clear();

		for (unsigned int j = 0; j < src->mBones[i]->mNumWeights; ++j)
		{
			Weight weight{};
			weight.BoneName = bone.BoneName;
			weight.WeightNum = src->mBones[i]->mWeights[j].mWeight;
			weight.VertexIndex = src->mBones[i]->mWeights[j].mVertexId;
			bone.Weights.emplace_back(weight);
		}

		m_Bones[src->mBones[i]->mName.C_Str()].OffsetMatrix = bone.OffsetMatrix;

		bones.emplace_back(bone);
	}

	m_MeshBones.emplace_back(bones);
}

void Model::CalcMeshBaseIndex(std::vector<ModelOtherInfo>& modelInfo)
{
	for (int i = 0; i < modelInfo.size(); ++i)
	{
		for (int j = i - 1; j >= 0; --j)
		{
			modelInfo[i].VertexBase += modelInfo[j].VertexNum;
		}

		for (int j = i - 1; j >= 0; --j)
		{
			modelInfo[i].IndexBase += modelInfo[j].IndexNum;
		}
	}
}

void Model::SetBoneDataToVertex(std::vector<Mesh>& meshes, std::vector<ModelOtherInfo>& modelInfo, std::vector<std::vector<Bone>>& meshBones)
{
	for (auto& mesh : meshes)
	{
		for (auto& vertex : mesh.Vertices)
		{
			for (int i = 0; i < 4; ++i)
			{
				vertex.BoneIndex[i] = -1;
				vertex.BoneWeight[i] = 0.0f;
			}
		}
	}

	int meshIndex = 0;
	for (auto& bones : meshBones)
	{
		for (auto& bone : bones)
		{
			for (auto& weight : bone.Weights)
			{
				int& index = meshes[meshIndex].Vertices[weight.VertexIndex].BoneCount;
				if (index < 4)
				{
					meshes[meshIndex].Vertices[weight.VertexIndex].BoneIndex[index] = m_Bones[weight.BoneName].index;
					meshes[meshIndex].Vertices[weight.VertexIndex].BoneWeight[index] = weight.WeightNum;
					++index;
				}
			}
		}
		++meshIndex;
	}
}

DirectX::XMMATRIX Model::AiToXmMatrix(const aiMatrix4x4& aiMat)
{
	DirectX::XMMATRIX xmMat(
		aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
		aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
		aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
		aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4
	);
	return xmMat;
}
