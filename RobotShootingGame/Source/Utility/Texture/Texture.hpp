/*+===================================================================
	File: Texture.hpp
	Summary: Textureクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/05 09:36 初回作成
            26/01/16 11:43 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "../DirectXTex/d3dx12.h"
#include "../DirectXTex/DirectXTex.h"

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

	/**
	 * @brief 指定されたファイル名のファイルを読み込みます。読み込みの成否はHRESULTで返されます。
	 * @param FileName 読み込むファイルのパスまたは名前を指定します。
	 * @return 操作の成否を示すHRESULT。成功時はS_OK、失敗時は適切なエラーコードを返します。
	 */
	HRESULT Load(const std::string& FileName);

	/**
	 * @brief デフォルトのテクスチャを作成します。RGBA 各チャネルは指定可能で、省略時は不透明な白 (255,255,255,255) になります。
	 * @param R 赤チャネルの値。0~255 の範囲。
	 * @param G 緑チャネルの値。0~255 の範囲。
	 * @param B 青チャネルの値。0~255 の範囲。
	 * @param A アルファ（不透明度）チャネルの値。0~255 の範囲。
	 * @return HRESULT：操作結果を表します。成功時は S_OK、失敗時は適切なエラーコードを返します。
	 */
	HRESULT CreateDefaultTexture(BYTE R = 255, BYTE G = 255, BYTE B = 255, BYTE A = 255);

	/**
	 * @brief テクスチャの幅を取得します。
	 * @return テクスチャの幅（ピクセル単位）。
	 */
	UINT Width() const { return m_Width; }

	/**
	 * @brief テクスチャの高さを取得します。
	 * @return テクスチャの高さ（ピクセル単位）。
	 */
	UINT Height() const { return m_Height; }

	/**
	 * @brief テクスチャリソースへのポインタを取得します。
	 * @return テクスチャリソースへのポインタ。
	 */
	ID3D12Resource* Resource() const { return m_pResource.Get(); }

	/**
	 * @brief シェーダーリソースビューの設定を取得します。
	 * @return シェーダーリソースビューの設定。
	 */
	D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() const { return m_ViewDesc; }

	/**
	 * @brief テクスチャが不透明かどうかを取得します。
	 * @return 不透明であれば true、そうでなければ false。
	 */
	bool IsOpaque() const { return m_IsOpaque; }

private:
	/**
	 * @brief DirectX::ScratchImage と DirectX::TexMetadata からテクスチャを生成します。
	 * @param scratch テクスチャのピクセルデータやイメージを保持する DirectX::ScratchImage の参照（ソースデータ）。
	 * @param meta 幅・高さ・フォーマット・ミップレベルなどのテクスチャ情報を含む DirectX::TexMetadata の参照。
	 * @return 操作結果を示す HRESULT。成功時は S_OK を返し、失敗時は適切なエラーコードを返す。
	 */
	HRESULT CreateTextureFromScratch(const DirectX::ScratchImage& scratch, const DirectX::TexMetadata& meta);

	UINT m_Width{};		// テクスチャの幅
	UINT m_Height{};	// テクスチャの高さ

	ComPtr<ID3D12Resource> m_pResource{};			// テクスチャリソース
	D3D12_SHADER_RESOURCE_VIEW_DESC m_ViewDesc{};	// シェーダーリソースビューの設定
	
	bool m_IsOpaque{};	// テクスチャが不透明かどうか
};
