#include "stdafx.h"
#include "App_FlowField.h"

App_FlowField::~App_FlowField()
{
	SAFE_DELETE(m_pGridGraph);
}

void App_FlowField::Start()
{
	//m_CostField.resize(m_Columns * m_Rows);
	//std::for_each(m_CostField.begin(), m_CostField.end(), [](BYTE& byte) {byte = BYTE(Elite::randomInt(255)); });
	MakeGridGraph();

	CalculateFlowField();
}

void App_FlowField::Update(float deltaTime)
{
}

void App_FlowField::Render(float deltaTime) const
{
	m_GraphRenderer.RenderGraph(m_pGridGraph, true, true,false,false);

	auto nodes = m_pGridGraph->GetAllActiveNodes();

	for (auto node : nodes)
	{
		DEBUGRENDERER2D->DrawDirection(m_pGridGraph->GetNodeWorldPos(node->GetIndex()), node->GetDirection(), float(m_SizeCell) / 2.f, Elite::Color{ 255.f,0.f,0.f });
	}
}

void App_FlowField::MakeGridGraph()
{
	m_pGridGraph = new Elite::GridGraph<Elite::FlowFieldNode, Elite::GraphConnection>(m_Columns, m_Rows, m_SizeCell, false, true, 1.f, 1.f);

	m_pGridGraph->GetNode(86)->SetCost(BYTE{ 255 });
	m_pGridGraph->GetNode(66)->SetCost(BYTE{ 255 });
	m_pGridGraph->GetNode(67)->SetCost(BYTE{ 255 });
	m_pGridGraph->GetNode(47)->SetCost(BYTE{ 255 });
}

void App_FlowField::CalculateIntegrationField()
{
	//std::for_each(m_IntegrationField.begin(), m_IntegrationField.end(), [](size_t& cellVallue) {cellVallue = std::numeric_limits<size_t>::max(); });
	auto nodes = m_pGridGraph->GetAllActiveNodes();
	std::for_each(nodes.begin(), nodes.end(), [](Elite::FlowFieldNode* node) {node->SetIntegrationValue(std::numeric_limits<size_t>::max()); });

	std::vector<Elite::FlowFieldNode*> openList{};
	m_pGridGraph->GetNode(m_GoalIdx)->SetIntegrationValue(0);
	//m_IntegrationField[m_GoalIdx] = 0;
	openList.push_back(m_pGridGraph->GetNode(m_GoalIdx));

	while (!openList.empty())
	{
		Elite::FlowFieldNode* currentNode = openList.front();
		openList.erase(remove(openList.begin(), openList.end(), currentNode));

		

		//std::vector<NeighbourIdx> neighbours{ GetNeighbours(currentNode) };
		//size_t neighbourCount{ neighbours.size() };
		auto connections{ m_pGridGraph->GetNodeConnections(currentNode->GetIndex()) };

		//for (NeighbourIdx neighbour : neighbours)
		//{
		//	size_t cost{ m_IntegrationField[currentNode] + m_CostField[neighbour.idx] };
		//	if (cost < m_IntegrationField[neighbour.idx])
		//	{
		//		if (std::find(openList.begin(), openList.end(), neighbour) == openList.end())
		//		{
		//			openList.push_back(neighbour.idx);
		//		}
		//
		//		m_IntegrationField[neighbour.idx] = cost;
		//	}
		//}
		for (auto connection : connections)
		{
			Elite::FlowFieldNode* neighbour{ m_pGridGraph->GetNode(connection->GetTo()) };
			if (neighbour->GetCost() != BYTE(255))
			{
				size_t cost{ currentNode->GetIntegrationValue() + neighbour->GetCost() };
				if (cost < neighbour->GetIntegrationValue())
				{
					if (std::find(openList.begin(), openList.end(), neighbour) == openList.end())
						openList.push_back(neighbour);
					neighbour->SetIntegrationValue(cost);
				}
			}

		}
	}
}

void App_FlowField::CalculateFlowField()
{
	CalculateIntegrationField();
	//for (size_t i = 0; i < m_FlowField.size(); ++i)
	//{
	//	std::vector<NeighbourIdx> neighbours{ GetNeighbours(i) };
	//	NeighbourIdx lowest{};
	//	for (NeighbourIdx neighbour : neighbours)
	//	{
	//		if (m_IntegrationField[neighbour.idx] < m_IntegrationField[lowest.idx])
	//			lowest = neighbour;
	//	}
	/*
	//	switch (lowest.direction)
	//	{
	//	case Direction::N: 
	//		m_FlowField[i] = Elite::Vector2{ 0.f,1.f };
	//		break;
	//	case Direction::NE: 
	//		m_FlowField[i] = Elite::Vector2{ 1.f,1.f }.GetNormalized();
	//		break;
	//	case Direction::E: 
	//		m_FlowField[i] = Elite::Vector2{ 1.f,0.f };
	//		break;
	//	case Direction::SE: 
	//		m_FlowField[i] = Elite::Vector2{ 1.f,-1.f }.GetNormalized();
	//		break;
	//	case Direction::S: 
	//		m_FlowField[i] = Elite::Vector2{ 0.f,-1.f };
	//		break;
	//	case Direction::SW: 
	//		m_FlowField[i] = Elite::Vector2{ -1.f,-1.f }.GetNormalized();
	//		break;
	//	case Direction::W: 
	//		m_FlowField[i] = Elite::Vector2{ -1.f,0.f };
	//		break;
	//	case Direction::NW: 
	//		m_FlowField[i] = Elite::Vector2{ -1.f,1.f }.GetNormalized();
	//		break;
	//	}
	//}
	*/
	const auto nodes = m_pGridGraph->GetAllActiveNodes();
	for (const auto node : nodes)
	{
		std::list<Elite::GraphConnection*> connections = m_pGridGraph->GetNodeConnections(node->GetIndex());
		Elite::GraphConnection* lowestConnection{ connections.front() };
		for (const auto connection : connections)
		{
			if (m_pGridGraph->GetNode(connection->GetTo())->GetIntegrationValue() < m_pGridGraph->GetNode(lowestConnection->GetTo())->GetIntegrationValue())
				lowestConnection = connection;
		}

		Elite::Vector2 direction{ m_pGridGraph->GetNodeWorldPos(lowestConnection->GetTo()) - m_pGridGraph->GetNodeWorldPos(node->GetIndex()) };
		direction.Normalize();
		node->SetDirection(direction);
	}
}
