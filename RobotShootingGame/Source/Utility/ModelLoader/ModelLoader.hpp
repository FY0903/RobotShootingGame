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
#include <map>
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

struct AnimePlayInfo
{
	float NowTime{};	// 現在の再生時間
	float TotalTime{};	// アニメーションの総時間
	float Speed{};		// 再生速度
	bool IsLoop{};		// ループ再生するかどうか
};

struct AnimeChannel
{
	int Node{};				// 対応するボーンのインデックス
	std::map<float, AnimeTransform> AnimeTransform{}; // キーフレームのリスト（時間、変形情報）
};

/**
 * @brief アニメーションのチャンネルと長さを表す構造体です。
 */
struct Animation
{
	//std::vector<Channel> Channels{};	// チャンネルのリスト
	AnimePlayInfo Info{};		// アニメーションの再生情報
	std::vector<AnimeChannel> Channels{}; // アニメーションチャンネルのリスト
};

/**
 * @brief 3Dモデルのメッシュとボーンデータを格納する構造体です。
 */
struct ModelData
{
	std::vector<Mesh> Meshes{}; // メッシュデータ
	std::vector<Bone> Bones{}; // ボーンデータ
	std::unordered_map<std::string, Bone> BoneMap{}; // ボーン名とボーンデータのマップ
	const aiScene* AiScene{}; // Assimpのシーンデータへのポインタ
};

class ModelLoader : public Singleton<ModelLoader>
{
public:
	ModelData Load(const std::string& FileName, bool inverseU, bool inverseV);
	HRESULT LoadAnimation(const std::string& FileName, ModelData& modelData, std::string name);
	HRESULT LoadAnimation(const std::string& FileName, std::string name);
	const aiScene* GetAnimation(const std::string& name)
	{
		if (m_Animations.find(name) == m_Animations.end())
		{
			return nullptr;
		}
		return m_Animations[name];
	}
private:
	friend class Singleton<ModelLoader>;
	/**
	 * @brief コンストラクタ
	 */
	ModelLoader() = default;

	/**
	 * @brief デストラクタ
	 */
	~ModelLoader();

	void MakeBoneHierarchy(aiNode* node, int parentIndex, std::vector<Bone>& bones);
	void MakeBoneHierarchy(aiNode* node, std::unordered_map<std::string, Bone>& bones);
	void LoadMesh(Mesh& dst, const aiMesh* src, bool inverseU, bool inverseV);
	void LoadDeformVertex(Mesh& dst, const aiMesh* src);
	void LoadBone(Mesh& dst, const aiMesh* src, std::vector<Bone>& bones);
	void LoadBone(Mesh& dst, const aiMesh* src, std::unordered_map<std::string, Bone>& bones);
	void LoadTexture(std::string FileName, Mesh& dst, const aiMaterial* src);


	std::unordered_map<std::string, Assimp::Importer*> m_Importers{}; // インポーター
	std::unordered_map<std::string, const aiScene*> m_Animations{}; // アニメーションデータ
};
