/*+===================================================================
	File: ModelLoader.hpp
	Summary: モデルの読み取りをするユーティリティ
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/23 15:48 初回作成
			  /07/24 13:03 モデル読み込み機能を追加
			  /07/26 14:36 ファイル名の変更
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <d3d12.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <assimp/scene.h>
#include <string>
#include <vector>

#include "../Singleton/Singleton.hpp"
#include "../SharedStruct/SharedStruct.hpp"

struct Bone
{
	std::string Name{}; // ボーン名
	int ParentID{};		// 親ボーンのインデックス
	DirectX::SimpleMath::Matrix InverseBindPose{}; // 逆バインドポーズ行列
};

struct Mesh
{
	std::vector<Vertex::Mesh> Vertices{};	// 頂点データ
	std::vector<uint32_t> Indices{};		// インデックスデータ
	std::string DiffuseMap{};				// ディフューズマップのファイルパス
};

struct ModelData
{
	std::vector<Mesh> Meshes{};			// メッシュデータ
	std::vector<Bone> Bones{};			// ボーンデータ
};

class ModelLoader : public Singleton<ModelLoader>
{
public:
	HRESULT Load(const std::string& FilePath);

private:
	friend class Singleton<ModelLoader>;

	ModelLoader() = default;
	~ModelLoader() = default;

	void ProcessNode(aiNode* node, const aiScene* scene, ModelData& model, const std::string& directory);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory);
};