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


## Sources

https://leifnode.com/2013/12/flow-field-pathfinding/  
https://www.aaai.org/Papers/AIIDE/2008/AIIDE08-031.pdf
https://www.reddit.com/r/gamedev/comments/jfg3gf/the_power_of_flow_field_pathfinding/
