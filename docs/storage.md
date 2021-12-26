# Storage of a world 

A world has to be generated first then saved into memory as efficiently as possible.
There are two main things to think: 

- Cache locality: Blocks near from each other in the world should be near each other in memory
- Fast access: A player can move in all directions as far as he wants and may TP from one place to another.
- The world can't be fully loaded at any point

Thus, the datastructure containing the blocks should at least:

- Has cache locality: around player at least a n times n times n chunk of data should be map linearly in memory
- Be expansible in directions at runtime 
- cold loading from anywhere should not be expensive


## Solutions
- Chunk like in Minecraft
