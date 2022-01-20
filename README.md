# FlowFields

## Goal

Getting a good understanding of FlowFields:  

- How do they work?
- What are they used for?
- Is it more performant then other pathfinding methods  
	- If yes: In what cases is it better?
	- If no: Why is it not better?
	
Make an example project to implement a FlowField.

## What is a FlowField

A FlowField is a grid of vectors that each point towards their neighbour closest to the goal.  
The agents making use of this way of navigating just have to query the cell they're on about where the vector points to to get their movement direction.  

## Why use a FlowField

FlowFields are mostly used when there's a need to simulate a big crowd moving to certain points.  
This is because when there's a lot of agents it's faster to just calculate one grid then to have to run an algorithm \(for example A\*\) for each agent.  

## How does a FlowField work?

A FlowField is generated in 3 steps.

1. Start with a Cost Field.  
This is a field of values ranging from 0 to 255 (more values are possible, but here a BYTE was used).
The higher the value is, the higher the cost is to traverse it.  
This field is pre-generated depending on the world you're trying to navigate.  
Depicted below is a Cost Field which displays the cost level by how green it is. The greener the square is, the lower the cost value.
The grey squares would represent obstacles.  
![CostField](https://github.com/PjotrBrunain/FlowFields/blob/main/Images/CostField.png?raw=true)  

2. With this Cost Field we generate an Integration Field.  
These integration values are used in the last step to determine the direction of the vector. The integration values are the combined cost values to get from point A to point B.  
To generate this Integration Field we first put all values to a very high value (I used the max value of size_t).
Next we calculate each value starting by putting the goal node's value to 0, then starting from the goal node outwards add the cost of each cell to the integration value of the cell it came from, replacing the integration value if the newly calculated value is lower.
This keeps going as long as there are values changing.  
Once all values are correctly calculated we go on to the next step.  
Below is some pseudocode for the basic algorithm I wrote.  

		void CalculateIntegrationField
		{
			for (auto node : AllNodes)
			{
				set node.IntegrationValue to max;
			}
			
			List openList;
			Add goal node to openList;
			
			while openList.size > 0
			{
				set currentNode to first of openList;
				remove currentNode from openList;
				for (auto neigbour : currentNode.Neighbours)
				{
					newIntegrationValue = CalculateIntegrationValue;
					if (newIntegrationValue < neighbour.IntegrationValue)
					{
						Add neighbour to openList if not already in there;
						Set neighbour IntegrationValue to newIntegrationValue;
					}
				}
			}
		}
		
(Values depicted here are the greener the cell, the higher the integration value)  
![IntegrationField](https://github.com/PjotrBrunain/FlowFields/blob/main/Images/IntegrationField.png?raw=true)  

3. Vector Field aka FlowField  
After calculating the integration Field we go over all the cells and check which one of its neighbours has the smallest integration value.  
We then point the vector towards that cell.  
![FlowField](https://github.com/PjotrBrunain/FlowFields/blob/main/Images/FlowField.png?raw=true)  


After these 3 steps all agents that use this grid can then just check the cell they're in for the direction they need to go to.  
This takes away the cost of having to calculate the path for each and every agent which makes this perfect for directing a lot of agents towards a goal.  
Another advantage of this is being able to combine other steeringbehaviors with this. In my example i added a simple wander combined with the flowfield direction to make for a somewhat drunken move towards the goal.
![FlowfieldInAction](https://github.com/PjotrBrunain/FlowFields/blob/main/Images/FlowFieldInAction.gif?raw=true)  

## Is it more performant than other pathfinding methods?

It depends on the usecase. If you have the need to steer a whole bunch of agents then it will perform better. If it is used to steer only 1 or 2 agents it is less perfomant.  
So this is ideal to use when you have a lot of agents to steer around in for example war games where you need to move large armies to certain points.  
The performance of the field also directly correlates to how many nodes you have. More nodes means higher calculation costs.  

These are some crude measurements i did:
- 3000 Agents, 25x25 field: ~40 fps  
![3000Agents25x25](https://github.com/PjotrBrunain/FlowFields/blob/main/Images/3000Agents25x25.gif?raw=true)
- 2000 Agents and below, 25x25 field: Easy 60 fps  
![2000Agents25x25](https://github.com/PjotrBrunain/FlowFields/blob/main/Images/2000Agents25x25.gif?raw=true)
- 1000 Agents and below, 100x100 field: Easy 60 fps  
![1000Agents100x100](https://github.com/PjotrBrunain/FlowFields/blob/main/Images/1000Agents100x100.gif?raw=true)
- 2000 Agents, 100x100 field: ~50 fps  
![2000Agents100x100](https://github.com/PjotrBrunain/FlowFields/blob/main/Images/2000Agents100x100.gif?raw=true)

Keep in mind these measurements are dependent on the physics calculations of the framework i use aswell.
## Sources

https://leifnode.com/2013/12/flow-field-pathfinding/  
https://www.aaai.org/Papers/AIIDE/2008/AIIDE08-031.pdf  
https://www.reddit.com/r/gamedev/comments/jfg3gf/the_power_of_flow_field_pathfinding/
