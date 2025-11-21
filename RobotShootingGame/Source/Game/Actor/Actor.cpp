/*+===================================================================
	File: Actor.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/14 11:45:12 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Actor.hpp"
#include "Utility/SharedStruct/SharedStruct.hpp"

Actor::Actor()
{
	// 定数バッファの生成
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_pWorldCB[i] = new ConstantBuffer(sizeof(CB::World));
		assert(m_pWorldCB[i]);	// nullptrチェック
		CB::World* ptr = m_pWorldCB[i]->GetPtr<CB::World>();
		ptr->WorldMat = m_Transform.GetWorldMatrixFloat4x4();
	}

	// ルートシグネチャの生成
	m_pRootSignature = new RootSignature();
	assert(m_pRootSignature);	// nullptrチェック
	m_pRootSignature->AddRootParameter(0, D3D12_SHADER_VISIBILITY_VERTEX);	// 定数バッファビュー
	// Createは派生クラスで行う(忘れない)
}

Actor::~Actor()
{
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		if (m_pWorldCB[i])
		{
			delete m_pWorldCB[i];
			m_pWorldCB[i] = nullptr;
		}
	}

	if (m_pRootSignature)
	{
		delete m_pRootSignature;
		m_pRootSignature = nullptr;
	}

	for (auto& component : m_Components)
	{
		component->Uninit();
		delete component;
	}
	m_Components.clear();
}

void Actor::Update()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得
	CB::World* ptr = m_pWorldCB[currentIndex]->GetPtr<CB::World>();			// 定数バッファのポインタを取得
	ptr->WorldMat = m_Transform.GetWorldMatrixFloat4x4();					// ワールド行列を更新

	for (auto& component : m_Components)
	{
		component->Update();
	}
}

void Actor::Draw()
{
	auto currentIndex = Engine::GetInstance().GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックスを取得
	auto commandList = Engine::GetInstance().GetCommandList();				// コマンドリストを取得

	// ルートシグネチャの初期化確認
	if (!m_pRootSignature->IsCreated() || !m_pRootSignature)
	{
		assert(false && "RootSignature::Create() が呼ばれていません。初期化不備です。");
		throw std::runtime_error(std::string("RootSignature::Create() が呼ばれていません。継承クラスで呼び出してください。") + typeid(*this).name());
	}

	// ルートシグネチャの設定
	commandList->SetGraphicsRootSignature(m_pRootSignature->Get());
	// 定数バッファの設定
	commandList->SetGraphicsRootConstantBufferView(0, m_pWorldCB[currentIndex]->GetAddress());

	for (auto& component : m_Components)
	{
		component->Draw();
	}
}
