/*+===================================================================
	File: Model.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/24 10:31:21 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/SharedStruct/SharedStruct.hpp"
#include "Utility/Texture/Texture.hpp"
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// ==============================
//	前方宣言
// ==============================
struct aiMesh;
struct aiMaterial;
struct aiNode;
struct aiScene;

/**
 * @brief Modelクラス
 */
class Model
{
public:
	struct Mesh
	{
		std::vector<Vertex::Mesh> Vertices{};	// 頂点データ
		std::vector<uint32_t> Indices{};		// インデックスデータ
		Texture* DiffuseMap{};					// ディフューズマップ
	};

	struct Weight
	{
		std::string BoneName{};
		float WeightNum{};
		int VertexIndex{};
	};

	struct Bone
	{
		std::string BoneName{};				// ボーン名
		int index{};						// ボーンインデックス
		DirectX::XMMATRIX Matrix{};
		DirectX::XMMATRIX OffsetMatrix{};
		DirectX::XMMATRIX AnimationMatrix{};
		DirectX::XMMATRIX Blend1{};
		DirectX::XMMATRIX Blend2{};
		std::vector<Weight> Weights{};	// ウェイト情報
	};

	struct ModelOtherInfo
	{
		std::string MeshName{};
		unsigned int VertexNum{};
		unsigned int IndexNum{};
		unsigned int VertexBase{};
		unsigned int IndexBase{};
		unsigned int MaterialIndex{};
	};

	/**
	 * コンストラクタ
	 */
	Model() = default;

	/**
	 * デストラクタ
	 */
	~Model();

	HRESULT Load(const std::string& fileName, bool inverseU, bool inverseV);

	inline const aiScene* GetScene() { return m_pScene; }
	inline const std::vector<Mesh>& GetMeshes() { return m_Meshes; }
	inline std::unordered_map<std::string, Bone>& GetBones() { return m_Bones; }

private:
	void LoadMesh(Mesh& dst, const aiMesh* src, bool inverseU, bool inverseV);
	void LoadTexture(const std::string& fineName, Mesh& dst, const aiMaterial* src);

	void CreateBone(const aiNode* node);
	void GetBoneInfo(const aiMesh* src);

	void CalcMeshBaseIndex(std::vector<ModelOtherInfo>& modelInfo);
	void SetBoneDataToVertex(std::vector<Mesh>& meshes, std::vector<ModelOtherInfo>& modelInfo, std::vector<std::vector<Bone>>& meshBones);

	DirectX::XMMATRIX AiToXmMatrix(const aiMatrix4x4& aiMat);

	Assimp::Importer m_Importer;
	const aiScene* m_pScene{};							// シーン情報

	std::vector<Mesh> m_Meshes{};						// メッシュ群
	std::unordered_map<std::string, Bone> m_Bones{};	// ボーン情報
	std::vector<std::vector<Bone>> m_MeshBones{};		// メッシュごとのボーン情報
	std::vector<ModelOtherInfo> m_ModelOtherInfo{};		// モデルのその他情報
};
