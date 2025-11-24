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
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
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
	Assimp::Importer importer;
	int flag = 0;
	flag |= aiProcess_Triangulate;              // 三角形化
	flag |= aiProcess_PreTransformVertices;     // 変換の適用
	flag |= aiProcess_CalcTangentSpace;         // 接線空間の計算
	flag |= aiProcess_GenSmoothNormals;			// スムースシェーディング用の法線を生成
	flag |= aiProcess_GenUVCoords;				// UV座標の生成
	flag |= aiProcess_RemoveRedundantMaterials;	// 冗長なマテリアルの削除
	flag |= aiProcess_OptimizeMeshes;			// メッシュの最適化

	auto scene = importer.ReadFile(fileName, flag);

	if (!scene)
	{
		auto error = importer.GetErrorString();
		OutputDebugStringA(error);
		assert(0 && "モデルの読み込みに失敗");
		return E_FAIL;
	}

	std::vector<Mesh> meshes;

	meshes.clear();
	meshes.resize(scene->mNumMeshes);

	// メッシュの読み込み
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		const auto pMesh = scene->mMeshes[i];
		LoadMesh(meshes[i], pMesh, inverseU, inverseV);
		const auto pMaterial = scene->mMaterials[i];
		LoadTexture(fileName, meshes[i], pMaterial);
	}

	scene = nullptr;

	m_Meshes = meshes;

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
	if (src->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)
	{
		auto dir = GetDirectoryPath(fineName);	// ディレクトリ名
		auto file = std::string(path.C_Str());	// ファイル名
		size_t idx = file.find_last_of('\\');	// 区切り文字を探す
		if (idx != std::string::npos)
		{
			file = file.substr(idx + 1);	// 区切り文字以降を取得

			dst.DiffuseMap = new Texture();
			if (FAILED(dst.DiffuseMap->Load(dir + file)))	// フルパスを設定
			{
				delete dst.DiffuseMap;
				dst.DiffuseMap = nullptr;
			}
		}
	}
	else
	{
		dst.DiffuseMap = nullptr;
	}
}
