#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>
#include <string>

struct Vertex
{
	DirectX::XMFLOAT3 Position;	// 頂点座標
	DirectX::XMFLOAT3 Normal;	// 法線ベクトル
	DirectX::XMFLOAT2 UV;		// UV座標
	DirectX::XMFLOAT3 Tangent;	// 接線ベクトル
	DirectX::XMFLOAT4 Color;	// 頂点カラー

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;

private:
	static const int InputElementCount = 5;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct alignas(256) Transform
{
	DirectX::XMMATRIX World;	// ワールド行列
	DirectX::XMMATRIX View;		// ビュー行列
	DirectX::XMMATRIX Proj;		// 射影行列
};

struct Mesh
{
	std::vector<Vertex> Vertices;	// 頂点データ
	std::vector<uint32_t> Indices;	// インデックスデータ
	std::wstring DiffuseMap;		// ディフューズマップのファイルパス
};