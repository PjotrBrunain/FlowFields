//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
#include "../SteeringAgent.h"
#include "../Obstacle.h"
#include "framework\EliteMath\EMatrix2x3.h"

//SEEK
//****
SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	steering.LinearVelocity = m_Target.Position - pAgent->GetPosition(); //Desired Velocity
	steering.LinearVelocity.Normalize(); //Normalize Desired Velocity
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); //Rescale to Max Speed

	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0,1,0 }, 0.4f);

	return steering;
}

//FLEE
//****
SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	float distanceToTarget = Elite::Distance(pAgent->GetPosition(), m_Target.Position);
	if (distanceToTarget > m_FleeRadius)
	{
		return SteeringOutput(Elite::ZeroVector2, 0.f, false);
	}

	SteeringOutput steering = Seek::CalculateSteering(deltaT, pAgent);
	steering.LinearVelocity *= -1;

	return steering;
}

SteeringOutput Face::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};

	Elite::Vector2 desiredFace{ m_Target.Position - pAgent->GetPosition() };

	Elite::Vector2 currentFace{ pAgent->GetDirection() };
	
	float correctionAngle{ std::acos(Elite::Dot(currentFace, desiredFace) / (currentFace.Magnitude() * desiredFace.Magnitude())) };

	steering.AngularVelocity = correctionAngle;
	
	return steering;
}

SteeringOutput Wander::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	//if (m_Target.Position.Distance(pAgent->GetPosition()) < m_Radius)
	{
		Elite::Vector2 circlePos;
		circlePos.x = pAgent->GetPosition().x + cos(m_WanderAngle - Elite::ToRadians(90)) * m_OffsetDistance;
		circlePos.y = pAgent->GetPosition().y + sin(m_WanderAngle - Elite::ToRadians(90)) * m_OffsetDistance;
		float newAngle{ -(m_WanderAngle - Elite::ToRadians(90)) + Elite::randomFloat(-m_MaxAngleChange, m_MaxAngleChange) };
		m_WanderAngle = newAngle;
		m_Target.Position.x = circlePos.x + m_Radius * cos(newAngle);
		m_Target.Position.y = circlePos.y + m_Radius * sin(newAngle);
	}
	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawPoint(m_Target.Position, 10, { 0,0,1 }, 0.4f);
		/*DEBUGRENDERER2D->DrawCircle()*/
	}
	return Seek::CalculateSteering(deltaT, pAgent);
}

SteeringOutput Pursuit::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};

	Elite::Vector2 targetPoint{m_Target.Position};
	targetPoint += m_Target.LinearVelocity;

	steering.LinearVelocity = targetPoint - pAgent->GetPosition(); //Desired Velocity
	steering.LinearVelocity.Normalize(); //Normalize Desired Velocity
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); //Rescale to Max Speed

	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0,1,0 }, 0.4f);

	return steering;
}

SteeringOutput Evade::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	float distanceToTarget = Elite::Distance(pAgent->GetPosition(), m_Target.Position);
	if (distanceToTarget > m_EvadeRadius)
	{
		return SteeringOutput(Elite::ZeroVector2, 0.f, false);
	}
	
	SteeringOutput steering{ Pursuit::CalculateSteering(deltaT, pAgent) };
	steering *= -1;
	
	return steering;
}

SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	steering.LinearVelocity = m_Target.Position - pAgent->GetPosition(); //Desired Velocity
	steering.LinearVelocity.Normalize(); //Normalize Desired Velocity
	 //Rescale to Max Speed

	if (Elite::DistanceSquared(m_Target.Position, pAgent->GetPosition()) < m_SlowDownRadius * m_SlowDownRadius)
	{
		steering.LinearVelocity *= (pAgent->GetMaxLinearSpeed() * (1 - 1 / Elite::Distance(m_Target.Position, pAgent->GetPosition())));
	}
	else
	{
		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();
	}
	
	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0,1,0 }, 0.4f);
	
	return steering;
}

void Arrive::SetSlowRadius(float slowRadius)
{
	m_SlowDownRadius = slowRadius;
}

float Arrive::GetSlowRadius() const
{
	return m_SlowDownRadius;
}

void Arrive::SetTargetRadius(float targetRadius)
{
}
