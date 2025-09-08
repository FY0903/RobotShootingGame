/*+===================================================================
	File: Texture2D.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/03 16:42:36 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "../ComPtr.h"
#include "../../../DirectXTex/d3dx12.h"
#include <string>

// ==============================
//	前方宣言
// ==============================
class DescriptorHeap;
class DescriptorHandle;

/**
 * @brief Texture2Dクラス
 */
class Texture2D
{
public:

	/**
	 * コンストラクタ
	 */
	Texture2D() = default;

	/**
	 * デストラクタ
	 */
	~Texture2D() = default;

	static Texture2D* Get(std::string path);
	static Texture2D* Get(std::wstring path);
	static Texture2D* GetWhite();

	ID3D12Resource* Resource() const { return m_pResource.Get(); }
	D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() const;

private:

	Texture2D(std::string path);
	Texture2D(std::wstring path);
	Texture2D(ID3D12Resource* buffer);
	ComPtr<ID3D12Resource> m_pResource{};	// テクスチャリソース
	bool Load(std::string& path);
	bool Load(std::wstring& path);

	static ID3D12Resource* GetDefaultResource(size_t width, size_t height);

	Texture2D(const Texture2D&) = delete;
	void operator=(const Texture2D&) = delete;
};
