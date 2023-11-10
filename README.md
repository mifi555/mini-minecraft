# mini-minecraft-stop-playin-and-unblock-me
mini-minecraft-stop-playin-and-unblock-me created by GitHub Classroom

**MILESTONE 1**

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
