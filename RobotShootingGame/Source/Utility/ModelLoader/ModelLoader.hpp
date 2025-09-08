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
#include <string>
#include <vector>
#include "../Singleton/Singleton.hpp"

// ==============================
//	前方宣言
// ==============================
struct Mesh;
struct MeshVertex;

struct aiMesh;
struct aiMaterial;

struct ImportSettings
{
	const wchar_t* filename = nullptr;	// ファイルパス
	std::vector<Mesh>& meshes;			// メッシュデータの格納先
	bool inverseU = false;				// U座標を反転するか
	bool inverseV = false;				// V座標を反転するか
};

class ModelLoader : public Singleton<ModelLoader>
{
public:
	bool Load(const ImportSettings& settings);	// モデルの読み込み
	std::vector<Mesh> Load(const std::string& FileName, bool inverseU, bool inverseV);

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

	void LoadMesh(Mesh& dst, const aiMesh* src, bool inverseU, bool inverseV);
	void LoadTexture(const wchar_t* filename, Mesh& dst, const aiMaterial* src);
	void LoadTexture(std::string FileName, Mesh& dst, const aiMaterial* src);
};
