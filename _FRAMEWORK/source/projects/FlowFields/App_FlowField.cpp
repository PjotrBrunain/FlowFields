#include "stdafx.h"
#include "App_FlowField.h"

void App_FlowField::Start()
{
	m_CostField.resize(m_Columns * m_Rows);
	std::for_each(m_CostField.begin(), m_CostField.end(), [](BYTE& byte) {byte = BYTE(Elite::randomInt(255)); });

	CalculateFlowField();
}

void App_FlowField::Update(float deltaTime)
{
}

void App_FlowField::Render(float deltaTime) const
{
}

std::vector<App_FlowField::NeighbourIdx> App_FlowField::GetNeighbours(size_t currentIdx) const
{
	std::vector<App_FlowField::NeighbourIdx> neighbours{};
	size_t N = currentIdx - m_Columns;
	if (N < currentIdx)
		neighbours.push_back({ N, Direction::N });
	size_t E = currentIdx + 1;
	if (E % m_Columns != 0 && E > currentIdx)
		neighbours.push_back({ E, Direction::E });
	size_t S = currentIdx + m_Columns;
	if (S > currentIdx)
		neighbours.push_back({ S, Direction::S });
	size_t W = currentIdx - 1;
	if (W % m_Columns != m_Columns - 1 && W < currentIdx)
		neighbours.push_back({ W, Direction::W });
	size_t NE = N + 1;
	if (NE % m_Columns != 0 && NE < currentIdx)
		neighbours.push_back({ NE , Direction::NE});
	size_t SE = S + 1;
	if (SE % m_Columns != 0 && SE > currentIdx)
		neighbours.push_back({ SE, Direction::SE });
	size_t SW = S - 1;
	if (SW % m_Columns != m_Columns - 1 && SW > currentIdx)
		neighbours.push_back({ SW, Direction::SW});
	size_t NW = N - 1;
	if (NW % m_Columns != m_Columns - 1 && NW < currentIdx)
		neighbours.push_back({ NW, Direction::NW });
	return neighbours;
}

void App_FlowField::CalculateIntegrationField()
{
	std::for_each(m_IntegrationField.begin(), m_IntegrationField.end(), [](size_t& cellVallue) {cellVallue = std::numeric_limits<size_t>::max(); });

	std::vector<size_t> openList{};
	m_IntegrationField[m_GoalIdx] = 0;
	openList.push_back(m_GoalIdx);

	while (!openList.empty())
	{
		size_t currentIdx = openList.front();
		openList.erase(remove(openList.begin(), openList.end(), currentIdx));

		

		std::vector<NeighbourIdx> neighbours{ GetNeighbours(currentIdx) };
		//size_t neighbourCount{ neighbours.size() };

		for (NeighbourIdx neighbour : neighbours)
		{
			size_t cost{ m_IntegrationField[currentIdx] + m_CostField[neighbour.idx] };
			if (cost < m_IntegrationField[neighbour.idx])
			{
				if (std::find(openList.begin(), openList.end(), neighbour) == openList.end())
				{
					openList.push_back(neighbour.idx);
				}

				m_IntegrationField[neighbour.idx] = cost;
			}
		}
	}
}

void App_FlowField::CalculateFlowField()
{
	CalculateIntegrationField();
	for (size_t i = 0; i < m_FlowField.size(); ++i)
	{
		std::vector<NeighbourIdx> neighbours{ GetNeighbours(i) };
		NeighbourIdx lowest{};
		for (NeighbourIdx neighbour : neighbours)
		{
			if (m_IntegrationField[neighbour.idx] < m_IntegrationField[lowest.idx])
				lowest = neighbour;
		}

		switch (lowest.direction)
		{
		case Direction::N: 
			m_FlowField[i] = Elite::Vector2{ 0.f,1.f };
			break;
		case Direction::NE: 
			m_FlowField[i] = Elite::Vector2{ 1.f,1.f }.GetNormalized();
			break;
		case Direction::E: 
			m_FlowField[i] = Elite::Vector2{ 1.f,0.f };
			break;
		case Direction::SE: 
			m_FlowField[i] = Elite::Vector2{ 1.f,-1.f }.GetNormalized();
			break;
		case Direction::S: 
			m_FlowField[i] = Elite::Vector2{ 0.f,-1.f };
			break;
		case Direction::SW: 
			m_FlowField[i] = Elite::Vector2{ -1.f,-1.f }.GetNormalized();
			break;
		case Direction::W: 
			m_FlowField[i] = Elite::Vector2{ -1.f,0.f };
			break;
		case Direction::NW: 
			m_FlowField[i] = Elite::Vector2{ -1.f,1.f }.GetNormalized();
			break;
		}
	}
}
