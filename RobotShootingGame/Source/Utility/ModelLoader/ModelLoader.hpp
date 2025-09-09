/*+===================================================================
	File: ModelLoader.hpp
	Summary: モデルの読み取りをするユーティリティ
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/23 15:48 初回作成
			  /07/24 13:03 モデル読み込み機能を追加
			  /07/26 14:36 ファイル名の変更
===================================================================+*/
#pragma once
#define NOMINMAX

// ==============================
//	include
// ==============================
#include <d3d12.h>
#include <DirectXMath.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "../Singleton/Singleton.hpp"

// ==============================
//	前方宣言
// ==============================
struct Mesh;

struct aiMesh;
struct aiMaterial;

class ModelLoader : public Singleton<ModelLoader>
{
public:
	std::vector<Mesh> Load(const std::string& FileName, bool inverseU, bool inverseV);

private:
	/**
	 * @brief 頂点の変形情報を格納する構造体です。
	 */
	struct DeformVertex
	{
		aiVector3D Position{};		// 頂点座標
		aiVector3D Normal{};		// 法線ベクトル
		int BoneNum{};				// ボーンの数
		std::string BoneName[4]{};	// ボーン名
		float BoneWeight[4]{};		// ボーンの影響度
	};

	/**
	 * @brief ボーンの変形、アニメーション、オフセット行列を保持する構造体です。
	 */
	struct Bone
	{
		aiMatrix4x4 Matrix{}; // ボーンの変形行列
		aiMatrix4x4 AnimationMatrix{}; // アニメーションの変形行列
		aiMatrix4x4 OffsetMatrix{}; // ボーンのオフセット行列
	};

	friend class Singleton<ModelLoader>;
	/**
	 * @brief コンストラクタ
	 */
	ModelLoader() = default;

	/**
	 * @brief デストラクタ
	 */
	~ModelLoader() = default;

	void LoadMesh(Mesh& dst, const aiMesh* src, bool inverseU, bool inverseV);
	void LoadTexture(std::string FileName, Mesh& dst, const aiMaterial* src);
	void CreateBone(aiNode* node);

};
