/*+===================================================================
	File: ComPtr.h
	Summary: Microsoft::WRL::ComPtrのヘッダファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/28 13:20 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <wrl/client.h>

// ==============================
//	Type Definitions
// ==============================
template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;