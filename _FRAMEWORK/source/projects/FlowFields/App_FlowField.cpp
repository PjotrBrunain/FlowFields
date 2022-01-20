#include "stdafx.h"
#include "App_FlowField.h"

#include "FlowFieldSteeringBehaviors.h"
#include "projects/Movement/SteeringBehaviors/CombinedSteering/CombinedSteeringBehaviors.h"

App_FlowField::~App_FlowField()
{
	SAFE_DELETE(m_pGridGraph);
	SAFE_DELETE(m_pBlendedSteering);
	SAFE_DELETE(m_pFollowFlowField);
	SAFE_DELETE(m_pWander);

	for (auto pAgent : m_pAgents)
	{
		SAFE_DELETE(pAgent);
	}
}

void App_FlowField::Start()
{
	//m_CostField.resize(m_Columns * m_Rows);
	//std::for_each(m_CostField.begin(), m_CostField.end(), [](BYTE& byte) {byte = BYTE(Elite::randomInt(255)); });
	MakeGridGraph();

	CalculateFlowField();

	m_pFollowFlowField = new FollowFlowField{ m_pGridGraph };
	m_pWander = new Wander{};
	m_pBlendedSteering = new BlendedSteering({ BlendedSteering::WeightedBehavior{ m_pFollowFlowField, 1.f }, BlendedSteering::WeightedBehavior{m_pWander, 1.f} });
}

void App_FlowField::Update(float deltaTime)
{
	if (INPUTMANAGER->IsMouseButtonUp(Elite::eLeft))
	{
		auto mouseData = INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, Elite::eLeft);
		auto mousePos = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld(Elite::Vector2{ float(mouseData.X), float(mouseData.Y) });

		auto node = m_pGridGraph->GetNodeAtWorldPos(mousePos);
		if (node)
			m_GoalIdx = node->GetIndex();

		CalculateFlowField();
	}
	if (INPUTMANAGER->IsMouseButtonDown(Elite::eMiddle))
	{
		auto mouseData = INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, Elite::eMiddle);
		auto mousePos = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld(Elite::Vector2{ float(mouseData.X), float(mouseData.Y) });
		m_SpawnPoints.push_back(mousePos);
	}

	UpdateImGui();

	auto nodes = m_pGridGraph->GetAllActiveNodes();
	std::for_each(nodes.begin(), nodes.end(), [this](Elite::FlowFieldNode* node)
		{
			node->SetShowCost(m_DrawCostGrid);
			node->SetShowIntegration(m_DrawIntegrationGrid);
		});

	if (m_SpawnAgents)
	{
		for (auto spawnPoint : m_SpawnPoints)
		{
			for (int i = 0; i < m_AmountOfAgentsToSpawn; ++i)
			{
				SpawnAgent(spawnPoint);
			}
		}
	}

	for (auto pAgent : m_pAgents)
	{
		pAgent->Update(deltaTime);
	}
}

void App_FlowField::Render(float deltaTime) const
{
	if (m_DrawGrid || m_DrawIntegrationGrid || m_DrawCostGrid)
	{
		m_GraphRenderer.RenderGraph(m_pGridGraph, true, false, false, false);
	}

	if (m_DrawVectors)
	{
		auto nodes = m_pGridGraph->GetAllActiveNodes();

		for (auto node : nodes)
		{
			if (node->GetIntegrationValue() != std::numeric_limits<size_t>::max())
				DEBUGRENDERER2D->DrawDirection(m_pGridGraph->GetNodeWorldPos(node->GetIndex()), node->GetDirection(), float(m_SizeCell) / 2.f, Elite::Color{ 0.f,0.f,255.f });
		}
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
	auto nodes = m_pGridGraph->GetAllActiveNodes();
	std::for_each(nodes.begin(), nodes.end(), [](Elite::FlowFieldNode* node) {node->SetIntegrationValue(std::numeric_limits<size_t>::max()); });

	std::vector<Elite::FlowFieldNode*> openList{};
	m_pGridGraph->GetNode(m_GoalIdx)->SetIntegrationValue(0);
	openList.push_back(m_pGridGraph->GetNode(m_GoalIdx));

	while (!openList.empty())
	{
		Elite::FlowFieldNode* currentNode = openList.front();
		openList.erase(remove(openList.begin(), openList.end(), currentNode));
		auto connections{ m_pGridGraph->GetNodeConnections(currentNode->GetIndex()) };
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

void App_FlowField::UpdateImGui()
{
#ifdef PLATFORM_WINDOWS
#pragma region UI
	{
		//Setup
		int menuWidth = 115;
		int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
		int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
		bool windowActive = true;
		ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
		ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 20));
		ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::PushAllowKeyboardFocus(false);

		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: SetGoal");
		ImGui::Text("MMB: SetSpawnPoint");
		ImGui::Unindent();

		/*Spacing*/ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		/*Spacing*/ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); ImGui::Spacing();

		ImGui::Text("FlowField");
		ImGui::Spacing();

		m_SpawnAgents = ImGui::Button("SpawnAgents");
		ImGui::InputInt("AmountToSpawn", &m_AmountOfAgentsToSpawn);

		ImGui::Checkbox("Grid", &m_DrawGrid);
		ImGui::Checkbox("CostGrid", &m_DrawCostGrid);
		ImGui::Checkbox("IntegrationField", &m_DrawIntegrationGrid);
		ImGui::Checkbox("VectorField", &m_DrawVectors);

		ImGui::PopAllowKeyboardFocus();
		ImGui::End();
	}
#pragma endregion
#endif
}

void App_FlowField::SpawnAgent(const Elite::Vector2& pos)
{
	SteeringAgent* pAgent = new SteeringAgent{};
	pAgent->SetPosition(pos);
	pAgent->SetAutoOrient(true);
	pAgent->SetMaxLinearSpeed(55.f);
	pAgent->SetMaxAngularSpeed(25.f);
	pAgent->SetMass(1);
	pAgent->SetSteeringBehavior(m_pBlendedSteering);
	m_pAgents.push_back(pAgent);
}
