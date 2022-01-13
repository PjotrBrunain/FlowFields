#pragma once

namespace Elite
{
	template <class T_NodeType, class T_ConnectionType>
	class AStar
	{
	public:
		AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction);

		// stores the optimal connection to a node and its total costs related to the start and end node of the path
		struct NodeRecord
		{
			T_NodeType* pNode = nullptr;
			T_ConnectionType* pConnection = nullptr;
			float costSoFar = 0.f; // accumulated g-costs of all the connections leading up to this one
			float estimatedTotalCost = 0.f; // f-cost (= costSoFar + h-cost)

			bool operator==(const NodeRecord& other) const
			{
				return pNode == other.pNode
					&& pConnection == other.pConnection
					&& costSoFar == other.costSoFar
					&& estimatedTotalCost == other.estimatedTotalCost;
			};

			bool operator<(const NodeRecord& other) const
			{
				return estimatedTotalCost < other.estimatedTotalCost;
			};
		};

		std::vector<T_NodeType*> FindPath(T_NodeType* pStartNode, T_NodeType* pDestinationNode);

	private:
		float GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const;

		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
		Heuristic m_HeuristicFunction;
	};

	template <class T_NodeType, class T_ConnectionType>
	AStar<T_NodeType, T_ConnectionType>::AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction)
		: m_pGraph(pGraph)
		, m_HeuristicFunction(hFunction)
	{
	}

	template <class T_NodeType, class T_ConnectionType>
	std::vector<T_NodeType*> AStar<T_NodeType, T_ConnectionType>::FindPath(T_NodeType* pStartNode, T_NodeType* pGoalNode)
	{
		vector<T_NodeType*> path;
		vector<NodeRecord> openList;
		vector<NodeRecord> closedList;
		NodeRecord currentNode;
		openList.push_back(NodeRecord{ pStartNode, nullptr, 0, GetHeuristicCost(pStartNode, pGoalNode) });

		while (!openList.empty())
		{
			//A
			currentNode = *std::min_element(openList.begin(), openList.end());
			//B
			if (currentNode.pNode == pGoalNode)
			{
				closedList.push_back(currentNode);
				break;
			}
				
			//C
			{
				for (auto connection: m_pGraph->GetNodeConnections(currentNode.pNode))
				{
					T_NodeType* node{ m_pGraph->GetNode(connection->GetTo()) };
					float gCost{ /*GetHeuristicCost(pStartNode, node)*/ currentNode.costSoFar + connection->GetCost() };
					bool hasFound{};
					//assert(connection->GetFrom() == currentNode.pNode->GetIndex());
					//D
					for (size_t i = 0; i < closedList.size(); ++i)
					{
						if (closedList[i].pNode == node)
						{
							if (gCost > closedList[i].costSoFar) hasFound = true;
							else closedList.erase(closedList.begin() + i);
						}
					}
					if (hasFound) continue;
					else
					{
						//E
						for (size_t i = 0; i < openList.size(); ++i)
						{
							if (openList[i].pNode == node)
							{
								if (gCost > openList[i].costSoFar) hasFound = true;
								else openList.erase(openList.begin() + i);
							}
						}
						if (hasFound) continue;
					}
					//F
					openList.push_back(NodeRecord{ node, connection,gCost,gCost + GetHeuristicCost(node, pGoalNode) });
				}
			}
			//G
			{
				openList.erase(remove(openList.begin(), openList.end(), currentNode));
				closedList.push_back(currentNode);
			}
		}

		while (currentNode.pNode != pStartNode)
		{
			path.push_back(currentNode.pNode);
			for (size_t i = 0; i < closedList.size(); ++i)
			{
				if (closedList[i].pNode->GetIndex() == currentNode.pConnection->GetFrom())
				{
					currentNode = closedList[i];
					break;
				}
			}
		}
		path.push_back(pStartNode);
		std::reverse(path.begin(), path.end());

		return path;
	}

	template <class T_NodeType, class T_ConnectionType>
	float Elite::AStar<T_NodeType, T_ConnectionType>::GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const
	{
		Vector2 toDestination = m_pGraph->GetNodePos(pEndNode) - m_pGraph->GetNodePos(pStartNode);
		return m_HeuristicFunction(abs(toDestination.x), abs(toDestination.y));
	}
}