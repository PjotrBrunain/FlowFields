#include "stdafx.h"
#include "FlowFieldSteeringBehaviors.h"

#include "projects/Movement/SteeringBehaviors/SteeringAgent.h"

SteeringOutput FollowFlowField::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};

	Elite::Vector2 velocity{};
	const auto node = m_pGridGraph->GetNodeAtWorldPos(pAgent->GetPosition());
	if (node)
	{
		velocity = node->GetDirection();
		velocity *= pAgent->GetMaxLinearSpeed();
	}
	steering.LinearVelocity = velocity;
	return steering;
}
