/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// Behaviors.h: Implementation of certain reusable behaviors for the BT version of the Agario Game
/*=============================================================================*/
#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
#define ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteMath/EMath.h"
#include "framework/EliteAI/EliteDecisionMaking/EliteBehaviorTree/EBehaviorTree.h"
#include "projects/Shared/Agario/AgarioAgent.h"
#include "projects/Shared/Agario/AgarioFood.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"

//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------

// ACTIONS
//***********

inline Elite::BehaviorState ChangeToWander(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent{ nullptr };
	pBlackboard->GetData("Agent", pAgent);

	if (!pAgent) return Elite::BehaviorState::Failure;

	pAgent->SetToWander();
	return Elite::BehaviorState::Success;
}

inline Elite::BehaviorState ChangeToSeek(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent{ nullptr };
	pBlackboard->GetData("Agent", pAgent);

	if (!pAgent) return Elite::BehaviorState::Failure;

	Elite::Vector2 target = Elite::Vector2();

	pBlackboard->GetData("Target", target);

	pAgent->SetToSeek(target);
	return Elite::BehaviorState::Success;
}

// CONDITIONS
//***********

inline bool IsFoodCloseby(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent{ nullptr };
	pBlackboard->GetData("Agent", pAgent);

	std::vector<AgarioFood*>* pFoods{nullptr};
	pBlackboard->GetData("FoodVec", pFoods);


	//Optional: Check if data is ok
	if (pFoods->empty()) return false;

	float closestDistance{};
	AgarioFood* pClosestFood = (*pFoods)[0];
	closestDistance = Elite::DistanceSquared(pAgent->GetPosition(), pClosestFood->GetPosition());

	for (size_t i = 1; i < pFoods->size(); i++)
	{
		float distanceSquared{ Elite::DistanceSquared(pAgent->GetPosition(), (*pFoods)[i]->GetPosition()) };
		if ( distanceSquared < closestDistance)
		{
			pClosestFood = (*pFoods)[i];
			closestDistance = distanceSquared;
		}
	}

	const float range{ pAgent->GetRadius() + 20.f };
	if (closestDistance < range * range)
	{
		pBlackboard->ChangeData("Target", pClosestFood->GetPosition());
		return true;
	}

	return false;
}

inline bool IsBiggerAgentCloseby(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent{ nullptr };
	pBlackboard->GetData("Agent", pAgent);

	std::vector<AgarioAgent*>* pAgents{ nullptr };

	if (pAgents->empty()) return false;

	AgarioAgent* pClosestAgent{ (*pAgents)[0] };
	float closestDistance{ DistanceSquared(pAgent->GetPosition(), pClosestAgent->GetPosition()) };
	
	for (size_t i = 1; i < pAgents->size(); ++i)
	{
		float distanceSquared{ Elite::DistanceSquared(pAgent->GetPosition(), (*pAgents)[i]->GetPosition()) };
		if (distanceSquared < closestDistance)
		{
			closestDistance = distanceSquared;
			pClosestAgent = (*pAgents)[i];
		}
	}

	const float range{ pAgent->GetRadius() + 20.f };
	if (closestDistance < range * range && pClosestAgent->GetRadius() >= pAgent->GetRadius())
	{
		auto currentFleeTarget{ nullptr };
		pBlackboard->GetData("AgentFleeTarget", currentFleeTarget);
		if (currentFleeTarget != pClosestAgent)
		{
			pBlackboard->ChangeData("AgentFleeTarget", pClosestAgent);
			return true;
		}
		else return false;
	}

	return false;
}






#endif