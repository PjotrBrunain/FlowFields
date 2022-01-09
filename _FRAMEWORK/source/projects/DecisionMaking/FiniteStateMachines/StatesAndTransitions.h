/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// StatesAndTransitions.h: Implementation of the state/transition classes
/*=============================================================================*/
#ifndef ELITE_APPLICATION_FSM_STATES_TRANSITIONS
#define ELITE_APPLICATION_FSM_STATES_TRANSITIONS

#include "projects/Shared/Agario/AgarioAgent.h"
#include "projects/Shared/Agario/AgarioFood.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include "framework/EliteAI/EliteData/EBlackboard.h"

//------------
//---STATES---
//------------

//A state that makes the agent wander
class WanderState : public Elite::FSMState
{
public:
	void OnEnter(Elite::Blackboard* pBlackboard) override
	{
		AgarioAgent* pAgent{ nullptr };
		bool success = pBlackboard->GetData("Agent", pAgent);
		if (!success) return;

		pAgent->SetToWander();
	}
};

class SeekFoodState : public Elite::FSMState
{
public:
	void OnEnter(Elite::Blackboard* pBlackboard) override
	{
		AgarioAgent* pAgent{ nullptr };
		bool success = pBlackboard->GetData("Agent", pAgent);
		if (!success) return;

		AgarioFood* pFoodTarget{ nullptr };
		success = pBlackboard->GetData("FoodTarget", pFoodTarget);
		if (!success) return;

		if(pFoodTarget != nullptr)
			pAgent->SetToSeek(pFoodTarget->GetPosition());
	}

	void Update(Elite::Blackboard* pBlackboard, float deltaTime) override
	{
		std::vector<AgarioFood*>* pFoodVector{ nullptr };
		bool success = pBlackboard->GetData("FoodVec", pFoodVector);
		if (!success) return;

		AgarioAgent* pAgent{ nullptr };
		success = pBlackboard->GetData("Agent", pAgent);
		if (!success) return;

		auto food = std::min_element(pFoodVector->begin(), pFoodVector->end(), [pAgent](const AgarioFood* food, const AgarioFood* food2) { return Elite::Distance(pAgent->GetPosition(), food->GetPosition()) < Elite::Distance(pAgent->GetPosition(), food2->GetPosition()); });

		pBlackboard->ChangeData("FoodTarget", *(food._Unwrapped()));
		pAgent->SetToSeek((*food)->GetPosition());
	}
};

//-----------------
//---TRANSITIONS---
//-----------------



#endif