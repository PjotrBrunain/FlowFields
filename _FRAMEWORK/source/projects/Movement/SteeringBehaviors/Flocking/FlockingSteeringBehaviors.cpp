#include "stdafx.h"
#include "FlockingSteeringBehaviors.h"
#include "TheFlock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"

//*******************
//COHESION (FLOCKING)
SteeringOutput Cohesion::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	m_Target = m_pFlock->GetAverageNeighborPos();
	return Seek::CalculateSteering(deltaT, pAgent);
}

//*********************
//SEPARATION (FLOCKING)

SteeringOutput Separation::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};

	int nrOfNeigbours{ m_pFlock->GetNrOfNeighbors() };
	if (nrOfNeigbours == 0) return steering;

	Elite::Vector2 newVelocity{};
	float distance{};
	for (int i = 0; i < nrOfNeigbours; i++)
	{
		SteeringAgent* neighbour = m_pFlock->GetNeighbors()[i];
		distance = Elite::DistanceSquared(pAgent->GetPosition(), neighbour->GetPosition());
		if (distance < m_FleeRadius * m_FleeRadius)
		{
			newVelocity = pAgent->GetPosition() - neighbour->GetPosition();
			Elite::Normalize(newVelocity);
			newVelocity *= /*pAgent->GetMaxLinearSpeed() **/ (1 - distance / (m_FleeRadius * m_FleeRadius)) /** 10.f*/;
			steering.LinearVelocity += newVelocity;
		}
	}

	Elite::Normalize(steering.LinearVelocity);

	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();
	
	return steering;
}

//*************************
//VELOCITY MATCH (FLOCKING)

SteeringOutput VelocityMatch::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};

	int nrOfNeighbours{ m_pFlock->GetNrOfNeighbors() };
	if (nrOfNeighbours == 0) return steering;

	Elite::Vector2 avgVelocity{};
	for (int i = 0; i < nrOfNeighbours; ++i)
	{
		SteeringAgent* neighbour = m_pFlock->GetNeighbors()[i];
		avgVelocity += neighbour->GetLinearVelocity();
	}
	steering.LinearVelocity = avgVelocity / float(nrOfNeighbours);

	return steering;
}
