/*+===================================================================
	File: MaterialBase.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/30 1:11:33 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/Texture/Texture.hpp"
#include "Utility/DescriptorHeap/DescriptorHeap.hpp"
#include "Utility/RootSignature/RootSignature.hpp"
#include "Utility/PipelineState/PipelineState.hpp"
#include <vector>
#include <string>

/**
 * @brief Materialクラス
 */
class MaterialBase
{
public:
	/**
	 * コンストラクタ
	 */
	MaterialBase();

	/**
	 * デストラクタ
	 */
	~MaterialBase();

	inline void SetVSFilepath(const std::wstring& filepath) { m_VSFilepath = filepath; }
	inline void SetPSFilepath(const std::wstring& filepath) { m_PSFilepath = filepath; }

	void SetCBV(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility);
	void SetCBV(UINT shaderRegister, UINT numDescriptors, D3D12_SHADER_VISIBILITY visibility);
	void SetSRV(UINT shaderRegister, UINT numDescriptors, D3D12_SHADER_VISIBILITY visibility);
	void SetStaticSampler(UINT shaderRegister, D3D12_FILTER filter);
	void SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout);
	void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);

	void Create();

	inline RootSignature* GetRootSignature() const { return m_pRootSignature; }
	inline PipelineState* GetPipelineState() const { return m_pPipelineState; }

private:
	std::wstring m_VSFilepath{};	// 頂点シェーダーファイルパス
	std::wstring m_PSFilepath{};	// ピクセルシェーダーファイルパス
	RootSignature* m_pRootSignature{};	// ルートシグネチャ
	PipelineState* m_pPipelineState{};	// パイプラインステート
};
