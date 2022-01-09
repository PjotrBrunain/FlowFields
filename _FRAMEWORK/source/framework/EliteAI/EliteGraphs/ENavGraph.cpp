#include "stdafx.h"
#include "ENavGraph.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EAStar.h"

using namespace Elite;

Elite::NavGraph::NavGraph(const Polygon& contourMesh, float playerRadius = 1.0f) :
	Graph2D(false),
	m_pNavMeshPolygon(nullptr)
{
	//Create the navigation mesh (polygon of navigatable area= Contour - Static Shapes)
	m_pNavMeshPolygon = new Polygon(contourMesh); // Create copy on heap

	//Get all shapes from all static rigidbodies with NavigationCollider flag
	auto vShapes = PHYSICSWORLD->GetAllStaticShapesInWorld(PhysicsFlags::NavigationCollider);

	//Store all children
	for (auto shape : vShapes)
	{
		shape.ExpandShape(playerRadius);
		m_pNavMeshPolygon->AddChild(shape);
	}

	//Triangulate
	m_pNavMeshPolygon->Triangulate();

	//Create the actual graph (nodes & connections) from the navigation mesh
	CreateNavigationGraph();
}

Elite::NavGraph::~NavGraph()
{
	delete m_pNavMeshPolygon;
	m_pNavMeshPolygon = nullptr;
}

int Elite::NavGraph::GetNodeIdxFromLineIdx(int lineIdx) const
{
	auto nodeIt = std::find_if(m_Nodes.begin(), m_Nodes.end(), [lineIdx](const NavGraphNode* n) { return n->GetLineIndex() == lineIdx; });
	if (nodeIt != m_Nodes.end())
	{
		return (*nodeIt)->GetIndex();
	}

	return invalid_node_index;
}

Elite::Polygon* Elite::NavGraph::GetNavMeshPolygon() const
{
	return m_pNavMeshPolygon;
}

void Elite::NavGraph::CreateNavigationGraph()
{
	//1. Go over all the edges of the navigationmesh and create nodes
	auto edges = m_pNavMeshPolygon->GetLines();
	for (auto* edge : edges)
	{
		if (m_pNavMeshPolygon->GetTrianglesFromLineIndex(edge->index).size() > 1)
		{
			AddNode(new NavGraphNode{ GetNextFreeNodeIndex(), edge->index, {(edge->p1.x + edge->p2.x) / 2.f, (edge->p1.y + edge->p2.y) / 2.f} });
		}

	}
	
	//2. Create connections now that every node is created
	for (auto* triangle : m_pNavMeshPolygon->GetTriangles())
	{
		std::vector<NavGraphNode*> lineNodes{};
		for (auto lineIdx : triangle->metaData.IndexLines)
		{
			for (auto* node : GetAllNodes())
			{
				if (node->GetLineIndex() == lineIdx)
				{
					lineNodes.push_back(node);
					break;
				}
			}
		}

		if (lineNodes.size() == 2)
			AddConnection(new GraphConnection2D{ lineNodes[0]->GetIndex(), lineNodes[1]->GetIndex() });
		if (lineNodes.size() == 3)
		{
			AddConnection(new GraphConnection2D{ lineNodes[0]->GetIndex(), lineNodes[1]->GetIndex() });
			AddConnection(new GraphConnection2D{ lineNodes[1]->GetIndex(), lineNodes[2]->GetIndex() });
			AddConnection(new GraphConnection2D{ lineNodes[2]->GetIndex(), lineNodes[0]->GetIndex() });
		}
	}
	
	//3. Set the connections cost to the actual distance
	SetConnectionCostsToDistance();
}

