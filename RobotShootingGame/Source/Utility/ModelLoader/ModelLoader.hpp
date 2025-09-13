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
#include "../SharedStruct/SharedStruct.hpp"

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
 * @brief ボーンの情報を保持する構造体です。
 */
struct Bone
{
	std::string Name{}; // ボーン名
	int ParentIndex{};	// 親ボーンのインデックス
	std::vector<int> ChildIndices{};	// 子ボーンのインデックスリスト

	aiMatrix4x4 Matrix{}; // ボーンの変形行列
	aiMatrix4x4 AnimationMatrix{}; // アニメーションの変形行列
	aiMatrix4x4 OffsetMatrix{}; // ボーンのオフセット行列
};

/**
 * @brief 3Dメッシュのデータを格納する構造体です。
 */
struct Mesh
{
	std::vector<Vertex::Mesh> Vertices{};	// 頂点データ
	std::vector<uint32_t> Indices{};	// インデックスデータ
	std::string DiffuseMap{};	// ディフューズマップのファイルパス
	std::string NormalMap{};	// 法線マップのファイルパス
	std::string SpecularMap{};	// スペキュラマップのファイルパス

	std::vector<DeformVertex> DeformVertices{}; // 変形頂点データ
};

/**
 * @brief 3Dアニメーションのキーフレームを表す構造体です。
 */
struct KeyFrame
{
	aiVector3D Position{}; // 位置
	aiQuaternion Rotation{}; // 回転
	aiVector3D Scaling{}; // スケーリング
};

/**
 * @brief ボーン名とキーフレームのリストを保持するチャンネル構造体です。
 */
struct Channel
{
	std::string BoneName{};				// ボーン名
	std::vector<KeyFrame> KeyFrames{};	// キーフレームのリスト
};

/**
 * @brief アニメーションのチャンネルと長さを表す構造体です。
 */
struct Animation
{
	std::vector<Channel> Channels{};	// チャンネルのリスト
};

/**
 * @brief 3Dモデルのメッシュとボーンデータを格納する構造体です。
 */
struct ModelData
{
	std::vector<Mesh> Meshes{}; // メッシュデータ
	std::vector<Bone> Bones{}; // ボーンデータ
	std::unordered_map<std::string, Animation> Animations{}; // アニメーションデータ
};

class ModelLoader : public Singleton<ModelLoader>
{
public:
	ModelData Load(const std::string& FileName, bool inverseU, bool inverseV);
	HRESULT LoadAnimation(const std::string& FileName, ModelData& modelData, std::string name);
private:
	friend class Singleton<ModelLoader>;
	/**
	 * @brief コンストラクタ
	 */
	ModelLoader() = default;

	/**
	 * @brief デストラクタ
	 */
	~ModelLoader() = default;

	void MakeBoneHierarchy(aiNode* node, int parentIndex, std::vector<Bone>& bones);
	void LoadMesh(Mesh& dst, const aiMesh* src, bool inverseU, bool inverseV);
	void LoadDeformVertex(Mesh& dst, const aiMesh* src);
	void LoadBone(Mesh& dst, const aiMesh* src, std::vector<Bone>& bones);
	void LoadBone(aiNode* node, int parent, std::vector<Bone>& bones);
	void LoadTexture(std::string FileName, Mesh& dst, const aiMaterial* src);
};
