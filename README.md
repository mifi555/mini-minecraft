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


## MILESTONE 2

Video Link: https://drive.google.com/file/d/1bObKZHQHKJqAiQMiiI04ZsGLLDmFoHAa/view?usp=sharing

Timestamps

0:00 - 1:18: Cave Systems and Post-Processing

1:19 - 2:16: Multithreaded Terrain Generation

2:17 - 3:00: Texturing and Texture Animation

Note: For Joseph's lava fragment shader, the video couldn't accurately capture the red color and it made the effect look orange instead. I've included an image here that more accurately demonstrates the color:

https://drive.google.com/file/d/1eItzAM7qhDRF9_k3sVYuG4ndIPiyMDqm/view?usp=drive_link

**Joseph: Cave Systems**

_3D Noise Functions_

- Implemented 3D Perlin Noise and Surflet functions with component scaling for cave generation.

_Underground Cave System_

- Utilized 3D Perlin Noise to shape and implement an underground cave system with interconnected caves, lava at the lowest depths, and unbreakable bedrock as the base level for the world.
- Initially had some difficulties with noise uniformity and the caves not looking natural, was able to fix the issue by scaling the Perlin Noise inputs.

_Frame Buffer Rendering_

- Set up frame buffer class and created helper functions to bind the frame buffer, edit the viewport for rendering on the entire frame buffer, and clear the screen.

_PostProcessShader and Quad Classes_

- Added PostProcessShader and Quad classes for use with post-process rendering, including various functions and handles for managing the data passed to the vertex and fragment shaders.

_Post-Process Render Pass_

- Created a helper function to perform a post-process render pass that displays a blue or red tinge whenever the player is in water or lava respectively.
- Had some trouble throughout the post-process rendering portion of the assignment due to all the files involved, but Adam's lecture and the logic in HW4 really helped me wrap my head around everything.

_Water and Lava Fragment Shaders_

- Created water and lava post-process fragment shaders that display a tinge of blue and red respectively. These are currently placeholder shaders and I will be replacing them for the Post-process Camera Overlay task in Milestone 3.

_PassThrough Vertex Shader_

- Added a vertex shader to pass vertex data to the water and lava fragment shaders without altering the data.

_Player Swimming Movement_

- Added members to the player class to indicate when the player is in water or lava and adjusted various movement functions to check what block the player is currently standing in.
- Changed the acceleration and gravity values so that they are slowed when the player is in water or lava, reducing the player's movement and fall speeds in order to simulate swimming.

_Improved Terrain_

- Improved upon the previous terrain by reducing the amount of water, making the islands more visually appealing, adding a layer of sand below the water and layering the water on top of it, and adding lava streams to the mountains.

**Michael: Multi-Threaded Terrain Generation**

Multi-threading is used to procedurally generate the terrain around the player. The radius can be changed in `Terrain.cpp` using `TERRAIN_DRAW_RADIUS` and `TERRAIN_CREATE_RADIUS`. By default it's 1 (3x3 DRAW radius) and 2 (5x5 CREATE radius) respectively.

At every `MyGL::tick()`, `Terrain::multithreadedWork()` runs to check if the player has entered into a new terrain zone. This function runs `Terrain::tryExpansion()` and `Terrain::checkThreadResults()`

In `Terrain::tryExpansion()`:

* determine the player's current and previous position
* compare those positions, if the player is in the same zone they were in previous position, return early
* determine the `TerrainConstants::Direction` the player went, to optimize which terrain zones we must consider for deletion / expansion
* delete zones that are outside the player's radius (in the opposite direction of the player)
* for new zones that will enter the player's radius (in the direction of the player), we either:
    1. spawn `BlockTypeWorker`'s to generate Terrain zones the player has not seen yet
    2. spawn `VBOWorker`'s to generate the VBO data of terrain zones we already have block data for


In `Terrain::checkThreadResults()`:

* spawn `VBOWorker`'s for new chunks with block data
* for complete `VBOWorker` threads, we create new vertex buffers to be sent to the GPU, which is done in the main thread

**Milan: Texturing and texture animation in OpenGL**

Created a way to load images as textures into OpenGL by using the Texture class base code from HW04/05

Associated the texture with a sampler2D in a shader and used a .qrc file to let Qt know that it should include the texture files in its working directory

Created an addition to lambert.frag.glsl that makes use of a sampler2D to apply texture colors to a surface

Split VBO Chunk Data to Transparent and Opaque VBO data in Chunk’s createMultiThreaded(), modified createVBOBuffer(), and drew interleaved transparent and opaque blocks in drawTerrainZone() of the Terrain class using the shader program.

Altered lambert.frag.glsl so that it includes a time variable to animate the UVs on a LAVA block and WATER block. The animateable block flag is the z value of the UV vec.

Using the BlockType of a given block, set the UV coordinates of a square face in the Chunk VBO so that they correspond to the appropriate texture square by using an unordered map of block type and UV coordinates 

Enabled alpha blending in MyGL::initializeGL() so that transparency can be applied to WATER blocks.

_DIFFICULTIES_: loading in textures and splitting VBO data into transparent and opaque blocks.

## MILESTONE 3

Video link: https://drive.google.com/file/d/1pWWu8ayry6nIkigPQSAintwixpzDHSeD/view

Timestamps: 

0:00 - 0:58: Additional Biomes, Post-process Camera Overlay, Waves, Procedural Assets

0:59 - 1:57: Procedural Sky, Fog

1:58 - 2:53: Super Mario 64 Implementation

2:54 - 3:09: Credits

**Joseph:**

_Additional Biomes_

- Created noise functions for the Summit Grove Plateaus and Mushroom Vale biomes.

- Added an additional t-value generator for the new biomes and a humidity noise function to help differentiate between the pairs of biomes at each (x, z) location.

- Implemented smoothed biome blending and interpolation between all four biomes.

- Defined the block structure and layering for the new biomes.

_Post-process Camera Overlay_

- Implemented water and lava post-process camera overlay with custom shaders that emulate water distortion effects via UV coordinate warping. Warping utilizes WorleyNoise and occurs as a function of time.

_Water and Lava Waves_

- In the Lambert vertex shader, applied a sin function that incorporates Worley noise and time to the x and y values of water and lava blocks to create water and lava waves.

_Procedural Assets_

- Added various block types used for the new biomes, caves, and procedural assets. Assigned textures to each block type.

- Created functions for defining procedural asset placement probabilities, placing assets depending on block type, and creating primitives including different types of trees and mushrooms. Assets include grass, flowers, mushrooms, trees, and bushes.

- Defined custom block colors based on the existing texture palette for use with the procedural assets.

_CHALLENGES_

The most challenging portion of this milestone was getting the four biomes to smoothly interpolate. This definitely took the most time to get right out of all of my tasks. I also feel like I hit a wall when I was working with the noise functions, which made it difficult to find patterns that I found appealing to use for the new biomes.

**Michael:**

**Milan: Procedural Sky/Day Night Cycle, Fog**

_SKY_
- Added a new Sky shader (sky.frag, sky.vert) and created a procedural sky background using the raycast method. Sky is drawn using a quad in MyGL (also changed quad array pos z value from 0.99f to 1.f in quad class) to fix clipping. Noise functions are used to generate the clouds.

- Modified the ShaderProgram by adding new uniform variables (int unifDimensions, int unifEye)

- Modified the position of the sun in the sky over time by rotating the sun on its x axis, changing the sky's color as the sun moves around the terrain.

- Passed in m_time incremented by dt in MyGL to the Sky shader in order to represent real time. Adjusted game time in sky.frag to create a 24 hour day/night cycle. Hard coded time intervals with unique sky colors (sunrise, noon, sunset, dusk, night) and interpolated between them as time advanced. Added stars that are generated using Worley noise that appear at night time.

- Used the light direction and color in the lambert shader to match terrain color with the sky's sun and color based on the sun’s rotation and time.

_FOG_
- Created a fog around the player position by adding a handle to represent player position in the Shaderprogam and interpolated block color with fog color determined by the distance of the object from the player, creating a fog effect that intensifies with distance.
- Fog can be disabled: uncomment NO_FOG and comment out FOG in lambert.frag


_DIFFICULTIES_: synchronizing day/night cycle with the sun's rotation. Fixed by passing real change in time into the sky shader.


