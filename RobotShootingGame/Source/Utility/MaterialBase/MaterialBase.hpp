/*+===================================================================
	File: MaterialBase.hpp
	Summary: MaterialBaseクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/30 01:11 初回作成
			26/01/15 18:55 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/Texture/Texture.hpp"
#include "Utility/DescriptorHeap/DescriptorHeap.hpp"
#include "Utility/RootSignature/RootSignature.hpp"
#include "Utility/PipelineState/PipelineState.hpp"
#include "Utility/ConstantBuffer/ConstantBuffer.hpp"
#include "System/Engine/Engine.hpp"

/**
 * @brief Materialクラス
 */
class MaterialBase
{
public:
	/**
	 * @brief 入力レイアウトタイプ列挙型
	 */
	enum InputLayoutType
	{
		None = -1,		// 入力レイアウトなし
		Mesh,			// メッシュ
		SkeletalMesh,	// スケルタルメッシュ
		Sprite,			// スプライト
		Line,			// ライン
		Num,
	};

	/**
	 * @brief MaterialBase のコンストラクタ。レンダーターゲット数とアルファブレンド設定でインスタンスを初期化します。
	 * @param renderTargetNum レンダーターゲットの数。
	 * @param alphaBlend アルファブレンドを有効にするかどうか。
	 */
	MaterialBase(size_t renderTargetNum = 1, bool alphaBlend = false);

	/**
	 * デストラクタ
	 */
	~MaterialBase();

	/**
	 * @brief 頂点シェーダーファイルパスを設定します。
	 * @param filepath 頂点シェーダーファイルのパス。
	 */
	inline void SetVSFilepath(const std::wstring& filepath) { m_VSFilepath = filepath; }
	
	/**
	 * @brief ピクセルシェーダーファイルパスを設定します。
	 * @param filepath ピクセルシェーダーファイルのパス。
	 */
	inline void SetPSFilepath(const std::wstring& filepath) { m_PSFilepath = filepath; }

	/**
	 * @brief ルートシグネチャに定数バッファビュー（CBV）を設定します。
	 * @param shaderRegister シェーダーレジスタ番号。
	 * @param visibility シェーダーの可視性。
	 */
	void SetCBV(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility);
	
	/**
	 * @brief ルートシグネチャに定数バッファビュー（CBV）を設定します。
	 * @param shaderRegister シェーダーレジスタ番号。
	 * @param numDescriptors ディスクリプタの数。
	 * @param visibility シェーダーの可視性。
	 */
	void SetCBV(UINT shaderRegister, UINT numDescriptors, D3D12_SHADER_VISIBILITY visibility);
	
	/**
	 * @brief ルートシグネチャにシェーダーリソースビュー（SRV）を設定します。
	 * @param shaderRegister シェーダーレジスタ番号。
	 * @param numDescriptors ディスクリプタの数。
	 * @param visibility シェーダーの可視性。
	 */
	void SetSRV(UINT shaderRegister, UINT numDescriptors, D3D12_SHADER_VISIBILITY visibility);
	
	/**
	 * @brief ルートシグネチャにスタティックサンプラーを設定します。
	 * @param shaderRegister シェーダーレジスタ番号。
	 * @param filter フィルタリングモード。
	 */
	void SetStaticSampler(UINT shaderRegister, D3D12_FILTER filter);
	
	/**
	 * @brief 入力レイアウトを設定します。
	 * @param layout 入力レイアウトの記述。
	 * @param type 入力レイアウトのタイプ。
	 */
	void SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout, InputLayoutType type);
	
	/**
	 * @brief プリミティブトポロジタイプを設定します。
	 * @param type プリミティブトポロジタイプ。
	 */
	void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);
	
	/**
	 * @brief レンダーターゲットビュー（RTV）フォーマットを設定します。
	 * @param format フォーマット。
	 * @param index レンダーターゲットのインデックス。
	 */
	void SetRTVFormat(DXGI_FORMAT format, size_t index);
	
	/**
	 * @brief デプスステンシルビュー（DSV）フォーマットを設定します。
	 * @param format フォーマット。
	 */
	void SetDSVFormat(DXGI_FORMAT format);

	/**
	 * @brief ルートシグネチャとパイプラインステートを作成します。
	 */
	void Create();

	/**
	 * @brief ルートシグネチャを取得します。
	 * @return ルートシグネチャへのポインタ。
	 */
	inline RootSignature* GetRootSignature() const { return m_pRootSignature; }
	
	/**
	 * @brief パイプラインステートを取得します。
	 * @return パイプラインステートへのポインタ。
	 */
	inline PipelineState* GetPipelineState() const { return m_pPSO; }

	/**
	 * @brief ルートパラメータのインデックスを取得します。
	 * @return ルートパラメータのインデックス。
	 */
	inline int GetRootParameterIndex() const { return m_rootParameterIndex; }

	/**
	 * @brief 入力レイアウトタイプを取得します。
	 * @return 入力レイアウトタイプ。
	 */
	inline InputLayoutType GetInputLayoutType() const { return m_InputLayoutType; }

private:
	std::wstring m_VSFilepath{};			// 頂点シェーダーファイルパス
	std::wstring m_PSFilepath{};			// ピクセルシェーダーファイルパス
	RootSignature* m_pRootSignature{};		// ルートシグネチャ
	PipelineState* m_pPSO{};				// パイプラインステート
	InputLayoutType m_InputLayoutType{};	// 入力レイアウトタイプ

	int m_rootParameterIndex{};				// ルートパラメータのインデックス
};
