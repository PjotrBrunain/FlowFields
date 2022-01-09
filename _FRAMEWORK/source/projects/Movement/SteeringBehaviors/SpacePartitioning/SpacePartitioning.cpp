#include "stdafx.h"
#include "SpacePartitioning.h"
#include "projects\Movement\SteeringBehaviors\SteeringAgent.h"

// --- Cell ---
// ------------
Cell::Cell(float left, float bottom, float width, float height)
{
	boundingBox.bottomLeft = { left, bottom };
	boundingBox.width = width;
	boundingBox.height = height;
}

std::vector<Elite::Vector2> Cell::GetRectPoints() const
{
	auto left = boundingBox.bottomLeft.x;
	auto bottom = boundingBox.bottomLeft.y;
	auto width = boundingBox.width;
	auto height = boundingBox.height;

	std::vector<Elite::Vector2> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(float width, float height, int rows, int cols, int maxEntities)
	: m_SpaceWidth(width)
	, m_SpaceHeight(height)
	, m_NrOfRows(rows)
	, m_NrOfCols(cols)
	, m_Neighbors(maxEntities)
	, m_NrOfNeighbors(0)
{
	int amountOfCells{ m_NrOfCols * m_NrOfRows };
	m_CellWidth = m_SpaceWidth / m_NrOfCols;
	m_CellHeight = m_SpaceHeight / m_NrOfRows;
	for (int row = 0; row < m_NrOfRows; ++row)
	{
		for (int col = 0; col < m_NrOfCols; ++col)
		{
			m_Cells.emplace_back(col * m_CellWidth, row * m_CellHeight, m_CellWidth, m_CellHeight);
		}
	}
}

void CellSpace::AddAgent(SteeringAgent* agent)
{
	m_Cells[PositionToIndex(agent->GetPosition())].agents.push_back(agent);
}

void CellSpace::UpdateAgentCell(SteeringAgent* agent, Elite::Vector2 oldPos)
{
	int oldIdx{ PositionToIndex(oldPos) };
	int newIdx{ PositionToIndex(agent->GetPosition()) };

	if (oldIdx != newIdx)
	{
		m_Cells[newIdx].agents.push_back(agent);
		m_Cells[oldIdx].agents.remove(agent);
	}
}

void CellSpace::RegisterNeighbors(SteeringAgent* agent, float queryRadius)
{
	for (auto* pNeighbor : m_Neighbors) pNeighbor = nullptr;
	m_NrOfNeighbors = 0;
	int idxLeft{ PositionToIndex(agent->GetPosition() - Elite::Vector2{queryRadius,0.f}) };
	int colLeft{ idxLeft % m_NrOfCols };
	if (colLeft < 0) colLeft = 0;
	int idxRight{ PositionToIndex(agent->GetPosition() + Elite::Vector2{queryRadius,0.f}) };
	int colRight{ idxRight % m_NrOfCols };
	if (colRight > m_NrOfCols) colRight = m_NrOfCols;
	int idxBottom{ PositionToIndex(agent->GetPosition() - Elite::Vector2{0.f,queryRadius}) };
	int rowBottom{ idxBottom / m_NrOfCols };
	if (rowBottom < 0) rowBottom = 0;
	int idxTop{ PositionToIndex(agent->GetPosition() + Elite::Vector2{0.f,queryRadius}) };
	int rowTop{ idxTop / m_NrOfCols };
	if (rowTop > m_NrOfRows) rowTop = m_NrOfRows;
	for (int row = rowBottom; row < rowTop; ++row)
	{
		for (int col = colLeft; col < colRight; ++col)
		{
			const auto& agents = m_Cells[row * m_NrOfCols + col].agents;
			for (const auto& pAgent : agents)
			{
				if (pAgent != agent)
				{
					if (Elite::DistanceSquared(agent->GetPosition(), pAgent->GetPosition()) < queryRadius * queryRadius)
					{
						m_Neighbors[m_NrOfNeighbors] = pAgent;
						m_NrOfNeighbors++;
					}
				}
			}
		}
	}
}

void CellSpace::RenderCells() const
{
	for (size_t i = 0; i < m_Cells.size(); ++i)
	{
		const auto& cell = m_Cells[i];
		DEBUGRENDERER2D->DrawSegment(cell.GetRectPoints()[0], cell.GetRectPoints()[1], { 1.f,0.f,0.f });
		DEBUGRENDERER2D->DrawSegment(cell.GetRectPoints()[1], cell.GetRectPoints()[2], { 1.f,0.f,0.f });
		DEBUGRENDERER2D->DrawSegment(cell.GetRectPoints()[2], cell.GetRectPoints()[3], { 1.f,0.f,0.f });
		DEBUGRENDERER2D->DrawSegment(cell.GetRectPoints()[3], cell.GetRectPoints()[0], { 1.f,0.f,0.f });

		DEBUGRENDERER2D->DrawString(cell.GetRectPoints()[1], std::to_string(i).c_str());
		DEBUGRENDERER2D->DrawString(cell.GetRectPoints()[1] - Elite::Vector2{ 0.f,3.f }, std::to_string(cell.agents.size()).c_str());
	}
}

int CellSpace::PositionToIndex(const Elite::Vector2 pos) const
{
	int column = int (pos.x / m_CellWidth);
	int row = int (pos.y / m_CellHeight);
	if (pos.x >= m_SpaceWidth)
	{
		column = m_NrOfCols - 1;
	}
	if (pos.y >= m_SpaceHeight)
	{
		row = m_NrOfRows - 1;
	}

	return row * m_NrOfCols + column;
}