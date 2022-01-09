#pragma once
#include <vector>
#include <iostream>
#include "framework/EliteMath/EMath.h"
#include "framework\EliteAI\EliteGraphs\ENavGraph.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EAStar.h"

namespace Elite
{
	class NavMeshPathfinding
	{
	public:
		static std::vector<Elite::Vector2> FindPath(Elite::Vector2 startPos, Elite::Vector2 endPos, Elite::NavGraph* pNavGraph, std::vector<Elite::Vector2>& debugNodePositions, std::vector<Elite::Portal>& debugPortals)
		{
			//Create the path to return
			std::vector<Elite::Vector2> finalPath{};

			//Get the start and endTriangle

			auto* pStartTriangle = pNavGraph->GetNavMeshPolygon()->GetTriangleFromPosition(startPos);
			auto* pEndTriangle = pNavGraph->GetNavMeshPolygon()->GetTriangleFromPosition(endPos);

			if (pStartTriangle == nullptr || pEndTriangle == nullptr || pStartTriangle == pEndTriangle) return finalPath;
		
			//We have valid start/end triangles and they are not the same
			//=> Start looking for a path
			//Copy the graph
			auto sharedGraph = pNavGraph->Clone();
			auto* pGraph = sharedGraph.get();
			
			//Create extra node for the Start Node (Agent's position
			NavGraphNode* pStartNode = new NavGraphNode{ pGraph->GetNextFreeNodeIndex(), -1, startPos };

			pGraph->AddNode(pStartNode);
			
			for (auto lineIdx : pStartTriangle->metaData.IndexLines)
			{
				for (auto* node : pGraph->GetAllNodes())
				{
					if (node->GetLineIndex() == lineIdx)
					{
						pGraph->AddConnection(new GraphConnection2D{ pStartNode->GetIndex(), node->GetIndex(), Distance(pStartNode->GetPosition(), node->GetPosition()) });
						break;
					}
				}
			}
			
			//Create extra node for the endNode
			NavGraphNode* pEndNode = new NavGraphNode{ pGraph->GetNextFreeNodeIndex(), -1, endPos };

			pGraph->AddNode(pEndNode);
			
			for (auto lineIdx : pEndTriangle->metaData.IndexLines)
			{
				for (auto* node : pGraph->GetAllNodes())
				{
					if (node->GetLineIndex() == lineIdx)
					{
						pGraph->AddConnection(new GraphConnection2D{ pEndNode->GetIndex(), node->GetIndex(), Distance(pEndNode->GetPosition(), node->GetPosition()) });
						break;
					}
				}
			}
			
			//Run A star on new graph

			auto pathFinder = AStar<NavGraphNode, GraphConnection2D>(pGraph, HeuristicFunctions::Chebyshev);
			auto path = pathFinder.FindPath(pStartNode, pEndNode);
			debugNodePositions.clear();
			std::for_each(path.begin(), path.end(), [&finalPath, &debugNodePositions] (const NavGraphNode* node) mutable
				{
					//finalPath.push_back(node->GetPosition());
					debugNodePositions.push_back(node->GetPosition());
				});
			
			//OPTIONAL BUT ADVICED: Debug Visualisation
			
			//Run optimiser on new graph, MAKE SURE the A star path is working properly before starting this section and uncommenting this!!!
			auto portals = SSFA::FindPortals(path, pNavGraph->GetNavMeshPolygon());
			debugPortals = portals;
			finalPath = SSFA::OptimizePortals(portals);

			return finalPath;
		}
	};
}
