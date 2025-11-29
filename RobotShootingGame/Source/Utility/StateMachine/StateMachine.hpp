/*+===================================================================
	File: StateMachine.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/28 13:50:46 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>

template<typename OwnerType>
class StateMachine;

template<typename OwnerType>
class NodeBase
{
public:
	NodeBase() = default;
	virtual ~NodeBase() = default;

private:
};

template<typename OwnerType>
class EdgeBase
{
public:
	EdgeBase(std::shared_ptr<NodeBase<OwnerType>> from, std::shared_ptr<NodeBase<OwnerType>> to, std::function<bool()> condition)
		:m_wpFromNode(from), m_wpToNode(to), m_fnCondition(condition)
	{
	}

	bool CanTransition()
	{
		return m_fnCondition ? m_fnCondition() : false;
	}

private:
	std::weak_ptr<NodeBase<OwnerType>> m_wpFromNode{};
	std::weak_ptr<NodeBase<OwnerType>> m_wpToNode{};
	std::function<bool()> m_fnCondition{};
};

template<typename OwnerType>
class StateMachine
{
public:
	StateMachine(OwnerType* pOwner)
		:m_pOwner(pOwner)
	{
	}

	void Update()
	{

	}

	void AddNode(const std::string& name, const std::shared_ptr<NodeBase<OwnerType>>& node)
	{
		auto node = std::make_shared<NodeBase<OwnerType>>();
		m_Nodes[name] = node;
	}

	void AddEdge(std::string from, std::string to, std::function<bool()> condition)
	{
		auto fromNode = GetNode(from);
		auto toNode = GetNode(to);
		if (!fromNode || !toNode) return;

		auto edge = std::make_shared<EdgeBase<OwnerType>>(fromNode, toNode, condition);
		m_Edges.push_back(edge);
	}

	std::shared_ptr<NodeBase<OwnerType>> GetNode(const std::string& name)
	{
		auto it = m_Nodes.find(name);
		if (it != m_Nodes.end())
		{
			return it->second;
		}
		return nullptr;
	}

private:
	bool NodeUpdate()
	{
		if (!m_spNowState) return false;
		return true;
	}

	OwnerType* m_pOwner{};
	std::shared_ptr<NodeBase<OwnerType>> m_spNowState{};

	std::unordered_map<std::string, std::shared_ptr<NodeBase<OwnerType>>> m_Nodes{};
	std::vector<std::shared_ptr<EdgeBase<OwnerType>>> m_Edges{};
};
