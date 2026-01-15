/*+===================================================================
	File: Model.hpp
	Summary: Modelクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/24 10:31 初回作成
			26/01/15 19:03 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/SharedStruct/SharedStruct.hpp"
#include "Utility/Texture/Texture.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

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
	/**
	 * メッシュ構造体
	 */
	struct Mesh
	{
		std::vector<Vertex::Mesh> Vertices{};	// 頂点データ
		std::vector<uint32_t> Indices{};		// インデックスデータ
		Texture* DiffuseMap{};					// ディフューズマップ
	};

	/**
	 * ウェイト構造体
	 */
	struct Weight
	{
		std::string BoneName{};	// ボーン名
		float WeightNum{};		// ウェイト値
		int VertexIndex{};		// 頂点インデックス
	};

	/**
	 * ボーン構造体
	 */
	struct Bone
	{
		std::string BoneName{};				// ボーン名
		int index{};						// ボーンインデックス
		std::vector<Weight> Weights{};		// ウェイト情報
		DirectX::XMMATRIX Matrix = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX OffsetMatrix = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX AnimationMatrix = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX Blend1 = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX Blend2 = DirectX::XMMatrixIdentity();
	};

	/**
	 * モデルのその他情報構造体
	 */
	struct ModelOtherInfo
	{
		std::string MeshName{};			// メッシュ名
		unsigned int VertexNum{};		// 頂点数
		unsigned int IndexNum{};		// インデックス数
		unsigned int VertexBase{};		// 頂点ベース
		unsigned int IndexBase{};		// インデックスベース
		unsigned int MaterialIndex{};	// マテリアルインデックス
	};

	/**
	 * コンストラクタ
	 */
	Model() = default;

	/**
	 * デストラクタ
	 */
	~Model();

	/**
	 * @brief 指定されたファイルを読み込みます。U座標およびV座標を必要に応じて反転できます。
	 * @param fileName 読み込むファイルのパスまたは名前。
	 * @param inverseU U座標（横方向）を反転する場合は true。反転しない場合は false。
	 * @param inverseV V座標（縦方向）を反転する場合は true。反転しない場合は false。
	 * @return HRESULT を返します。成功時は通常 S_OK、失敗時は対応するエラーコードが返されます。
	 */
	HRESULT Load(const std::string& fileName, bool inverseU, bool inverseV);

	/**
	 * @brief シーン情報を取得します。
	 * @return シーン情報へのポインタを返します。
	 */
	inline const aiScene* GetScene() { return m_pScene; }

	/**
	 * @brief メッシュ群を取得します。
	 * @return メッシュ群への参照を返します。
	 */
	inline const std::vector<Mesh>& GetMeshes() { return m_Meshes; }

	/**
	 * @brief ボーン情報を取得します。
	 * @return ボーン情報への参照を返します。
	 */
	inline std::unordered_map<std::string, Bone>& GetBones() { return m_Bones; }

	/**
	 * @brief ボーンの数を取得します。
	 * @return ボーンの数を返します。
	 */
	inline const int GetBoneNum() { return static_cast<int>(m_Bones.size()); }

	/**
	 * @brief ボーン行列用定数バッファデータを取得します。
	 * @return ボーン行列用定数バッファデータへの参照を返します。
	 */
	inline std::vector<DirectX::XMMATRIX>& GetBoneMatCB() { return m_BoneMatCB; }

private:
	/**
	 * @brief メッシュを読み込みます。
	 * @param dst 読み込んだメッシュを格納する先
	 * @param src 読み込む元のメッシュ
	 * @param inverseU U座標（横方向）を反転する場合は true。反転しない場合は false。
	 * @param inverseV V座標（縦方向）を反転する場合は true。反転しない場合は false。
	 */
	void LoadMesh(Mesh& dst, const aiMesh* src, bool inverseU, bool inverseV);
	
	/**
	 * @brief テクスチャを読み込みます。
	 * @param fineName 読み込むテクスチャのファイル名
	 * @param dst 読み込んだテクスチャを格納するメッシュ
	 * @param src 読み込む元のマテリアル
	 */
	void LoadTexture(const std::string& fineName, Mesh& dst, const aiMaterial* src);

	/**
	 * @brief ボーンを作成します。
	 * @param node ボーン情報を持つノード
	 */
	void CreateBone(const aiNode* node);
	
	/**
	 * @brief ボーン情報を取得します。
	 * @param src ボーン情報を持つメッシュ
	 */
	void GetBoneInfo(const aiMesh* src);

	/**
	 * @brief メッシュのベースインデックスを計算します。
	 * @param modelInfo モデルのその他情報のベクトル
	 */
	void CalcMeshBaseIndex(std::vector<ModelOtherInfo>& modelInfo);
	
	/**
	 * @brief 頂点にボーンデータを設定します。
	 * @param meshes メッシュ群
	 * @param modelInfo モデルのその他情報のベクトル
	 * @param meshBones メッシュごとのボーン情報のベクトル
	 */
	void SetBoneDataToVertex(std::vector<Mesh>& meshes, std::vector<ModelOtherInfo>& modelInfo, std::vector<std::vector<Bone>>& meshBones);

	/**
	 * @brief aiMatrix4x4をDirectX::XMMATRIXに変換します。
	 * @param aiMat 変換するaiMatrix4x4
	 * @return 変換されたDirectX::XMMATRIXを返します。
	 */
	DirectX::XMMATRIX AiToXmMatrix(const aiMatrix4x4& aiMat);

	Assimp::Importer m_Importer;
	const aiScene* m_pScene{};							// シーン情報

	std::vector<Mesh> m_Meshes{};						// メッシュ群
	std::unordered_map<std::string, Bone> m_Bones{};	// ボーン情報
	std::vector<std::vector<Bone>> m_MeshBones{};		// メッシュごとのボーン情報
	std::vector<ModelOtherInfo> m_ModelOtherInfo{};		// モデルのその他情報

	std::vector<DirectX::XMMATRIX> m_BoneMatCB{};		// ボーン行列用定数バッファデータ
};
