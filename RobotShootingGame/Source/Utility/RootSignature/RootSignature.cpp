/*+===================================================================
	File: RootSignature.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/08/06 17:40:08 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "RootSignature.hpp"

RootSignature::Desc::Desc()
{
	for (auto& deny : m_DenyStage)
	{
		deny = true; // 初期状態では全てのステージアクセスを拒否
	}
}

RootSignature::Desc::~Desc()
{
	m_ranges.clear();
	m_samplers.clear();
	m_params.clear();
}

RootSignature::Desc& RootSignature::Desc::Begin(int count)
{
	m_Flags = 0;
	for (auto& deny : m_DenyStage)
	{
		deny = true; // 初期状態では全てのステージアクセスを拒否
	}

	m_desc = {};

	m_samplers.clear();
	m_ranges.resize(count);
	m_params.resize(count);

	return *this;
}

RootSignature::Desc& RootSignature::Desc::SetCBV(ShaderStage stage, int index, uint32_t reg)
{
	SetParam(stage, index, reg, D3D12_DESCRIPTOR_RANGE_TYPE_CBV); // CBVのパラメータを設定

	return *this; // 自身を返す
}

RootSignature::Desc& RootSignature::Desc::SetSRV(ShaderStage stage, int index, uint32_t reg)
{
	SetParam(stage, index, reg, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // SRVのパラメータを設定
	
	return *this; // 自身を返す
}

RootSignature::Desc& RootSignature::Desc::SetUAV(ShaderStage stage, int index, uint32_t reg)
{
	SetParam(stage, index, reg, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); // UAVのパラメータを設定
	
	return *this; // 自身を返す
}

RootSignature::Desc& RootSignature::Desc::SetSmp(ShaderStage stage, int index, uint32_t reg, SamplerState state)
{
	SetParam(stage, index, reg, D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER); // サンプラーのパラメータを設定

	return *this; // 自身を返す
}

RootSignature::Desc& RootSignature::Desc::AddStaticSmp(ShaderStage stage, uint32_t reg, SamplerState state)
{
	D3D12_STATIC_SAMPLER_DESC desc{};

	desc.MipLODBias = D3D12_DEFAULT_MIP_LOD_BIAS;	// MIPレベルのバイアスをデフォルトに設定
	desc.MaxAnisotropy = 1;	// 最大異方性フィルタリングのレベルを1に設定
	desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;	// 比較関数を常に偽に設定
	desc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	// ボーダーカラーを透明な黒に設定
	desc.MinLOD = 0.0f;	// 最小LODを0に設定
	desc.MaxLOD = D3D12_FLOAT32_MAX;	// 最大LODを最大値に設定
	desc.ShaderRegister = reg;	// シェーダーレジスタを設定
	desc.RegisterSpace = 0;	// レジスタスペースを0に設定
	desc.ShaderVisibility = D3D12_SHADER_VISIBILITY(stage);	// シェーダーの可視性を設定
	CheckStage(stage);	// 指定されたステージのアクセスを許可

	switch (state)
	{
	case PointWrap:
		desc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;	// ポイントサンプリング、ラップ
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// U座標のアドレスモードをラップに設定
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// V座標のアドレスモードをラップに設定
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// W座標のアドレスモードをラップに設定
		break;
	case PointClamp:
		desc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;	// ポイントサンプリング、クランプ
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;	// U座標のアドレスモードをクランプに設定
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;	// V座標のアドレスモードをクランプに設定
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;	// W座標のアドレスモードをクランプに設定
		break;
	case LinearWrap:
		desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	// リニアサンプリング、ラップ
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// U座標のアドレスモードをラップに設定
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// V座標のアドレスモードをラップに設定
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// W座標のアドレスモードをラップに設定
		break;
	case LinearClamp:
		desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	// リニアサンプリング、クランプ
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;	// U座標のアドレスモードをクランプに設定
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;	// V座標のアドレスモードをクランプに設定
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;	// W座標のアドレスモードをクランプに設定
		break;
	case AnisotropicWrap:
		desc.Filter = D3D12_FILTER_ANISOTROPIC;	// 異方性サンプリング、ラップ
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// U座標のアドレスモードをラップに設定
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// V座標のアドレスモードをラップに設定
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// W座標のアドレスモードをラップに設定
		desc.MaxAnisotropy = D3D12_MAX_MAXANISOTROPY;	// 最大異方性フィルタリングのレベルを最大値に設定
		break;
	case AnisotropicClamp:
		desc.Filter = D3D12_FILTER_ANISOTROPIC;	// 異方性サンプリング、クランプ
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;	// U座標のアドレスモードをクランプに設定
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;	// V座標のアドレスモードをクランプに設定
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;	// W座標のアドレスモードをクランプに設定
		desc.MaxAnisotropy = D3D12_MAX_MAXANISOTROPY;	// 最大異方性フィルタリングのレベルを最大値に設定
		break;
	}

	m_samplers.push_back(desc);	// スタティックサンプラーの配列に追加

	return *this; // 自身を返す
}

RootSignature::Desc& RootSignature::Desc::AllowIL()
{
	m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;	// 入力アセンブラ入力レイアウトを許可
	
	return *this; // 自身を返す
}

RootSignature::Desc& RootSignature::Desc::AllowSo()
{
	m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;	// ストリーム出力を許可

	return *this; // 自身を返す
}

RootSignature::Desc& RootSignature::Desc::End()
{
	if (m_DenyStage[0]) m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;	// 頂点シェーダーのルートアクセスを拒否
	if (m_DenyStage[1]) m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;	// ハルシェーダーのルートアクセスを拒否
	if (m_DenyStage[2]) m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;	// ドメインシェーダーのルートアクセスを拒否
	if (m_DenyStage[3]) m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;	// ジオメトリシェーダーのルートアクセスを拒否
	if (m_DenyStage[4]) m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;	// ピクセルシェーダーのルートアクセスを拒否

	m_desc.NumParameters = static_cast<UINT>(m_params.size());	// ルートパラメーターの数を設定
	m_desc.pParameters = m_params.data();	// ルートパラメーターのポインタを設定
	m_desc.NumStaticSamplers = static_cast<UINT>(m_samplers.size());	// スタティックサンプラーの数を設定
	m_desc.pStaticSamplers = m_samplers.data();	// スタティックサンプラーのポインタを設定
	m_desc.Flags = static_cast<D3D12_ROOT_SIGNATURE_FLAGS>(m_Flags);	// ルートシグネチャのフラグを設定

	return *this; // 自身を返す
}

void RootSignature::Desc::CheckStage(ShaderStage stage)
{
	auto index = static_cast<int>(stage - 1);

	if (0 <= index && index < 5)
	{
		m_DenyStage[index] = false;	// 指定されたステージのアクセスを許可
	}
}

void RootSignature::Desc::SetParam(ShaderStage stage, int index, uint32_t reg, D3D12_DESCRIPTOR_RANGE_TYPE type)
{
	if (index >= m_params.size()) return;	// インデックスが範囲外の場合は何もしない

	m_ranges[index].RangeType = type;	// ディスクリプタレンジのタイプを設定
	m_ranges[index].NumDescriptors = 1;	// ディスクリプタの数を1に設定
	m_ranges[index].BaseShaderRegister = reg;	// シェーダーレジスタを設定
	m_ranges[index].RegisterSpace = 0;	// レジスタスペースを0に設定
	m_ranges[index].OffsetInDescriptorsFromTableStart = 0;	// テーブルの開始からのオフセットを0に設定

	m_params[index].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// ルートパラメーターのタイプをディスクリプタテーブルに設定
	m_params[index].DescriptorTable.NumDescriptorRanges = 1;	// ディスクリプタレンジの数を1に設定
	m_params[index].DescriptorTable.pDescriptorRanges = &m_ranges[index];	// ディスクリプタレンジのポインタを設定
	m_params[index].ShaderVisibility = D3D12_SHADER_VISIBILITY(stage);	// シェーダーの可視性を設定
	CheckStage(stage);	// 指定されたステージのアクセスを許可
}

RootSignature::~RootSignature()
{
	Term(); // ルートシグネチャの終了処理
}

bool RootSignature::Init(ID3D12Device* pDevice, const D3D12_ROOT_SIGNATURE_DESC* pDesc)
{
	ComPtr<ID3DBlob> pBlob; // ルートシグネチャのバイナリデータを格納するポインタ
	ComPtr<ID3DBlob> pErrorBlob; // エラーメッセージを格納するポインタ

	// シリアライズ
	HRESULT hr = D3D12SerializeRootSignature(
		pDesc, 
		D3D_ROOT_SIGNATURE_VERSION_1, 
		pBlob.GetAddressOf(), 
		pErrorBlob.GetAddressOf()
	);
	if (FAILED(hr))
	{
		// エラーメッセージが存在する場合は表示
		if (pErrorBlob)
		{
			MessageBoxA(nullptr, static_cast<const char*>(pErrorBlob->GetBufferPointer()), "ルートシグネチャのシリアライズに失敗", MB_OK | MB_ICONERROR);
		}
		else
		{
			MessageBox(nullptr, "ルートシグネチャのシリアライズに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		}
		return false; // シリアライズに失敗した場合はfalseを返す
	}

	// ルートシグネチャの作成
	hr = pDevice->CreateRootSignature(
		0, // ノードマスク（通常は0）
		pBlob->GetBufferPointer(), // シリアライズされたルートシグネチャのポインタ
		pBlob->GetBufferSize(), // シリアライズされたルートシグネチャのサイズ
		IID_PPV_ARGS(m_rootSignature.GetAddressOf()) // ルートシグネチャのポインタを取得
	);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "ルートシグネチャの生成に失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		return false; // エラー終了
	}

	return true; // ルートシグネチャの初期化に成功した場合はtrueを返す
}

void RootSignature::Term()
{
	m_rootSignature.Reset(); // ルートシグネチャをリセット
}
