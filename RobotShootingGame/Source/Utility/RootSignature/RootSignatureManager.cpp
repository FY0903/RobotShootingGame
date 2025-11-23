/*+===================================================================
	File: RootSignatureManager.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/22 21:38:54 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "RootSignatureManager.hpp"

void RootSignatureManager::ResisterRootSignature(const std::string& name, RootSignature* pRootSignature)
{
	m_RootSignatures[name] = pRootSignature;
}

RootSignature* RootSignatureManager::GetRootSignature(const std::string& name)
{
	auto it = m_RootSignatures.find(name);
	if (it != m_RootSignatures.end())
	{
		return it->second;
	}
	return nullptr;
}

RootSignatureManager::~RootSignatureManager()
{
	for (auto& pair : m_RootSignatures)
	{
		if (pair.second)
		{
			delete pair.second;
			pair.second = nullptr;
		}
	}
	m_RootSignatures.clear();
}
