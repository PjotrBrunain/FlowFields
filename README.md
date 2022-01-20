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
The agents making use of this way of navigating just have to querry the cell they're on about where the vector points to to get their movement direction.  

## Why use a FlowField

FlowFields are mostly used when there's a need to simulate a big crowd moving to certain points.  
This is because when there's a lot of agents it's faster to just calculate one grid then to have to run an algorithm \(for example A\*\) for each agent.  

## How does a FlowField work?

A FlowField is generated in 3 steps.

1. Start with a cost Field.  
![CostField](https://github.com/PjotrBrunain/FlowFields/blob/main/Images/CostField.png?raw=true)  
This is a field of values ranging from 0 to 255 (this can go higher if you want but i used a BYTE for my example).  
The higher the value is the higher the cost is to traverse it.  
This is depicted above by how green it is. The greener the square is the lower the cost value is.

2. With this cost Field we generate an integration Field.  
![IntegrationField](https://github.com/PjotrBrunain/FlowFields/blob/main/Images/IntegrationField.png?raw=true)  
(Values depicted here are how greener how higher the integration value)  
To generate this integration Field we first put all values to a very high value (I used the max value of size_t) and then we put the goal node's value to 0.  
After this we go from the starting node outwards and add the cost of each cell to the cost of the cell it came from replacing the value if a different calculated value is lower.  
This keeps going aslong as there are values changing.  
Once all values are correctly calculated we go on to the next step.  

3. Vector Field aka FlowField  
![FlowField](https://github.com/PjotrBrunain/FlowFields/blob/main/Images/FlowField.png?raw=true)  
After calculating the integration Field we go over all the cells and check which one of it's neighbours has the smallest integration value.  
We then point the vector towards that cell.  


After these 3 steps all agents that use this grid can then just check the cell they're in for the direction they need to go to.  
This takes away the cost of having to calculate the path for each and every agent which makes this perfect for directing a lot of agents towards a goal.  
Another advantage of this is being able to combine other steeringbehaviors with this. In my example i added a simple wander to the agents behavior as you can see below.
![FlowfieldInAction](https://github.com/PjotrBrunain/FlowFields/blob/main/Images/FlowFieldInAction.gif?raw=true)  

## Is it more performant then other pathfinding methods?

It depends on the usecase. If you have the need to steer a whole bunch of agents then it is more performant. If it is used to steer only 1 or 2 agents it won't be more performant.  
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
