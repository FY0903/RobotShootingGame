/*+===================================================================
	File: SharedStruct.hpp
	Summary: VBOやIBO、定数バッファなどで共有する構造体群のヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/05 12:56 初回作成
			26/01/16 11:11 コメント記載
===================================================================+*/
#pragma once

#include <d3d12.h>

// ==============================
//	constexpr
// ==============================
constexpr std::size_t D3D12_CONSTANT_BUFFER_ALIGNMENT = 256; // 定数バッファのアライメント

/**
 * @brief 頂点構造体群
 */
namespace Vertex
{
	/**
	 * @brief スプライト頂点構造体
	 */
	struct Sprite
	{
		DirectX::SimpleMath::Vector3 Position{};	// 頂点座標
		DirectX::SimpleMath::Vector2 UV{};			// UV座標

		static const D3D12_INPUT_LAYOUT_DESC InputLayout;

	private:
		static const int InputElementCount = 2;
		static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};

	/**
	 * @brief メッシュ頂点構造体
	 */
	struct Mesh
	{
		DirectX::XMFLOAT3 Position{};	// 頂点座標
		DirectX::XMFLOAT3 Normal{};		// 法線ベクトル
		DirectX::XMFLOAT2 UV{};			// UV座標
		DirectX::XMFLOAT3 Tangent{};	// 接線ベクトル
		DirectX::XMFLOAT4 Color{};		// 頂点カラー
		int BoneIndex[4]{};				// ボーンインデックス
		float BoneWeight[4]{};			// ボーンウェイト
		int BoneCount{};				// ボーン数

		static const D3D12_INPUT_LAYOUT_DESC InputLayout;

	private:
		static const int InputElementCount = 8;
		static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};

	/**
	 * @brief ライン頂点構造体
	 */
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

/**
 * @brief 定数バッファ構造体群
 */
namespace CB
{
	/**
	 * @brief ワールド・ビュー・プロジェクション行列構造体
	 */
	struct WVP
	{
		DirectX::XMFLOAT4X4 WorldMat{};		// ワールド行列
		DirectX::XMFLOAT4X4 ViewMat{};		// ビュー行列
		DirectX::XMFLOAT4X4 ProjMat{};		// 射影行列
	};

	/**
	 * @brief ボーン行列構造体
	 */
	struct BoneMatrix
	{
		DirectX::XMMATRIX BoneMat[400];		// ボーン行列配列
	};

	/**
	 * @brief 時間構造体
	 */
	struct Time
	{
		float DeltaTime{};
	};

	/**
	 * @brief 光源構造体
	 */
	struct Light
	{
		DirectX::XMFLOAT3 Position{};	// 光源の位置
		float Range{};					// 光源の範囲
		DirectX::XMFLOAT3 Direction{};	// 光源の方向
		float Angle{};					// スポットライトの角度
		DirectX::XMFLOAT4 Color{};		// 光源の色
	};

	/**
	 * @brief カメラ構造体
	 */
	struct Camera
	{
		DirectX::XMFLOAT4X4 invVMat{};		// カメラの逆ビュー行列
		DirectX::XMFLOAT4X4 invPMat{};		// カメラの逆射影行列
		DirectX::XMFLOAT4X4 PMat{};			// カメラの射影行列
	};

	/**
	 * @brief SSAOカーネル構造体
	 */
	struct SSAOKernel
	{
		DirectX::XMFLOAT3 Kernel[64];		// SSAO用カーネル
	};
}
