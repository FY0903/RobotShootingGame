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
#include <vector>
#include <string>
#include <unordered_map>

// ==============================
//	前方宣言
// ==============================
struct aiMesh;
struct aiMaterial;
struct aiNode;

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

	struct Bone
	{

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

	inline const std::vector<Mesh>& GetMeshes() const { return m_Meshes; }

private:
	void LoadMesh(Mesh& dst, const aiMesh* src, bool inverseU, bool inverseV);
	void LoadTexture(const std::string& fineName, Mesh& dst, const aiMaterial* src);

	void CreateBone(aiNode* node);

	std::vector<Mesh> m_Meshes{};
	std::unordered_map<std::string, Bone> m_Bones{};
};
