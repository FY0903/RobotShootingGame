/*+===================================================================
	File: RootSignature.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/08/06 17:40:08 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "../ComPtr.h"
#include <d3d12.h>
#include <vector>

// ==============================
//	enum
// ==============================
/**
 * @brief シェーダーステージを表す列挙型です。
 */
enum ShaderStage
{
	ALL,
	VS,
	HS,
	DS,
	GS,
	PS,
};

/**
 * @brief テクスチャサンプリングの方法とアドレッシングモードを指定する列挙型です。
 */
enum SamplerState
{
	PointWrap,			// ポイントサンプリング、ラップ
	PointClamp,			// ポイントサンプリング、クランプ
	LinearWrap,			// リニアサンプリング、ラップ
	LinearClamp,		// リニアサンプリング、クランプ
	AnisotropicWrap,	// 異方性サンプリング、ラップ
	AnisotropicClamp,	// 異方性サンプリング、クランプ
};

/**
 * @brief RootSignatureクラス
 */
class RootSignature
{
public:
	/**
	 * @brief Direct3D 12のルートシグネチャを構築・管理するためのクラスです。
	 */
	class Desc
	{
	public:
		/**
		 * @brief コンストラクタ
		 */
		Desc();

		/**
		 * @brief デストラクタ
		 */
		~Desc();

		/**
		 * @brief 指定された数の要素の先頭への参照を返します。
		 * @param [count] 取得または処理を開始する要素数。
		 * @return Desc 型の参照。指定された数の要素の先頭を指します。
		 */
		Desc& Begin(int count);

		/**
		 * @brief 指定したシェーダーステージの定数バッファビュー (CBV) を設定します。
		 * @param [stage] CBV を設定する対象のシェーダーステージ。
		 * @param [index] 設定する CBV のインデックス。
		 * @param [reg] バインドするレジスタ番号。
		 * @return このオブジェクトへの参照 (Desc&)。
		 */
		Desc& SetCBV(ShaderStage stage, int index, uint32_t reg);

		/**
		 * @brief 指定されたシェーダーステージの指定レジスタにシェーダーリソースビュー（SRV）を設定します。
		 * @param [stage] SRV を設定する対象のシェーダーステージ。
		 * @param [index] SRV を設定するスロットのインデックス。
		 * @param [reg SRV をバインドするレジスタ番号。
		 * @return 設定後の Desc オブジェクトへの参照。
		 */
		Desc& SetSRV(ShaderStage stage, int index, uint32_t reg);

		/**
		 * @brief 指定したシェーダーステージの指定されたインデックスにアンオーダードアクセスビュー (UAV) を設定します。
		 * @param [stage] UAV を設定する対象のシェーダーステージ。
		 * @param [index] UAV をバインドするスロットのインデックス。
		 * @param [reg] UAV をバインドするレジスタ番号。
		 * @return 設定された UAV の記述子 (Desc) への参照。
		 */
		Desc& SetUAV(ShaderStage stage, int index, uint32_t reg);

		/**
		 * @brief シェーダーステージにサンプラーステートを設定します。
		 * @param [stage] サンプラーを設定するシェーダーステージ。
		 * @param [index] サンプラーのインデックス。
		 * @param [reg] サンプラーをバインドするレジスタ番号。
		 * @param [state] 設定するサンプラーステート。
		 * @return 設定後のDescオブジェクトへの参照。
		 */
		Desc& SetSmp(ShaderStage stage, int index, uint32_t reg, SamplerState state);

		/**
		 * @brief 指定したシェーダーステージに静的サンプラーを追加します。
		 * @param [stage] サンプラーを追加する対象のシェーダーステージ。
		 * @param [reg] サンプラーをバインドするレジスタ番号。
		 * @param [state] 追加するサンプラーの状態を表すSamplerState。
		 * @return 追加された静的サンプラーの記述子（Desc型）への参照。
		 */
		Desc& AddStaticSmp(ShaderStage stage, uint32_t reg, SamplerState state);

		/**
		 * @brief IL（中間言語）を許可する設定を返します。
		 * @return ILを許可する設定を表すDesc型の参照。
		 */
		Desc& AllowIL();

		/**
		 * @brief SO（スタックオーバーフロー）を許可する設定を返します。
		 * @return SO を許可する設定を表す Desc 型の参照。
		 */
		Desc& AllowSo();

		/**
		 * @brief イテレーションの終端を示すイテレータを返します。
		 * @return イテレーションの終端を示すDesc型の参照。
		 */
		Desc& End();

		/**
		 * @brief ルートシグネチャの記述子を取得します。
		 * @return ルートシグネチャの記述子（D3D12_ROOT_SIGNATURE_DESC 型）への定数参照。
		 */
		inline const D3D12_ROOT_SIGNATURE_DESC& GetDesc() const { return m_desc; }

	private:
		std::vector<D3D12_DESCRIPTOR_RANGE> m_ranges{};			// ディスクリプタレンジの配列
		std::vector<D3D12_STATIC_SAMPLER_DESC> m_samplers{};	// スタティックサンプラーの配列
		std::vector<D3D12_ROOT_PARAMETER> m_params{};			// ルートパラメーターの配列
		D3D12_ROOT_SIGNATURE_DESC m_desc{};						// ルートシグネチャの設定
		bool m_DenyStage[5]{};									// 各シェーダーステージのルートアクセスを拒否するフラグ
		uint32_t m_Flags{};										// ルートシグネチャのフラグ

		/**
		 * @brief 指定されたShaderStageが有効かどうかを検証します。
		 * @param [stage] 検証するシェーダーステージ。
		 */
		void CheckStage(ShaderStage stage);

		/**
		 * @brief シェーダーステージのパラメータを設定します。
		 * @param [stage] パラメータを設定する対象のシェーダーステージ。
		 * @param [index] 設定するパラメータのインデックス。
		 * @param [reg] 関連付けるレジスタ番号。
		 * @param [type] ディスクリプタレンジの種類（D3D12_DESCRIPTOR_RANGE_TYPE 列挙体）。
		 */
		void SetParam(ShaderStage stage, int index, uint32_t reg, D3D12_DESCRIPTOR_RANGE_TYPE type);
	};

	/**
	 * コンストラクタ
	 */
	RootSignature() = default;

	/**
	 * デストラクタ
	 */
	~RootSignature();
	
	/**
	 * @brief ID3D12Device オブジェクトとルートシグネチャ記述子を使用して初期化を行います。
	 * @param [pDevice] 初期化に使用する ID3D12Device へのポインタ。
	 * @param [desc] ルートシグネチャの設定を指定する D3D12_ROOT_SIGNATURE_DESC 構造体への参照。
	 * @return 初期化に成功した場合は true、失敗した場合は false を返します。
	 */
	bool Init(ID3D12Device* pDevice, const D3D12_ROOT_SIGNATURE_DESC& desc);

	/**
	 * @brief 終了処理を実行します。
	 */
	void Term();

	/**
	 * @brief m_rootSignature から ID3D12RootSignature ポインタを取得します。
	 * @return m_rootSignature が保持する ID3D12RootSignature へのポインタ。
	 */
	inline ID3D12RootSignature* GetPtr() const { return m_rootSignature.Get(); }

private:
	ComPtr<ID3D12RootSignature> m_rootSignature;	// ルートシグネチャ
};
