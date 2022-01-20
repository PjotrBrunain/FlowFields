#pragma once
//#include "framework/EliteAI/EliteGraphs/EGraphConnectionTypes.h"
//#include "framework/EliteAI/EliteGraphs/EGraphNodeTypes.h"
#include "framework/EliteAI/EliteGraphs/EGridGraph.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"

class FollowFlowField : public ISteeringBehavior
{
public:
	FollowFlowField(Elite::GridGraph<Elite::FlowFieldNode, Elite::GraphConnection>* pGridGraph) :m_pGridGraph{pGridGraph} {}

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

private:
	Elite::GridGraph<Elite::FlowFieldNode, Elite::GraphConnection>* m_pGridGraph;
};