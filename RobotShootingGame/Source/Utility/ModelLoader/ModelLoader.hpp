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
#include <string>
#include <vector>

/**
 * @brief MeshVertexクラス
 */
class MeshVertex
{
public:
	/**
	 * @brief コンストラクタ
	 */
	MeshVertex() = default;

	/**
	 * @brief メッシュの頂点データを初期化します。
	 * @param [position] 頂点の位置ベクトル。
	 * @param [normal] 頂点の法線ベクトル。
	 * @param [texcoord] 頂点のテクスチャ座標。
	 * @param [tangent] 頂点の接線ベクトル。
	 */
	MeshVertex(DirectX::XMFLOAT3 const& position, DirectX::XMFLOAT3 const& normal, DirectX::XMFLOAT2 const& texcoord, DirectX::XMFLOAT3 const& tangent)
		: Position(position), Normal(normal), TexCoord(texcoord), Tangent(tangent){};

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;

	DirectX::XMFLOAT3 Position{};
	DirectX::XMFLOAT3 Normal{};
	DirectX::XMFLOAT2 TexCoord{};
	DirectX::XMFLOAT3 Tangent{};

private:
	static const int InputElementCount = 4;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

// ==============================
//	structs
// ==============================
/**
 * @brief マテリアルの物理的特性とテクスチャ情報を表す構造体です。
 */
struct Material
{
	DirectX::XMFLOAT3 Diffuse{};	// 拡散反射色
	DirectX::XMFLOAT3 Specular{};	// 鏡面反射色
	float Alpha{};					// アルファ値
	float Shininess{};				// 光沢度
	const char* DiffuseMap{};		// 拡散反射マップのファイル名
	const char* SpecularMap{};		// 鏡面反射マップのファイル名
	const char* ShininessMap{};		// 光沢度マップのファイル名
	const char* NormalMap{};		// 法線マップのファイル名
};

/**
 * @brief メッシュデータを表す構造体です。
 */
struct Mesh
{
	std::vector<MeshVertex> Vertices{};	// 頂点データ
	std::vector<uint32_t> Indices{};	// インデックスデータ
	uint32_t MaterialId{};				// マテリアルID
};

// ==============================
//	プロトタイプ宣言
// ==============================
/**
 * @brief 指定されたファイルからメッシュとマテリアルを読み込みます。
 * @param [filename] 読み込むメッシュファイルのパス。
 * @param [meshes] 読み込まれたメッシュ情報を格納する std::vector<Mesh> への参照。
 * @param [materials] 読み込まれたマテリアル情報を格納する std::vector<Material> への参照。
 * @return メッシュとマテリアルの読み込みに成功した場合は true、失敗した場合は false を返します。
 */
bool LoadMesh(const char* filename, std::vector<Mesh>& meshes, std::vector<Material>& materials);
