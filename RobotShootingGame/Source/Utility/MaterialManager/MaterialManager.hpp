/*+===================================================================
	File: MaterialManager.hpp
	Summary: MaterialManagerクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/30 05:41 初回作成
			26/01/15 18:58 コメント記載
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

	/**
	 * @brief テクスチャを設定します。
	 * @param pTexture 設定するテクスチャへのポインタ。
	 */
	void SetTexture(class Texture* pTexture);
	
	/**
	 * @brief レンダーターゲットをテクスチャとして設定します。
	 * @param pRTV 設定するレンダーターゲットへのポインタ。
	 */
	void SetTexture(class RenderTarget* pRTV);
	
	/**
	 * @brief 指定したレジスタに定数バッファを設定します。
	 * @param index レジスタのインデックス。
	 * @param size 定数バッファのサイズ。
	 * @return フレームバッファ数分の定数バッファポインタの配列への参照。
	 */
	std::array<ConstantBuffer*, FRAME_BUFFER_COUNT>& SetCBAtRegister(int index, size_t size);
	
	/**
	 * @brief 指定したレジスタの定数バッファを取得します。
	 * @param index レジスタのインデックス。
	 * @return フレームバッファ数分の定数バッファポインタの配列へのポインタ。存在しない場合はnullptrを返します。
	 */
	const std::array<ConstantBuffer*, FRAME_BUFFER_COUNT>* GetCBByRegister(int index) const;
	
	/**
	 * @brief 指定したレジスタとフレームインデックスの定数バッファを取得します。
	 * @param index レジスタのインデックス。
	 * @param frameIndex フレームインデックス。
	 * @return 指定したレジスタとフレームインデックスに対応する定数バッファへのポインタ。存在しない場合はnullptrを返します。
	 */
	ConstantBuffer* GetCBByRegisterForFrame(int index, size_t frameIndex) const;
	
	/**
	 * @brief 指定したレジスタの定数バッファサイズを取得します。
	 * @param index レジスタのインデックス。
	 * @return 指定したレジスタの定数バッファサイズ。存在しない場合は-1を返します。
	 */
	size_t GetCBSizeForRegister(int index) const;
	
	/**
	 * @brief 定数バッファのサイズを取得します。
	 * @return 定数バッファのサイズ。
	 */
	int GetCBSize() const;
	
	/**
	 * @brief ディスクリプタヒープを取得します。
	 * @return ディスクリプタヒープへのポインタ。存在しない場合はnullptrを返します。
	 */
	DescriptorHeap* GetDescriptorHeap() const;
	
	/**
	 * @brief 指定したインデックスのディスクリプタハンドルを取得します。
	 * @param index 取得するディスクリプタハンドルのインデックス。
	 * @return 指定したインデックスのディスクリプタハンドルへのポインタ。存在しない場合はnullptrを返します。
	 */
	DescriptorHandle* GetDescriptorHandle(size_t index) const;
	
	/**
	 * @brief ルートシグネチャを取得します。
	 * @return ルートシグネチャへのポインタ。存在しない場合はnullptrを返します。
	 */
	class RootSignature* GetRootSignature() const;
	
	/**
	 * @brief パイプラインステートを取得します。
	 * @return パイプラインステートへのポインタ。存在しない場合はnullptrを返します。
	 */
	class PipelineState* GetPipelineState() const;
	
	/**
	 * @brief 不透明かどうかを設定します。
	 * @param isOpaque 不透明かどうかのフラグ。
	 */
	inline void SetIsOpaque(bool isOpaque) { m_IsOpaque = isOpaque; }
	
	/**
	 * @brief 不透明かどうかを取得します。
	 * @return 不透明であればtrue、そうでなければfalse。
	 */
	inline bool IsOpaque() const { return m_IsOpaque; }
	
	/**
	 * @brief ルートパラメータのインデックスを取得します。
	 * @return ルートパラメータのインデックス。
	 */
	int GetRootParameterIndex() const;
	
	/**
	 * @brief 入力レイアウトタイプを取得します。
	 * @return 入力レイアウトタイプ。
	 */
	MaterialBase::InputLayoutType GetInputLayoutType() const;

private:
	MaterialBase* m_pMaterial{};														// マテリアル
	DescriptorHeap* m_pDescriptorHeap{};												// ディスクリプタヒープ
	std::vector<DescriptorHandle*> m_pDescriptorHandle{};								// ディスクリプタハンドル
	std::unordered_map<int, std::array<ConstantBuffer*, FRAME_BUFFER_COUNT>> m_pCBVs{};	// CBV用定数バッファ
	std::unordered_map<int, size_t> m_CBSize{};											// 定数バッファサイズ
	
	bool m_IsOpaque{};	// 不透明かどうか
};

/**
 * @brief MaterialManagerクラス
 */
class MaterialManager : public Singleton<MaterialManager>
{
public:
	/**
	 * 初期化処理
	 */
	void Init();

	/**
	 * @brief 指定された名前で Material を作成し、そのポインタを返します。
	 * @param materialName 作成するマテリアルの名前を表す文字列（const std::string&）。
	 * @return 作成された Material へのポインタ。エラー時の挙動は実装依存で、nullptr を返す場合があります。
	 */
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

	/**
	 * @brief 指定した名前と設定で MaterialBase オブジェクトを作成して返します。
	 * @param name 作成するマテリアルの名前。
	 * @param renderTargetNum レンダーターゲットの数。
	 * @param alphaBlend アルファブレンドを有効にするかどうか。
	 * @return 作成された MaterialBase へのポインタ。作成に失敗した場合は nullptr を返す可能性があります。
	 */
	MaterialBase* CreateMaterialBase(const std::string& name, size_t renderTargetNum = 1, bool alphaBlend = false);
	
	/**
	 * @brief 指定した名前に対応する MaterialBase オブジェクトを取得します。
	 * @param name 検索するマテリアルの名前を表す文字列（const std::string&）。
	 * @return 見つかったマテリアルへのポインタ（MaterialBase*）。存在しない場合は nullptr を返すことがあります。
	 */
	MaterialBase* GetMaterial(const std::string& name);

	std::unordered_map<std::string, MaterialBase*> m_MaterialData{};					// マテリアルデータ
	std::unordered_map<std::string, std::vector<Material*>> m_MaterialInstanceData{};	// マテリアルインスタンスデータ
};
