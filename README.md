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

## Sources

https://leifnode.com/2013/12/flow-field-pathfinding/  
https://www.aaai.org/Papers/AIIDE/2008/AIIDE08-031.pdf
https://www.reddit.com/r/gamedev/comments/jfg3gf/the_power_of_flow_field_pathfinding/
