/*+===================================================================
	File: FileUtil.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/07/22 16:51 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "FileUtil.hpp"

// ==============================
//	constexpr
// ==============================
constexpr int MAX_PATH_LENGTH = 520; // 最大パス長

bool SearchFilePath(const wchar_t* filename, std::wstring& result)
{
	if (!filename) return false;

	if (!wcscmp(filename, L" ") || !wcscmp(filename, L"")) return false;

	wchar_t exePath[MAX_PATH_LENGTH] = {};
	GetModuleFileNameW(nullptr, exePath, MAX_PATH_LENGTH);	// 実行ファイルのパスを取得
	exePath[MAX_PATH_LENGTH - 1] = L'\0'; // null終端化
	PathRemoveFileSpecW(exePath); // ファイル名を除去してディレクトリパスを取得

	wchar_t dstPath[MAX_PATH_LENGTH] = {};
	wcscpy_s(dstPath, filename);	// 検索するファイル名をコピー

	// ファイルの存在を確認
	if (PathFileExistsW(dstPath))
	{
		result = dstPath;
		return true;
	}

	// 相対パスでの検索
	swprintf_s(dstPath, L"..\\%s", filename);
	if (PathFileExistsW(dstPath))
	{
		result = dstPath;
		return true;
	}

	// 相対パスでの検索
	swprintf_s(dstPath, L"..\\..\\%s", filename);
	if (PathFileExistsW(dstPath))
	{
		result = dstPath;
		return true;
	}

	// リソースディレクトリでの検索
	swprintf_s(dstPath, L"\\res\\%s", filename);
	if (PathFileExistsW(dstPath))
	{
		result = dstPath;
		return true;
	}

	// 実行ファイルのディレクトリでの検索
	swprintf_s(dstPath, L"%s\\%s", exePath, filename);
	if (PathFileExistsW(dstPath))
	{
		result = dstPath;
		return true;
	}

	// 実行ファイルの親ディレクトリでの検索
	swprintf_s(dstPath, L"%s\\..\\%s", exePath, filename);
	if (PathFileExistsW(dstPath))
	{
		result = dstPath;
		return true;
	}

	// 実行ファイルの親の親ディレクトリでの検索
	swprintf_s(dstPath, L"%s\\..\\..\\%s", exePath, filename);
	if (PathFileExistsW(dstPath))
	{
		result = dstPath;
		return true;
	}

	// 実行ファイルのリソースディレクトリでの検索
	swprintf_s(dstPath, L"%s\\res\\%s", exePath, filename);
	if (PathFileExistsW(dstPath))
	{
		result = dstPath;
		return true;
	}

	return false; // ファイルが見つからなかった場合は false を返す
}
