/*+===================================================================
	File: Animation.cpp
	Summary: アニメーションクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/25 14:38 初回作成
			26/01/15 16:39 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Animation.hpp"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>

HRESULT Animation::Load(const std::string& fileName)
{
	// ファイルからアニメーションデータを読み込む
	auto scene = m_Importer.ReadFile(fileName, aiProcess_MakeLeftHanded);

	// 読み込みに失敗した場合はエラーメッセージを出力して終了
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
	// アニメーションシーンが存在しない場合はエラーを出力して終了
	if (!m_Animations)
	{
		assert(0 && "アニメーションが読み込まれていません。");
		return nullptr;
	}

	// 指定されたインデックスが範囲外の場合はエラーを出力して終了
	if (index < 0 || index >= static_cast<int>(m_Animations->mNumAnimations))
	{
		assert(0 && "アニメーションのインデックスが範囲外です。");
		return nullptr;
	}

	return m_Animations->mAnimations[index];
}
