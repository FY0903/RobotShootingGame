/*+===================================================================
	File: MaterialManager.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/30 5:41:16 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/MaterialBase/MaterialBase.hpp"
#include "Utility/DescriptorHeap/DescriptorHeap.hpp"

class Material
{
public:
	/**
	 * コンストラクタ
	 */
	Material(MaterialBase* pMaterial);
	
	/**
	 * デストラクタ
	 */
	~Material();

	void SetTexture(class Texture* pTexture);
	void SetTexture(class RenderTarget* pRTV);
	std::vector<ConstantBuffer*> SetCB(size_t index, class ConstantBuffer* pCB);

	DescriptorHeap* GetDescriptorHeap() const;
	DescriptorHandle* GetDescriptorHandle(size_t index) const;
	class RootSignature* GetRootSignature() const;
	class PipelineState* GetPipelineState() const;
	inline bool IsOpaque() const { return m_IsOpaque; }
	int GetRootParameterIndex() const;
	class ConstantBuffer* GetCB(size_t index) const;
	int GetCBSize() const;

private:
	MaterialBase* m_pMaterial{}; // マテリアル
	DescriptorHeap* m_pDescriptorHeap{}; // ディスクリプタヒープ
	std::vector<DescriptorHandle*> m_pDescriptorHandle{}; // ディスクリプタハンドル
	bool m_IsOpaque{};
};

/**
 * @brief MaterialManagerクラス
 */
class MaterialManager : public Singleton<MaterialManager>
{
public:
	void Init();

	Material* CreateMaterial(const std::string& materialName);

private:
	friend class Singleton<MaterialManager>;

	/**
	 * コンストラクタ
	 */
	MaterialManager() = default;

	/**
	 * デストラクタ
	 */
	~MaterialManager();

	MaterialBase* CreateMaterialBase(const std::string& name, size_t renderTargetNum = 1, bool alphaBlend = false);
	MaterialBase* GetMaterial(const std::string& name);

	std::unordered_map<std::string, MaterialBase*> m_MaterialData{};
	std::unordered_map<std::string, std::vector<Material*>> m_MaterialInstanceData{};
};
