#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <vector>
#include <string>

#define MAX_BONES (64)

namespace Vertex
{
	struct Sprite
	{
		DirectX::SimpleMath::Vector3 Position{};	// 頂点座標
		DirectX::SimpleMath::Vector2 UV{};		// UV座標

		static const D3D12_INPUT_LAYOUT_DESC InputLayout;

	private:
		static const int InputElementCount = 2;
		static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};

	struct Mesh
	{
		DirectX::XMFLOAT3 Position{};	// 頂点座標
		DirectX::XMFLOAT3 Normal{};	// 法線ベクトル
		DirectX::XMFLOAT2 UV{};		// UV座標
		DirectX::XMFLOAT3 Tangent{};	// 接線ベクトル
		DirectX::XMFLOAT4 Color{};	// 頂点カラー
		int BoneIndices[4]{};		// ボーンのインデックス
		float BoneWeights[4]{};		// ボーンの重み
		static const D3D12_INPUT_LAYOUT_DESC InputLayout;

	private:
		static const int InputElementCount = 7;
		static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};

	struct Line
	{
		DirectX::SimpleMath::Vector3 Position{};	// 頂点座標
		DirectX::SimpleMath::Vector4 Color{};	// 頂点カラー
		static const D3D12_INPUT_LAYOUT_DESC InputLayout;
	private:
		static const int InputElementCount = 2;
		static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};
}

struct alignas(256) Transform
{
	DirectX::XMMATRIX World{};	// ワールド行列
	DirectX::XMMATRIX View{};		// ビュー行列
	DirectX::XMMATRIX Proj{};		// 射影行列
};

struct alignas(256) BoneTransform
{
	DirectX::SimpleMath::Matrix BoneMatrices[MAX_BONES]{}; // ボーンの変形行列
};
