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

HRESULT ModelLoader::Load(const std::string& FilePath)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(FilePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_MakeLeftHanded);

	ModelData model{};
	model.Meshes.clear();
	model.Bones.clear();

	ProcessNode(scene->mRootNode, scene, model, FilePath);

	return S_OK;
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
		}
	}

	return meshData;
}
