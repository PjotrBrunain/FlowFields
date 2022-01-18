#pragma once

#include "framework/EliteAI/EliteGraphs/EGridGraph.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphUtilities/EGraphEditor.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphUtilities/EGraphRenderer.h"
#include "framework/EliteInterfaces/EIApp.h"

class App_FlowField final: public IApp
{
public:
	App_FlowField() = default;
	virtual ~App_FlowField();

	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;

private:
	unsigned int m_Rows{ 10 };
	unsigned int m_Columns{ 10 };
	unsigned int m_SizeCell = 15;

	Elite::GridGraph<Elite::FlowFieldNode, Elite::GraphConnection>* m_pGridGraph;

	Elite::GraphEditor m_GraphEditor{};
	Elite::GraphRenderer m_GraphRenderer{};

	std::vector<BYTE> m_CostField{};
	std::vector<size_t> m_IntegrationField{m_Rows*m_Columns,std::numeric_limits<size_t>::max()};
	std::vector<Elite::Vector2> m_FlowField{ m_Rows * m_Columns,Elite::Vector2{} };
	int m_GoalIdx{5};

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

	std::vector<NeighbourIdx> GetNeighbours(size_t currentIdx) const;
};

