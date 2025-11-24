#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <vector>
#include <string>

// ==============================
//	constexpr
// ==============================
constexpr std::size_t D3D12_CONSTANT_BUFFER_ALIGNMENT = 256; // 定数バッファのアライメント

// 頂点構造体
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

		static const D3D12_INPUT_LAYOUT_DESC InputLayout;

	private:
		static const int InputElementCount = 5;
		static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};

	struct Line
	{
		DirectX::XMFLOAT3 Position{};	// 頂点座標
		DirectX::XMFLOAT4 Color{};	// 頂点カラー
		static const D3D12_INPUT_LAYOUT_DESC InputLayout;
	private:
		static const int InputElementCount = 2;
		static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};
}

// 定数バッファ構造体
namespace CB
{
	struct WVP
	{
		DirectX::XMFLOAT4X4 WorldMat{};		// ワールド行列
		DirectX::XMFLOAT4X4 ViewMat{};		// ビュー行列
		DirectX::XMFLOAT4X4 ProjMat{};		// 射影行列
	};
}
