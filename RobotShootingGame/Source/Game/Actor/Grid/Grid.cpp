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
	auto line = AddComponent<Line>();
	line->AddPoint({ -10.0f, 0.0f, 0.0f }, { 10.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }); // X軸
	line->AddPoint({ 0.0f, -10.0f, 0.0f }, { 0.0f, 10.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }); // Y軸
	line->AddPoint({ 0.0f, 0.0f, -10.0f }, { 0.0f, 0.0f, 10.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }); // Z軸
	line->Create();
}

void Grid::OnUninit()
{
}
