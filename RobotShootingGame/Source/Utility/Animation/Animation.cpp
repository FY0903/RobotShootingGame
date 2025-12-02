/*+===================================================================
	File: Animation.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/25 14:38:44 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Animation.hpp"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>

HRESULT Animation::Load(const std::string& fileName)
{
	auto scene = m_Importer.ReadFile(fileName, aiProcess_MakeLeftHanded);

	if (!scene)
	{
		auto error = m_Importer.GetErrorString();
		OutputDebugStringA(error);
		assert(0 && "アニメーションの読み込みに失敗");
		return E_FAIL;
	}

	m_Animations = scene;

	return S_OK;
}

aiAnimation* Animation::GetAnimation(int index)
{
	if (!m_Animations)
	{
		assert(0 && "アニメーションが読み込まれていません。");
		return nullptr;
	}

	if (index < 0 || index >= static_cast<int>(m_Animations->mNumAnimations))
	{
		assert(0 && "アニメーションのインデックスが範囲外です。");
		return nullptr;
	}

	return m_Animations->mAnimations[index];
}
