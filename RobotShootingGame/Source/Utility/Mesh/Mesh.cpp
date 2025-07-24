/*+===================================================================
	File: Mesh.cpp
	Summary: モデルの読み取りをするユーティリティ
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/23 15:48 初回作成
			  /07/24 13:03 モデル読み込み機能を追加
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Mesh.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <codecvt>
#include <cassert>
#include <iostream>

// ==============================
//	defines
// ==============================
#define FMT_FLOAT3 DXGI_FORMAT_R32G32B32_FLOAT
#define FMT_FLOAT2 DXGI_FORMAT_R32G32_FLOAT
#define APPEND D3D12_APPEND_ALIGNED_ELEMENT
#define IL_VERTEX D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA

// ==============================
//	const
// ==============================
const D3D12_INPUT_ELEMENT_DESC MeshVertex::InputElements[] = {
	{ "POSITION", 0, FMT_FLOAT3, 0, APPEND, IL_VERTEX, 0 },
	{ "NORMAL", 0, FMT_FLOAT3, 0, APPEND, IL_VERTEX, 0 },
	{ "TEXCOORD", 0, FMT_FLOAT2, 0, APPEND, IL_VERTEX, 0 },
	{ "TANGENT", 0, FMT_FLOAT3, 0, APPEND, IL_VERTEX, 0 }
};

const D3D12_INPUT_LAYOUT_DESC MeshVertex::InputLayout = {
	MeshVertex::InputElements,
	MeshVertex::InputElementCount
};

static_assert(sizeof(MeshVertex) == 44, "Vertex struct/layout mismatch");

namespace {
	/**
	 * @brief MeshLoaderクラス
	 */
	class MeshLoader
	{
	public:
		MeshLoader() = default;
		~MeshLoader() = default;

		bool Load(const char* filename, std::vector<Mesh>& meshes, std::vector<Material>& materials);

	private:
		void ParseMesh(Mesh& dstMesh, const aiMesh* pSrcMesh);
		void ParseMaterial(Material& dstMaterial, const aiMaterial* pSrcMaterial);
	};
	
	bool MeshLoader::Load(const char* filename, std::vector<Mesh>& meshes, std::vector<Material>& materials)
	{
		if (!filename) return false;

		Assimp::Importer importer;
		int flag = 0;
		flag |= aiProcess_Triangulate; // 三角形に変換
		flag |= aiProcess_JoinIdenticalVertices; // 同一頂点の結合
		flag |= aiProcess_CalcTangentSpace; // 接線空間の計算
		flag |= aiProcess_GenSmoothNormals; // スムーズ法線の生成
		flag |= aiProcess_GenUVCoords; // UV座標の生成
		flag |= aiProcess_RemoveRedundantMaterials;	 // 冗長なマテリアルの削除
		flag |= aiProcess_OptimizeMeshes; // メッシュの最適化

		// ファイルを読み込む
		auto pScene = importer.ReadFile(filename, flag);
		if (!pScene) {
			std::cerr << "Assimp読み込み失敗: " << importer.GetErrorString() << std::endl;
			return false;
		}

		// 読み込みに失敗した場合
		if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mNumMeshes)
		{
			return false; // 読み込み失敗
		}

		// メッシュのメモリを確保
		meshes.clear();
		meshes.resize(pScene->mNumMeshes);

		// メッシュデータを変換
		for (size_t i = 0; i < meshes.size(); ++i)
		{
			const auto pMesh = pScene->mMeshes[i];
			ParseMesh(meshes[i], pMesh);
		}

		// マテリアルのメモリを確保
		materials.clear();
		materials.resize(pScene->mNumMaterials);

		// マテリアルデータを変換
		for (size_t i = 0; i < materials.size(); ++i)
		{
			const auto pMaterial = pScene->mMaterials[i];
			ParseMaterial(materials[i], pMaterial);
		}

		// 不要になったのでクリア
		pScene = nullptr;

		return true; // 読み込み成功
	}

	void MeshLoader::ParseMesh(Mesh& dstMesh, const aiMesh* pSrcMesh)
	{
		// マテリアル番号を設定
		dstMesh.MaterialId = pSrcMesh->mMaterialIndex;

		aiVector3D zero3D(0.0f, 0.0f, 0.0f);

		// 頂点データのメモリを確保
		dstMesh.Vertices.resize(pSrcMesh->mNumVertices);

		// 頂点データを変換
		for (uint32_t i = 0; i < pSrcMesh->mNumVertices; ++i)
		{
			auto pPosition = &(pSrcMesh->mVertices[i]);
			auto oNormal = &(pSrcMesh->mNormals[i]);
			auto pTexCoord = (pSrcMesh->HasTextureCoords(0)) ? &(pSrcMesh->mTextureCoords[0][i]) : &zero3D;
			auto pTangent = (pSrcMesh->HasTangentsAndBitangents()) ? &(pSrcMesh->mTangents[i]) : &zero3D;

			dstMesh.Vertices[i] = MeshVertex(
				DirectX::XMFLOAT3(pPosition->x, pPosition->y, pPosition->z),
				DirectX::XMFLOAT3(oNormal->x, oNormal->y, oNormal->z),
				DirectX::XMFLOAT2(pTexCoord->x, pTexCoord->y),
				DirectX::XMFLOAT3(pTangent->x, pTangent->y, pTangent->z)
			);
		}

		// 頂点インデックスのメモリを確保
		dstMesh.Indices.resize(pSrcMesh->mNumFaces * 3);

		for (uint32_t i = 0; i < pSrcMesh->mNumFaces; ++i)
		{
			const auto& face = pSrcMesh->mFaces[i];
			assert(face.mNumIndices == 3); // Assimpは三角形メッシュを前提としている
			
			for (uint32_t j = 0; j < face.mNumIndices; ++j)
			{
				dstMesh.Indices[i * 3 + j] = face.mIndices[j];
			}
		}

	}

	void MeshLoader::ParseMaterial(Material& dstMaterial, const aiMaterial* pSrcMaterial)
	{
		aiColor3D color(0.0f, 0.0f, 0.0f);

		// ==============================
		//	拡散反射色の取得
		// ==============================
		if (pSrcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
		{
			dstMaterial.Diffuse = DirectX::XMFLOAT3(color.r, color.g, color.b);
		}
		else
		{
			dstMaterial.Diffuse = DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f); // デフォルト値
		}

		// ==============================
		//	鏡面反射色の取得
		// ==============================
		color = aiColor3D(0.0f, 0.0f, 0.0f);
		if (pSrcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
		{
			dstMaterial.Specular = DirectX::XMFLOAT3(color.r, color.g, color.b);
		}
		else
		{
			dstMaterial.Specular = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f); // デフォルト値
		}

		// ==============================
		//	鏡面反射の光沢度の取得
		// ==============================
		float shininess = 0.0f;
		if (pSrcMaterial->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
		{
			dstMaterial.Shininess = shininess;
		}
		else
		{
			dstMaterial.Shininess = 0.0f; // デフォルト値
		}

		// ==============================
		//	ディフューズマップの取得
		// ==============================
		aiString path;
		if (pSrcMaterial->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)
		{
			dstMaterial.DiffuseMap = std::string(path.C_Str());
		}
		else
		{
			dstMaterial.DiffuseMap.clear(); // デフォルト値
		}
	}
}

bool LoadMesh(const char* filename, std::vector<Mesh>& meshes, std::vector<Material>& materials)
{
	MeshLoader loader;
	return loader.Load(filename, meshes, materials);
}
