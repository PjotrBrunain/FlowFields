#pragma once
#include "../SteeringHelpers.h"
#include "FlockingSteeringBehaviors.h"
#include "projects/Movement/SteeringBehaviors/SpacePartitioning/SpacePartitioning.h"

class ISteeringBehavior;
class SteeringAgent;
class BlendedSteering;
class PrioritySteering;


#define USE_SPACE_PARTITIONING

class Flock
{
public:
	Flock(
		int flockSize = 50,
		float worldSize = 100.f,
		SteeringAgent* pAgentToEvade = nullptr,
		bool trimWorld = false);

	~Flock();

	void Update(float deltaT);
	void UpdateAndRenderUI();
	void Render(float deltaT);

#ifdef USE_SPACE_PARTITIONING
	const vector<SteeringAgent*>& GetNeighbors() const { if (m_UsingPartitioning) return m_pPartitionedSpace->GetNeighbors(); return m_Neighbors; }
	int GetNrOfNeighbors() const { if (m_UsingPartitioning) return m_pPartitionedSpace->GetNrOfNeighbors(); return m_NrOfNeighbors; }

#else // No space partitioning
	int GetNrOfNeighbors() const { return m_NrOfNeighbors; }
	const vector<SteeringAgent*>& GetNeighbors() const { return m_Neighbors; }
#endif // USE_SPACE_PARTITIONING

	void RegisterNeighbors(SteeringAgent* pAgent);


	Elite::Vector2 GetAverageNeighborPos() const;
	Elite::Vector2 GetAverageNeighborVelocity() const;

	void SetSeekTarget(TargetData target);
	void SetWorldTrimSize(float size) { m_WorldSize = size; }

private:
	//Datamembers
	int m_FlockSize = 0;
	vector<SteeringAgent*> m_Agents;


	CellSpace* m_pPartitionedSpace = nullptr;
	int m_NrOfCellsX{ 25 };

	vector<Elite::Vector2> m_OldPositions = {};


	vector<SteeringAgent*> m_Neighbors;


	bool m_TrimWorld = false;
	float m_WorldSize = 0.f;

	float m_NeighborhoodRadius = 10.f;
	int m_NrOfNeighbors = 0;

	bool m_CanRenderDebug = false;

	SteeringAgent* m_pAgentToEvade = nullptr;

	//Steering Behaviors
	Separation* m_pSeparationBehavior = nullptr;
	Cohesion* m_pCohesionBehavior = nullptr;
	VelocityMatch* m_pVelMatchBehavior = nullptr;
	Seek* m_pSeekBehavior = nullptr;
	Wander* m_pWanderBehavior = nullptr;
	Evade* m_pEvadeBehavior = nullptr;

	BlendedSteering* m_pBlendedSteering = nullptr;
	PrioritySteering* m_pPrioritySteering = nullptr;

	float* GetWeight(ISteeringBehavior* pBehaviour);

	bool m_UsingPartitioning{};

private:
	Flock(const Flock& other);
	Flock& operator=(const Flock& other);
};