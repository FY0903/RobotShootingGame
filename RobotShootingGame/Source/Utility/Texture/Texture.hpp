/*+===================================================================
	File: Texture.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/05 9:36:05 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "../DirectXTex/d3dx12.h"

/**
 * @brief Textureクラス
 */
class Texture
{
public:
	/**
	 * コンストラクタ
	 */
	Texture() = default;

	/**
	 * デストラクタ
	 */
	~Texture() = default;

	HRESULT Load(const std::string& FileName);

	UINT Width() const { return m_Width; }
	UINT Height() const { return m_Height; }

	ID3D12Resource* Resource() const { return m_pResource.Get(); }
	D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() const { return m_ViewDesc; }

private:
	UINT m_Width{};		// テクスチャの幅
	UINT m_Height{};	// テクスチャの高さ
	ComPtr<ID3D12Resource> m_pResource{};			// テクスチャリソース
	D3D12_SHADER_RESOURCE_VIEW_DESC m_ViewDesc{};	// シェーダーリソースビューの設定
	bool m_IsOpaque{};
};
