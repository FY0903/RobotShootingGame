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
#include "../SharedStruct/SharedStruct.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../../../DirectXTex/d3dx12.h"
#include <filesystem>
#include <cassert>

namespace fs = std::filesystem;

std::wstring GetDirectoryPath(const std::wstring& filepath)
{
	fs::path path = filepath;

	return path.remove_filename().c_str();
}

std::string ToUTF8(const std::wstring& value)
{
    auto length = WideCharToMultiByte(CP_UTF8, 0U, value.data(), -1, nullptr, 0, nullptr, nullptr);
    auto buffer = new char[length];

    WideCharToMultiByte(CP_UTF8, 0U, value.data(), -1, buffer, length, nullptr, nullptr);

    std::string result(buffer);
    delete[] buffer;
    buffer = nullptr;

    return result;
}

std::wstring ToWideString(const std::string& str)
{
    auto num1 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str.c_str(), -1, nullptr, 0);

    std::wstring wstr;
    wstr.resize(num1);

    auto num2 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str.c_str(), -1, &wstr[0], num1);

    assert(num1 == num2);
    return wstr;
}

bool ModelLoader::Load(const ImportSettings& settings)
{
	if (!settings.filename) return false;

	auto& meshes = settings.meshes;
	auto inverseU = settings.inverseU;
	auto inverseV = settings.inverseV;

	auto path = ToUTF8(settings.filename);

	Assimp::Importer importer;
	int flag = 0;
	flag |= aiProcess_Triangulate;              // 三角形化
	flag |= aiProcess_PreTransformVertices;     // 変換の適用
	flag |= aiProcess_CalcTangentSpace;         // 接線空間の計算
	flag |= aiProcess_GenSmoothNormals;			// スムースシェーディング用の法線を生成
	flag |= aiProcess_GenUVCoords;				// UV座標の生成
	flag |= aiProcess_RemoveRedundantMaterials;	// 冗長なマテリアルの削除
	flag |= aiProcess_OptimizeMeshes;			// メッシュの最適化

	auto scene = importer.ReadFile(path, flag);

	if (!scene)
	{
		auto error = importer.GetErrorString();
		OutputDebugStringA(error);
		return false;
	}

	meshes.clear();
	meshes.resize(scene->mNumMeshes);

	// メッシュの読み込み
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		const auto pMesh = scene->mMeshes[i];
		LoadMesh(meshes[i], pMesh, inverseU, inverseV);
		const auto pMaterial = scene->mMaterials[i];
		LoadTexture(settings.filename, meshes[i], pMaterial);
	}

	scene = nullptr;

	return true;
}

void ModelLoader::LoadMesh(Mesh& dst, const aiMesh* src, bool inverseU, bool inverseV)
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

		MeshVertex vertex{};
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

void ModelLoader::LoadTexture(const wchar_t* filename, Mesh& dst, const aiMaterial* src)
{
	aiString path;
	if (src->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)
	{
		auto dir = GetDirectoryPath(filename);
		auto file = std::string(path.C_Str());
		size_t idx = file.find_last_of('\\');
		if (idx != std::wstring::npos)
		{
			file = file.substr(idx + 1);
			dst.DiffuseMap = dir + ToWideString(file);
		}
	}
	else
	{
		dst.DiffuseMap.clear();
	}
}
