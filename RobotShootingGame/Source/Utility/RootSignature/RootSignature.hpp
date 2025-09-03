/*+===================================================================
	File: RootSignature.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/03 12:56:59 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "../ComPtr.h"

// ==============================
//	前方宣言
// ==============================
struct ID3D12RootSignature;

/**
 * @brief RootSignatureクラス
 */
class RootSignature
{
public:
	/**
	 * コンストラクタ
	 */
	RootSignature();

	/**
	 * デストラクタ
	 */
	~RootSignature() = default;

	ID3D12RootSignature* Get() const { return m_pRootSignature.Get(); }

private:
	ComPtr<ID3D12RootSignature> m_pRootSignature{};	// ルートシグネチャ
};
