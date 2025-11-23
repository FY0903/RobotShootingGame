/*+===================================================================
	File: RootSignatureManager.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/22 21:38:54 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/Singleton/Singleton.hpp"
#include <unordered_map>
#include <functional>

#include "RootSignature.hpp"

/**
 * @brief RootSignatureManagerクラス
 */
class RootSignatureManager : public Singleton<RootSignatureManager>
{
public:
	void ResisterRootSignature(const std::string& name, RootSignature* pRootSignature);

	RootSignature* GetRootSignature(const std::string& name);

private:
	friend class Singleton<RootSignatureManager>;

	/**
	 * コンストラクタ
	 */
	RootSignatureManager() = default;

	/**
	 * デストラクタ
	 */
	~RootSignatureManager();

	std::unordered_map<std::string, RootSignature*> m_RootSignatures{};
};
