# mini-minecraft-stop-playin-and-unblock-me
mini-minecraft-stop-playin-and-unblock-me created by GitHub Classroom

## MILESTONE 1

Video recording of Milestone features:

https://drive.google.com/file/d/1LEl1vRFtn9Xfg04fpZy6D9NtYGLuvNhW/view?usp=sharing

Timestamps

0:00 - 0:57: Efficient Terrain Rendering and Chunking

0:58 - 2:17: Procedural Terrain

2:18 - 3:26: Game Engine Tick Function and Player Physics


**Milan: Game Engine Tick Function and Player Physics**

_Tick function_
- Invoked Player::tick in MyGL::tick and calculated delta-time using QDateTime::currentMSecsSinceEpoch().

_Input processing_
- implemented input processing for ground mode and flight mode
- altered the functionality of keyPressEvent so that an InputBundle is constructed and passed to the Player's processInputs function, where the state of the keyboard is read and used to set the Player's physics attributes 
- implemented reading of mouse movement to rotate the camera by computing changes in mouse’s X and Y position
- difficulties with mouse movement implementation between macOS and Windows
- added keyReleaseEvent to reflect physics attributes once keys are released

_Player Physics_
- implemented physics computation function, including features like drag, velocity, acceleration, and gravity
- created a helper function that checks if a player is on the ground (useful for things like determining jumping) by calculating the player position by flooring the x and z coordinates of player’s BoundingBoxMin and offsetting them by the loop indices x and z. The y-coordinate is also offset downwards slightly (-0.005f) to check just below the player and for each calculated position, the function checks if the corresponding terrain block is not empty (!= EMPTY) using terrain.getBlockAt(position). If a non-empty block is found, it indicates that the player is touching the ground (isGrounded = true).
- implemented a collision function that uses raycasting and grid marching: treat player as blocks of 1x2x1 and compute minimum point of player bounding box and then cast out rays from player vertices to detect collisions with the terrain
- difficulties with getting stuck on blocks after dropping to the ground from flight mode or jumping on blocks

_Block Interaction_
- Remove blocks with left-click (within 3 units of the camera) by casting out a ray using grid march and setting the selected block as an EMPTY block
- Place blocks with right-click adjacent to the targeted block face: by casting out a ray and using grid march, check on which axes the block lies on and calculate position adjacent to the face of the block hit to place a block
- difficulties with placing blocks on some points in the terrain

**Joseph - Procedural Terrain**

_Grassland and Mountain Biome Noise Functions_
- I began by testing different implementations of FBM and Perlin Noise in the instanced vertex shader. 
- From there I created functions for both the grassland and mountain biomes and requested feedback from my team, Professor Adam, and Jackie before moving on to the interpolation step.

_Biome Interpolation Noise Function_
- To interpolate between the biomes, I created a Perlin Noise based t-value equal to 0.5 * (perlinNoise(xz / 512.f) + 1.f), used smoothstep(0.55, 0.45, t) on it, and then mixed the values with (grassH, mountainsH, t) respectively to determine the final height value. 
- This resulted in mountains being taller and in a more concentrated area rather than scattered throughout the map.

_C++ Implementation of Noise Functions and Biome Block Layering_
- After translating the GLSL code from the vertex shader to C++ in terrain.cpp, I added a SNOW block type and assigned the proper block types and colors to their specified biomes and heights in generateChunkTerrain().

_Difficulties_
- The most challenging portion of this assignment was creating noise functions that looked natural and aesthetically appealing while remaining in the bounds [0, 255] throughout their oscillation cycle. 
- I had to scrap noise functions that didn't translate well to our overall implementation and continuously test and augment each function prior to our final submission.

**Michael: Efficient Terrain Rendering and Chunking**

_Efficient Rendering of Chunks_

- Instanced rendering of the `Cube` Drawable was replaced with a more efficient rendering of Chunks, which now inherits from `Drawable` as well
- One chunk is considered one “mesh” and only visible block faces are drawn (i.e. block faces that neighbor `EMPTY` blocks). This is implemented in `Chunk::createVBOdata()` function override
- `chunkconstants.h` was created to keep `Chunk::createVBOData` code concise and easier to read, and holds various hardcoded data such as face positions, normals and colors. 

_Interleaved VBO for vertex data_

- `Chunk::buildInterleavedVBOFromData` takes a vector of floats (the interleaved data) and a vector of ints (indices) and generates two VBO’s. The interleaved VBO that contains the vertex data holds its data in the following form: `{ pos0 (4 floats), nor0 (4 floats), col0 (4 floats),  … }` for each vertex. `ShaderProgram::drawInterleaved` was implemented to handle interleaved data. 

_New Chunk Generation Based on player proximity_

- `Terrain::generateChunksInProximity` was implemented to build new chunks given a chunk position at (x, z). If the given chunk has “null” neighboring chunks, they are replaced with instantiated chunks. Additionally, `Terrain::instantiateChunk` will automatically generate biome terrain for the new chunk. At every `tick()`, we check the player’s current “chunk” position, and fill in new chunks for its neighbors if needed. 

_Difficulties + Potential improvements_

- Currently, chunks don’t consider their neighboring chunks when drawing, and instead automatically treats blocks beyond their own boundaries as EMPTY blocks. This renders block faces that aren’t visible by the player. This could be further optimized by checking for blocks in neighboring chunks, to determine whether faces that are on the very edge of a chunk should be drawn or not. 
- I had some difficulty debugging `out_of_range` exceptions, as Qt Creator doesn’t catch `.at()` exceptions by default for vectors. I found that you can actually set breakpoints for this in the debugger. 

