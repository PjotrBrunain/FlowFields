#include "stdafx.h"
#include "TheFlock.h"

#include "../SteeringAgent.h"
#include "../Steering/SteeringBehaviors.h"
#include "../CombinedSteering/CombinedSteeringBehaviors.h"

//Constructor & Destructor
Flock::Flock(
	int flockSize /*= 50*/,
	float worldSize /*= 100.f*/,
	SteeringAgent* pAgentToEvade /*= nullptr*/,
	bool trimWorld /*= false*/)

	: m_FlockSize{ flockSize }
	, m_TrimWorld{ trimWorld }
	, m_WorldSize{ worldSize }
	, m_NeighborhoodRadius{ 15 }
	, m_NrOfNeighbors{ 0 }
	, m_pAgentToEvade{ pAgentToEvade }
{
	m_Agents.resize(m_FlockSize);

	// initialize the flock and the memory pool
	m_Neighbors.resize(m_FlockSize - 1);

	m_pPartitionedSpace = new CellSpace(worldSize, worldSize, 200, 200, flockSize);
	m_pSeekBehavior = new Seek();
	m_pWanderBehavior = new Wander();
	m_pEvadeBehavior = new Evade();
	m_pCohesionBehavior = new Cohesion(this);
	m_pSeparationBehavior = new Separation(this);
	m_pVelMatchBehavior = new VelocityMatch(this);

	m_pBlendedSteering = new BlendedSteering({ BlendedSteering::WeightedBehavior{m_pCohesionBehavior, 1.f}, BlendedSteering::WeightedBehavior{m_pWanderBehavior, 1.f}, BlendedSteering::WeightedBehavior{m_pSeparationBehavior, 2.f}, BlendedSteering::WeightedBehavior{m_pSeekBehavior, 1.f}, BlendedSteering::WeightedBehavior{m_pVelMatchBehavior, 1.f} });
	m_pPrioritySteering = new PrioritySteering({ m_pEvadeBehavior, m_pBlendedSteering });
	
	for (int i = 0; i < m_FlockSize; i++)
	{
		m_Agents[i] = new SteeringAgent();
		m_Agents[i]->SetAutoOrient(true);
		m_Agents[i]->SetMaxLinearSpeed(55.f);
		m_Agents[i]->SetMaxAngularSpeed(25.f);
		m_Agents[i]->SetMass(1);
		m_Agents[i]->SetPosition({ Elite::randomFloat(0, m_WorldSize), Elite::randomFloat(0, m_WorldSize) });
		m_Agents[i]->SetSteeringBehavior(m_pPrioritySteering);
		m_pPartitionedSpace->AddAgent(m_Agents[i]);
	}
	std::for_each(m_Agents.begin(), m_Agents.end(), [this](const BaseAgent* agent) mutable {m_OldPositions.push_back(agent->GetPosition()); });
}

Flock::~Flock()
{
	//clean up any additional data
	SAFE_DELETE(m_pSeekBehavior);
	SAFE_DELETE(m_pWanderBehavior);
	SAFE_DELETE(m_pEvadeBehavior);
	SAFE_DELETE(m_pCohesionBehavior);
	SAFE_DELETE(m_pSeparationBehavior);
	SAFE_DELETE(m_pVelMatchBehavior);

	SAFE_DELETE(m_pBlendedSteering);
	SAFE_DELETE(m_pPrioritySteering);
	SAFE_DELETE(m_pPartitionedSpace);

	for (auto pAgent : m_Agents)
	{
		SAFE_DELETE(pAgent);
	}
	m_Agents.clear();
}

void Flock::Update(float deltaT)
{
	//update the flock
	// loop over all the agents
		// register its neighbors	(-> memory pool is filled with neighbors of the currently evaluated agent)
		// update it				(-> the behaviors can use the neighbors stored in the pool, next iteration they will be the next agent's neighbors)
		// trim it to the world
	for (size_t i = 0; i < m_Agents.size(); ++i)
	{
		m_pPartitionedSpace->UpdateAgentCell(m_Agents[i], m_OldPositions[i]);
	}
	m_OldPositions.clear();
	std::for_each(m_Agents.begin(), m_Agents.end(), [this](const BaseAgent* agent) mutable {m_OldPositions.push_back(agent->GetPosition()); });
	
	m_pEvadeBehavior->SetTarget(m_pAgentToEvade->GetPosition());
	for (size_t j = 0; j < m_Agents.size(); j++)
	{
		m_pPartitionedSpace->UpdateAgentCell(m_Agents[j], m_OldPositions[j]);
		
		if (m_UsingPartitioning) m_pPartitionedSpace->RegisterNeighbors(m_Agents[j], m_NeighborhoodRadius);
		else RegisterNeighbors(m_Agents[j]);
		if (j == 0 && m_CanRenderDebug)
		{
			auto& neighbors = m_Neighbors;
			int nrOfNeighbors{ m_NrOfNeighbors };
			if (m_UsingPartitioning)
			{
				nrOfNeighbors = m_pPartitionedSpace->GetNrOfNeighbors();
				neighbors = m_pPartitionedSpace->GetNeighbors();
			}
			for (int i = 0; i < nrOfNeighbors; ++i)
			{
				DEBUGRENDERER2D->DrawCircle(neighbors[i]->GetPosition(), 1.f, Elite::Color{ 0.f,1.f,0.f }, 0.4f);
			}
			DEBUGRENDERER2D->DrawCircle(m_Agents[0]->GetPosition(), m_NeighborhoodRadius, Elite::Color{ 0.f,1.f,0.f }, 0.4f);
		}
		m_Agents[j]->Update(deltaT);

		
		if (m_TrimWorld)
		{
			m_Agents[j]->TrimToWorld(Elite::Vector2(0, 0), Elite::Vector2(m_WorldSize, m_WorldSize));
		}
	}
}

void Flock::Render(float deltaT)
{
	//render the flock
	for (auto pAgent : m_Agents)
	{
		pAgent->Render(deltaT);
	}
	if (m_UsingPartitioning && m_CanRenderDebug)
	{
		m_pPartitionedSpace->RenderCells();
	}
}

void Flock::UpdateAndRenderUI()
{
	//Setup
	int menuWidth = 235;
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
	ImGui::Text("LMB: place target");
	ImGui::Text("RMB: move cam.");
	ImGui::Text("Scrollwheel: zoom cam.");
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("STATS");
	ImGui::Indent();
	ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Flocking");
	ImGui::Spacing();

	//Implement checkboxes for debug rendering and weight sliders here

	ImGui::Text("Behavior Weights");
	ImGui::Spacing();

	ImGui::Checkbox("RenderDebug", &m_CanRenderDebug);
	
	ImGui::SliderFloat("Cohesion", &m_pBlendedSteering->GetWeightedBehaviorsRef()[0].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Wander", &m_pBlendedSteering->GetWeightedBehaviorsRef()[1].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Separation", &m_pBlendedSteering->GetWeightedBehaviorsRef()[2].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Seek", &m_pBlendedSteering->GetWeightedBehaviorsRef()[3].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("VelMatch", &m_pBlendedSteering->GetWeightedBehaviorsRef()[4].weight, 0.f, 1.f, "%.2");

	ImGui::Checkbox("UseSpacePartitioning", &m_UsingPartitioning);

	//End
	ImGui::PopAllowKeyboardFocus();
	ImGui::End();

}

void Flock::RegisterNeighbors(SteeringAgent* pAgent)
{
	for (auto* pNeighbor : m_Neighbors)
	{
		pNeighbor = nullptr;
	}
	m_NrOfNeighbors = 0;
	for (size_t i = 0; i < m_Agents.size(); i++)
	{
		if (pAgent != m_Agents[i])
		{
			if (Elite::DistanceSquared(pAgent->GetPosition(), m_Agents[i]->GetPosition()) < m_NeighborhoodRadius * m_NeighborhoodRadius)
			{
				//replace_first(m_Neighbors.begin(), m_Neighbors.end(), nullptr, m_Agents[i]);
				m_Neighbors[m_NrOfNeighbors] = m_Agents[i];
				m_NrOfNeighbors++;
			}
		}
	}
}

Elite::Vector2 Flock::GetAverageNeighborPos() const
{
	if (m_UsingPartitioning)
	{
		int nrOfNeighbors = m_pPartitionedSpace->GetNrOfNeighbors();
		const auto& neighbors = m_pPartitionedSpace->GetNeighbors();
		Elite::Vector2 avgPos{};
		for (int i = 0; i < nrOfNeighbors; ++i)
		{
			avgPos += neighbors[i]->GetPosition();
		}
		return avgPos / float(m_NrOfNeighbors);
	}
	else
	{
		int nrOfNeighbors = m_NrOfNeighbors;
		const auto& neighbors = m_Neighbors;
		Elite::Vector2 avgPos{};
		for (int i = 0; i < nrOfNeighbors; ++i)
		{
			avgPos += neighbors[i]->GetPosition();
		}
		return avgPos / float(m_NrOfNeighbors);
	}
}

Elite::Vector2 Flock::GetAverageNeighborVelocity() const
{
	if (m_UsingPartitioning)
	{
		int nrOfNeighbors = m_pPartitionedSpace->GetNrOfNeighbors();
		const auto& neighbors = m_pPartitionedSpace->GetNeighbors();
		Elite::Vector2 avgVel{};
		for (int i = 0; i < nrOfNeighbors; ++i)
		{
			avgVel += neighbors[i]->GetLinearVelocity();
		}
		return avgVel / float(m_NrOfNeighbors);
	}
	else
	{
		int nrOfNeighbors = m_NrOfNeighbors;
		const auto& neighbors = m_Neighbors;
		Elite::Vector2 avgVel{};
		for (int i = 0; i < nrOfNeighbors; ++i)
		{
			avgVel += neighbors[i]->GetLinearVelocity();
		}
		return avgVel / float(m_NrOfNeighbors);
	}
}

void Flock::SetSeekTarget(TargetData target)
{
	//set target for Seek behavior

	m_pSeekBehavior->SetTarget(target);
}

float* Flock::GetWeight(ISteeringBehavior* pBehavior)
{
	if (m_pBlendedSteering)
	{
		auto& weightedBehaviors = m_pBlendedSteering->GetWeightedBehaviorsRef();
		auto it = find_if(weightedBehaviors.begin(),
			weightedBehaviors.end(),
			[pBehavior](BlendedSteering::WeightedBehavior el)
			{
				return el.pBehavior == pBehavior;
			}
		);

		if (it != weightedBehaviors.end())
			return &it->weight;
	}

	return nullptr;
}
