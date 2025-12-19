/*+===================================================================
	File: Grid.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/22 16:46:33 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Grid.hpp"
#include "Utility/Compornent/Line/Line.hpp"

void Grid::OnInit()
{
	// マテリアルの設定
	m_pMaterial = MaterialManager::GetInstance().CreateMaterial("Line");

	// 線分コンポーネントの追加
	auto line = AddComponent<Line>();
	line->AddPoint({ -100.0f, 0.0f, 0.0f }, { 100.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }); // X軸
	line->AddPoint({ 0.0f, -100.0f, 0.0f }, { 0.0f, 100.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }); // Y軸
	line->AddPoint({ 0.0f, 0.0f, -100.0f }, { 0.0f, 0.0f, 100.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }); // Z軸

	for (int i = -10; i <= 10; ++i)
	{
		if (!i) continue; // 中心線はスキップ

		// グリッド線の追加（XZ平面）
		line->AddPoint({ static_cast<float>(i), 0.0f, -10.0f }, { static_cast<float>(i), 0.0f, 10.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
		line->AddPoint({ -10.0f, 0.0f, static_cast<float>(i) }, { 10.0f, 0.0f, static_cast<float>(i) }, { 1.0f, 1.0f, 1.0f, 1.0f });
	}

	line->Create();

	// 不透明設定を無効化
	m_pMaterial->SetIsOpaque(false);
}

void Grid::OnUpdate()
{
}

void Grid::OnUninit()
{
}
