/*+===================================================================
	File: Empty.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/30 0:39:09 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Empty.hpp"
#include "Game/Actor/Object/Object.hpp"

void Empty::OnInit()
{
	// 子オブジェクトとしてObjectを追加
	AddChildActor<Object>();
}

void Empty::OnUpdate()
{
}

void Empty::OnUninit()
{
}
