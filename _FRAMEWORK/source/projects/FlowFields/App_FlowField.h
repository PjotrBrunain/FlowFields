#pragma once

#include "framework/EliteAI/EliteGraphs/EGridGraph.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphUtilities/EGraphEditor.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphUtilities/EGraphRenderer.h"
#include "framework/EliteInterfaces/EIApp.h"
#include "projects/Movement/SteeringBehaviors/SteeringAgent.h"

class BlendedSteering;
class Wander;
class FollowFlowField;

class App_FlowField final: public IApp
{
public:
	App_FlowField() = default;
	virtual ~App_FlowField();

	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;

private:
	unsigned int m_Rows{ 100 };
	unsigned int m_Columns{ 100 };
	unsigned int m_SizeCell = 15;

	Elite::GridGraph<Elite::FlowFieldNode, Elite::GraphConnection>* m_pGridGraph;

	Elite::GraphEditor m_GraphEditor{};
	Elite::GraphRenderer m_GraphRenderer{};

	int m_GoalIdx{5};
	std::vector<Elite::Vector2> m_SpawnPoints{};
	bool m_SpawnAgents;
	std::vector<SteeringAgent*> m_pAgents;

	FollowFlowField* m_pFollowFlowField;
	Wander* m_pWander;
	BlendedSteering* m_pBlendedSteering;

	int m_AmountOfAgentsToSpawn{ 20 };

	bool m_DrawVectors{true};
	bool m_DrawCostGrid{};
	bool m_DrawIntegrationGrid{true};
	bool m_DrawGrid{};

	void MakeGridGraph();

	void CalculateIntegrationField();
	void CalculateFlowField();

	enum class Direction
	{
		N,
		NE,
		E,
		SE,
		S,
		SW,
		W,
		NW
	};
	struct NeighbourIdx
	{
		size_t idx{};
		Direction direction{};
	};

	void UpdateImGui();
	void SpawnAgent(const Elite::Vector2& pos);
};

