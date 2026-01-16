/*+===================================================================
	File: RootSignature.cpp
	Summary: RootSignatureクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/03 12:56 初回作成
			26/01/16 11:11 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "RootSignature.hpp"
#include "System/Engine/Engine.hpp"

void RootSignature::AddRootParameter(UINT shaderRegiser, D3D12_SHADER_VISIBILITY visibility)
{
	CD3DX12_ROOT_PARAMETER param{};
	param.InitAsConstantBufferView(shaderRegiser, 0, visibility);
	m_RootParams.push_back(param);
}

void RootSignature::AddDescriptorRange(UINT shaderRegiser, D3D12_DESCRIPTOR_RANGE_TYPE rangeType, UINT numDescriptors, D3D12_SHADER_VISIBILITY visibility)
{
	// テクスチャを使用するためのディスクリプタレンジを作成必要がある
	CD3DX12_DESCRIPTOR_RANGE range{};
	range.Init(rangeType, numDescriptors, shaderRegiser);
	m_DescriptorRanges.push_back(range);

	CD3DX12_ROOT_PARAMETER param{};
	param.InitAsDescriptorTable(1, &m_DescriptorRanges.back(), visibility);
	m_RootParams.push_back(param);
}

void RootSignature::AddStaticSampler(UINT shaderRegiser, D3D12_FILTER filter)
{
	CD3DX12_STATIC_SAMPLER_DESC sampler{};
	sampler.Init(shaderRegiser, filter);
	m_StaticSamplers.push_back(sampler);
}

void RootSignature::Create()
{
	auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;	// 入力アセンブラステージでの入力レイアウトを許可
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;				// ハルシェーダーステージでのルートアクセスを拒否
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;			// ドメインシェーダーステージでのルートアクセスを拒否
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;			// ジオメトリシェーダーステージでのルートアクセスを拒否

	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC desc{};
	desc.NumParameters = static_cast<UINT>(m_RootParams.size());	// ルートパラメータの数
	desc.NumStaticSamplers = static_cast<UINT>(m_StaticSamplers.size()); // スタティックサンプラーの数
	desc.pParameters = m_RootParams.data();				// ルートパラメータ
	desc.pStaticSamplers = m_StaticSamplers.data();	// スタティックサンプラー
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
