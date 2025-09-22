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

struct KeyFrame
{
	float Time{}; // キーフレームの時間
	DirectX::SimpleMath::Vector3 Position{}; // 位置
	DirectX::SimpleMath::Quaternion Rotation{}; // 回転
	DirectX::SimpleMath::Vector3 Scale{}; // スケール
};

struct BoneAnimation
{
	std::string BoneName{}; // ボーン名
	std::vector<KeyFrame> KeyFrames{}; // キーフレームの配列
};;

struct Animation
{
	std::string Name{}; // アニメーション名
	float Duration{}; // アニメーションの長さ
	float TicksPerSecond{}; // 1秒あたりのティック数
	std::vector<BoneAnimation> BoneAnimations{}; // ボーンアニメーションの配列
};

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
	HRESULT Load(const std::string& FilePath, ModelData& OutModel);
	HRESULT LoadAnimation(const std::string& FilePath, std::vector<Animation>& OutAnimations);

private:
	friend class Singleton<ModelLoader>;

	ModelLoader() = default;
	~ModelLoader() = default;

	void ProcessNode(aiNode* node, const aiScene* scene, ModelData& model, const std::string& directory);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory);
	void ProcessBones(const aiScene* scene, ModelData& model);
	void ProcessAnimations(const aiScene* scene, std::vector<Animation>& animations);
};