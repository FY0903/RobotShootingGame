/*+===================================================================
	File: Object.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/24 13:58:08 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Object.hpp"
#include "Utility/Compornent/MeshRenderer/MeshRenderer.hpp"

void Object::OnInit()
{
	m_pModel = new Model();
	//if (FAILED(m_pModel->Load("Assets/Model/test/FBX_Demo_Idle.fbx", false, false)))
	if (FAILED(m_pModel->Load("Assets/Model/character/Hew_kyaracter(1.0).fbx", false, false)))
	{
		assert(0 && "Object.cpp モデルの読み込みに失敗しました。");
	}
	AddComponent<MeshRenderer>()->Init(m_pModel);
}

void Object::OnUninit()
{
	if (m_pModel)
	{
		delete m_pModel;
		m_pModel = nullptr;
	}
}
