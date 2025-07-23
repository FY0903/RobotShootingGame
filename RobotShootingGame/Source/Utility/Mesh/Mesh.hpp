/*+===================================================================
	File: Mesh.hpp
	Summary: モデルの読み取りをするユーティリティ
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/23 15:48 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <d3d12.h>
#include <DirectXMath.h>
#include <string>
#include <vector>

// ==============================
//	structs
// ==============================
struct MeshVertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexCoord;
	DirectX::XMFLOAT3 Tangent;

	MeshVertex() = default;

	MeshVertex(DirectX::XMFLOAT3 const& position, DirectX::XMFLOAT3 const& normal, DirectX::XMFLOAT2 const& texcoord, DirectX::XMFLOAT3 const& tangent)
		: Position(position), Normal(normal), TexCoord(texcoord), Tangent(tangent){};

private:
	static const int InputElementCount = 4;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct Material
{
	DirectX::XMFLOAT3 Diffuse;	// 拡散反射色
	DirectX::XMFLOAT3 Specular;	// 鏡面反射色
	float Alpha;				// アルファ値
	float Shininess;			// 光沢度
	std::string DiffuseMap;		// 拡散反射マップのファイル名
};

struct Mesh
{
	std::vector<MeshVertex> Vertices;	// 頂点データ
	std::vector<uint32_t> Indices;		// インデックスデータ
	uint32_t MaterialId;				// マテリアルID
};

/**
 * @brief 指定されたファイルからメッシュとマテリアルを読み込みます。
 * @param [filename] 読み込むメッシュファイルのパス（ワイド文字列）。
 * @param [meshes] 読み込まれたメッシュ情報が格納される std::vector への参照。
 * @param [materials] 読み込まれたマテリアル情報が格納される std::vector への参照。
 * @return 読み込みに成功した場合は true、失敗した場合は false を返します。
 */
bool LoadMesh(const wchar_t* filename, std::vector<Mesh>& meshes, std::vector<Material>& materials);
