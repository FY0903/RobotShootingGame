/*+===================================================================
	File: RootSignature.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/03 12:56:59 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "RootSignature.hpp"
#include "../../../DirectXTex/d3dx12.h"
#include "../../System/Engine/Engine.hpp"
#include <iterator>

RootSignature::RootSignature()
{
	auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;	// 入力アセンブラステージでの入力レイアウトを許可
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;				// ハルシェーダーステージでのルートアクセスを拒否
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;			// ドメインシェーダーステージでのルートアクセスを拒否
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;			// ジオメトリシェーダーステージでのルートアクセスを拒否

	CD3DX12_ROOT_PARAMETER rootParams[2]{};
	rootParams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); // 定数バッファビューをルートパラメータとして初期化

	CD3DX12_DESCRIPTOR_RANGE tableRange[1]{};	// ディスクリプタテーブル
	tableRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // シェーダーリソースビューを1つ、シェーダーレジスタ0から使用する
	rootParams[1].InitAsDescriptorTable(static_cast<UINT>(std::size(tableRange)), tableRange, D3D12_SHADER_VISIBILITY_ALL); // ディスクリプタテーブルをルートパラメータとして初期化

	// スタティックサンプラーの設定
	CD3DX12_STATIC_SAMPLER_DESC sampler = CD3DX12_STATIC_SAMPLER_DESC(
		0,									// シェーダーレジスタ
		D3D12_FILTER_MIN_MAG_MIP_LINEAR);	// フィルター

	// ルートシグニチャの設定
	D3D12_ROOT_SIGNATURE_DESC desc{};
	desc.NumParameters = static_cast<UINT>(std::size(rootParams));	// ルートパラメータの数
	desc.NumStaticSamplers = 1;					// スタティックサンプラーの数
	desc.pParameters = rootParams;				// ルートパラメータ
	desc.pStaticSamplers = &sampler;			// スタティックサンプラー
	desc.Flags = flag;							// フラグ

	ComPtr<ID3DBlob> pBlob{};					// バイナリ
	ComPtr<ID3DBlob> pErrorBlob{};				// エラーメッセージ

	// シリアライズ
	HRESULT hr = D3D12SerializeRootSignature(
		&desc,							// ルートシグニチャの設定
		D3D_ROOT_SIGNATURE_VERSION_1_0,	// バージョン1.0
		pBlob.GetAddressOf(),			// バイナリ
		pErrorBlob.GetAddressOf());		// エラーメッセージ
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA(static_cast<const char*>(pErrorBlob->GetBufferPointer()));
		}
		MessageBox(nullptr, "ルートシグネチャのシリアライズに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return; // エラー終了
	}

	// ルートシグネチャの生成
	hr = Engine::GetInstance().GetDevice()->CreateRootSignature(
		0,												// ノードマスク
		pBlob->GetBufferPointer(),						// シリアライズされたルートシグネチャ
		pBlob->GetBufferSize(),							// バッファサイズ
		IID_PPV_ARGS(m_pRootSignature.GetAddressOf())); // ルートシグネチャのポインタ
	if (FAILED(hr))
	{
		MessageBox(nullptr, "ルートシグネチャの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return; // エラー終了
	}
}
