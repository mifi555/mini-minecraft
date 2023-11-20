#include "player.h"
#include <QString>
#include <cmath>

#include <glm/gtx/euler_angles.hpp>
#include <iostream>
#include <ostream>


//const float MAX_PITCH = 90.f;
//const float MIN_PITCH = -90.f;

Player::Player(glm::vec3 pos, const Terrain &terrain)
    : Entity(pos), m_velocity(0,0,0), m_acceleration(0,0,0),
      m_camera(pos + glm::vec3(0, 1.5f, 0)), mcr_terrain(terrain),
    m_hasJumped(false), m_interfaceAxis(-1),
    mcr_camera(m_camera), m_flightMode(true),
    m_water(false), m_lava(false)
{

}

Player::~Player()
{}

void Player::tick(float dT, InputBundle &input) {
    processInputs(mcr_terrain, input);
    computePhysics(dT, mcr_terrain, input);
}

void Player::processInputs(const Terrain &terrain, InputBundle &inputs) {
    //glm::mat3 cameraRotation = glm::mat3(glm::orientate3(glm::vec3(glm::radians(inputs.mouseY), 0, glm::radians(inputs.mouseX))));
    //glm::mat3 groundRotation = glm::mat3(glm::eulerAngleY(glm::radians(inputs.mouseX)));
    //m_camera.setRotation(cameraRotation);

    constexpr static float bump = 4.f;

    //distinguish between flight and ground mode
    float acceleration_scalar = bump * 90.0f;


    // Player slow down when swimming.
    if (m_water == true || m_lava == true) {
        acceleration_scalar *= float(2.0f/3.0f);
    }

    m_acceleration = glm::vec3(0,0,0);
    //FLIGHT MODE ON: player begins in flight mode
    if(m_flightMode){
        //W -> Accelerate positively along forward vector
        if (inputs.wPressed){
            m_acceleration += 2 * acceleration_scalar * this->m_forward;
        }
        //S -> Accelerate negatively along forward vector
        else if (inputs.sPressed){
            m_acceleration -= 2 * acceleration_scalar * this->m_forward;
        }
        //D -> Accelerate positively along right vector
        else if (inputs.dPressed){
            m_acceleration += 2 * acceleration_scalar * this->m_right;
        }
        //A -> Accelerate negatively along right vector
        else if (inputs.aPressed){
            m_acceleration -= 2 * acceleration_scalar * this->m_right;
        }
        //E -> Accelerate positively along up vector
        else if (inputs.ePressed){
            m_acceleration += 2 * acceleration_scalar * this->m_up;
        }
        //Q -> Accelerate negatively along up vector
        else if (inputs.qPressed){
            m_acceleration -= 2 * acceleration_scalar * this->m_up;
        }
    }
    //When flight mode is not active, the player is subject to gravity and terrain collisions (described later).
    //Additionally, the player's movement changes slightly:

    //else, flight mode is NOT active
    else {
        //calculate if player is on ground
        playerOnGround(terrain, inputs);

        //W -> Accelerate positively along forward vector, discarding Y component and re-normalizing
        if (inputs.wPressed){
            m_acceleration += acceleration_scalar * glm::normalize(glm::vec3(m_forward.x, 0, m_forward.z));
        }
        //S -> Accelerate negatively along forward vector, discarding Y component and re-normalizing
        else if (inputs.sPressed){
            m_acceleration -= acceleration_scalar * glm::normalize(glm::vec3(m_forward.x, 0, m_forward.z));
        }
        //D -> Accelerate positively along right vector, discarding Y component and re-normalizing
        else if (inputs.dPressed){
            m_acceleration += acceleration_scalar * glm::normalize(glm::vec3(m_right.x, 0, m_right.z));
        }
        //A -> Accelerate negatively along right vector, discarding Y component and re-normalizing
        else if (inputs.aPressed){
            m_acceleration -= acceleration_scalar * glm::normalize(glm::vec3(m_right.x, 0, m_right.z));

        }
        //Spacebar -> Add a vertical component to the player's velocity to make them jump
        if (inputs.spacePressed && !m_hasJumped){
            float jumpStrength = 50.0f; // Adjust this value as needed for the desired jump strength
            m_velocity.y += jumpStrength;
            m_hasJumped = true; // Player has jumped, prevent further jumps until reset
        }
    }
}

void Player::toggleFlightMode(){
    m_flightMode = !m_flightMode;
}

bool Player::playerOnGround(const Terrain &terrain, InputBundle &input) {
    glm::vec3 playerBoundingBoxMin = this->m_position - glm::vec3(0.5f, 0.f, 0.5f);
    bool isGrounded = false;
    for (int x = 0; x <= 1; x++) {
        for (int z = 0; z >= -1; z--) {
            glm::vec3 position = glm::vec3(floor(playerBoundingBoxMin.x) + x, floor(playerBoundingBoxMin.y - 0.005f), floor(playerBoundingBoxMin.z) + z);
            //if not empty block -> touching ground
            if (terrain.getBlockAt(position) != EMPTY &&
                terrain.getBlockAt(position) != WATER &&
                terrain.getBlockAt(position) != LAVA &&
                position[1] >= 25) {
                // Ground is detected
                isGrounded = true;

                // Update player swimming indicators.
                m_water = false;
                m_lava = false;

                if (terrain.getBlockAt(position) == BEDROCK) {
                    m_lava = true;
                } else if (terrain.getBlockAt(position) == SAND) {
                    m_water = true;
                }

                break;
            } else if (terrain.getBlockAt(position) == WATER) {
                m_water = true;
            } else if (terrain.getBlockAt(position) == LAVA || position[1] < 25) {
                m_lava = true;
            }
        }
        if (isGrounded) break; // If we've found ground, no need to check further
    }
    input.ground = isGrounded;
    if (isGrounded) {
        m_hasJumped = false; // Allow jumping again when the player is back on the ground
    }
    return isGrounded;
}


void Player::computePhysics(float dT, const Terrain &terrain, InputBundle &input) {
    // In both movement modes, the player's velocity is reduced to less than 100%
    // of its current value every frame (simulates friction + drag) before acceleration is added to it.
    m_velocity *= 0.8f;
    m_velocity += m_acceleration * dT;
    glm::vec3 rayDir = m_velocity * dT;

    glm::vec3 g = glm::vec3(0.0f, (-9.8f * 20.f), 0.f);

    // Player falls more slowly when in liquid.
    if (m_water == true || m_lava == true) {
        g *= float(2.0f/3.0f);
    }

    // Rather than directly changing camera position based on WASD, make the keys alter acceleration or velocity
    // Position += Velocity * dT

    // check if player is in ground or in the air
    if (!m_flightMode) {
        //player is falling
        if(!input.ground) {
            m_acceleration = g;
            m_velocity += m_acceleration * dT;
        }
        else if (input.ground && !input.spacePressed) {
            m_velocity.y = 0.0f;
    }
        // perform collision detection
        // Cast a ray from each cube vertex in the Player’s movement direction, length = speed
        rayDir = m_velocity * dT;
        collision(terrain, &rayDir);
    }
    //move camera and player along ray direction vector
    moveAlongVector(rayDir);
}

void Player::collision(const Terrain &terrain, glm::vec3 *rayDir){
    //compute player bounding box
    //treat player pos as being center of a cube
    //offset 0.5f in the x and z directions
    glm::vec3 boundingBoxMin = this->m_position - glm::vec3(0.5f, 0.0f, 0.5f);

    //coordinates of block that was hit
    glm::ivec3 out_blockHit = glm::ivec3();
    //distance to block that was hit
    float out_dist = 0.0f;

    bool collision = false;

    //loop over bounding box dimensions to check if player's next movement results in a collision
    //dimensions: 1 in x, 2 in y, 1 in z
    //treat player as a 1x2x1 block
    for(int x = 0; x <= 1; x++){
        for(int z = 1; z >= 0 ; z--){
            for(int y = 0; y <= 2; y++){
                glm::vec3 rayOrigin = boundingBoxMin + glm::vec3(x, y, z);
                //GRID MARCH: shoot out ray and check for intersection
                if(gridMarch(rayOrigin, *rayDir, terrain, &out_dist, &out_blockHit)){
                    //calculate minimum distance of distance:
                    //distance to the hit block (outDist)
                    //distance from player position to the hit block

                    // If any of these rays intersect with a non-empty block in the terrain, it updates the *rayDir with the minimum distance to the block
                    float distance = glm::min(out_dist - 0.005f, glm::length(this->m_position - glm::vec3(out_blockHit)));
                    //update ray's direction:
                    *rayDir = distance * glm::normalize(*rayDir);
                    collision = true;
                }
            }
        }
    }
}

bool Player::gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection, const Terrain &terrain, float *out_dist, glm::ivec3 *out_blockHit){
    float maxLen = glm::length(rayDirection); // Farthest we search
    glm::ivec3 currCell = glm::ivec3(glm::floor(rayOrigin));
    rayDirection = glm::normalize(rayDirection); // Now all t values represent world dist.

    float curr_t = 0.f;
    while(curr_t < maxLen) {
        float min_t = glm::sqrt(3.f);
        float interfaceAxis = -1; // Track axis for which t is smallest
        for(int i = 0; i < 3; ++i) { // Iterate over the three axes
            if(rayDirection[i] != 0) { // Is ray parallel to axis i?
                float offset = glm::max(0.f, glm::sign(rayDirection[i])); // See slide 5
                // If the player is *exactly* on an interface then
                // they'll never move if they're looking in a negative direction
                if(currCell[i] == rayOrigin[i] && offset == 0.f) {
                    offset = -1.f;
                }
                int nextIntercept = currCell[i] + offset;
                float axis_t = (nextIntercept - rayOrigin[i]) / rayDirection[i];
                axis_t = glm::min(axis_t, maxLen); // Clamp to max len to avoid super out of bounds errors
                if(axis_t < min_t) {
                    min_t = axis_t;
                    interfaceAxis = i;
                }
            }
        }
        if(interfaceAxis == -1) {
            throw std::out_of_range("interfaceAxis was -1 after the for loop in gridMarch!");
        }
        //store interface exis for block placement
        m_interfaceAxis = interfaceAxis;

        curr_t += min_t; // min_t is declared in slide 7 algorithm
        rayOrigin += rayDirection * min_t;
        glm::ivec3 offset = glm::ivec3(0,0,0);
        // Sets it to 0 if sign is +, -1 if sign is -
        offset[interfaceAxis] = glm::min(0.f, glm::sign(rayDirection[interfaceAxis]));
        currCell = glm::ivec3(glm::floor(rayOrigin)) + offset;
        // If currCell contains something other than EMPTY, return
        // curr_t
        BlockType cellType = terrain.getBlockAt(currCell.x, currCell.y, currCell.z);
        if(cellType != BlockType::EMPTY &&
            cellType != BlockType::WATER &&
            cellType != BlockType::LAVA) {
            *out_blockHit = currCell;
            *out_dist = glm::min(maxLen, curr_t);

            // Update player swimming indicators.
            m_water = false;
            m_lava = false;

            return true;
        } else if (cellType == BlockType::WATER) {
            m_water = true;
        } else if (cellType == BlockType::LAVA) {
            m_lava = true;
        }
    }
    *out_dist = glm::min(maxLen, curr_t);
    return false;
}


void Player::removeBlock(Terrain *terrain) {
    glm::vec3 rayOrigin = m_camera.mcr_position;
    //remove block within 3 units away from player
    glm::vec3 rayDirection = 3.0f * glm::normalize(this->m_forward);
    float out_dist = 0.0f;
    glm::ivec3 out_blockHit = glm::ivec3();

    if (gridMarch(rayOrigin, rayDirection, *terrain, &out_dist, &out_blockHit)) {
        // If block is BEDROCK it is unbreakable.
        if (terrain->getBlockAt(out_blockHit.x, out_blockHit.y, out_blockHit.z) == BEDROCK) {
            return;
        } else {
            terrain->setBlockAt(out_blockHit.x, out_blockHit.y, out_blockHit.z, EMPTY);
            //->getChunkAt(out_blockHit.x, out_blockHit.z).get()->createVBOdata();
        }
    }
}

void Player::placeBlock(Terrain *terrain, BlockType blockType) {
    glm::vec3 rayOrigin = m_camera.mcr_position;
    glm::vec3 rayDirection = 3.0f * glm::normalize(this->m_forward);
    float out_dist = 0.f;
    glm::ivec3 out_blockHit = glm::ivec3();

    if (gridMarch(rayOrigin, rayDirection, *terrain, &out_dist, &out_blockHit)) {
        //check on which axes the block lies on and calculate position adjacent to the face of the block hit
        if (m_interfaceAxis == 0) {
            //check if position is empty
            if (terrain->getBlockAt(out_blockHit.x, out_blockHit.y, out_blockHit.z + glm::sign(rayDirection.z)) == EMPTY) {
                //place block at that position
                terrain->setBlockAt(out_blockHit.x, out_blockHit.y, out_blockHit.z + glm::sign(rayDirection.z), blockType);
                //terrain->getChunkAt(out_blockHit.x, out_blockHit.z).get()->createVBOdata();
            }
        } else if (m_interfaceAxis == 1) {
            if (terrain->getBlockAt(out_blockHit.x, out_blockHit.y + glm::sign(rayDirection.y), out_blockHit.z) == EMPTY) {
                terrain->setBlockAt(out_blockHit.x, out_blockHit.y + glm::sign(rayDirection.y), out_blockHit.z, blockType);
                //terrain->getChunkAt(out_blockHit.x, out_blockHit.z).get()->createVBOdata();
            }
        } else if (m_interfaceAxis == 2) {
            if (terrain->getBlockAt(out_blockHit.x + glm::sign(rayDirection.x), out_blockHit.y, out_blockHit.z) == EMPTY) {
                terrain->setBlockAt(out_blockHit.x + glm::sign(rayDirection.x), out_blockHit.y, out_blockHit.z, blockType);
                //terrain->getChunkAt(out_blockHit.x, out_blockHit.z).get()->createVBOdata();
            }
        }
    }
}

void Player::setCameraWidthHeight(unsigned int w, unsigned int h) {
    m_camera.setWidthHeight(w, h);
}

void Player::moveAlongVector(glm::vec3 dir) {
    Entity::moveAlongVector(dir);
    m_camera.moveAlongVector(dir);
}
void Player::moveForwardLocal(float amount) {
    Entity::moveForwardLocal(amount);
    m_camera.moveForwardLocal(amount);
}
void Player::moveRightLocal(float amount) {
    Entity::moveRightLocal(amount);
    m_camera.moveRightLocal(amount);
}
void Player::moveUpLocal(float amount) {
    Entity::moveUpLocal(amount);
    m_camera.moveUpLocal(amount);
}
void Player::moveForwardGlobal(float amount) {
    Entity::moveForwardGlobal(amount);
    m_camera.moveForwardGlobal(amount);
}
void Player::moveRightGlobal(float amount) {
    Entity::moveRightGlobal(amount);
    m_camera.moveRightGlobal(amount);
}
void Player::moveUpGlobal(float amount) {
    Entity::moveUpGlobal(amount);
    m_camera.moveUpGlobal(amount);
}
void Player::rotateOnForwardLocal(float degrees) {
    Entity::rotateOnForwardLocal(degrees);
    m_camera.rotateOnForwardLocal(degrees);
}
void Player::rotateOnRightLocal(float degrees) {
    Entity::rotateOnRightLocal(degrees);
    m_camera.rotateOnRightLocal(degrees);
}
void Player::rotateOnUpLocal(float degrees) {
    Entity::rotateOnUpLocal(degrees);
    m_camera.rotateOnUpLocal(degrees);
}
void Player::rotateOnForwardGlobal(float degrees) {
    Entity::rotateOnForwardGlobal(degrees);
    m_camera.rotateOnForwardGlobal(degrees);
}
void Player::rotateOnRightGlobal(float degrees) {
    Entity::rotateOnRightGlobal(degrees);
    m_camera.rotateOnRightGlobal(degrees);
}
void Player::rotateOnUpGlobal(float degrees) {
    Entity::rotateOnUpGlobal(degrees);
    m_camera.rotateOnUpGlobal(degrees);
}

QString Player::posAsQString() const {
    std::string str("( " + std::to_string(m_position.x) + ", " + std::to_string(m_position.y) + ", " + std::to_string(m_position.z) + ")");
    return QString::fromStdString(str);
}
QString Player::velAsQString() const {
    std::string str("( " + std::to_string(m_velocity.x) + ", " + std::to_string(m_velocity.y) + ", " + std::to_string(m_velocity.z) + ")");
    return QString::fromStdString(str);
}
QString Player::accAsQString() const {
    std::string str("( " + std::to_string(m_acceleration.x) + ", " + std::to_string(m_acceleration.y) + ", " + std::to_string(m_acceleration.z) + ")");
    return QString::fromStdString(str);
}
QString Player::lookAsQString() const {
    std::string str("( " + std::to_string(m_forward.x) + ", " + std::to_string(m_forward.y) + ", " + std::to_string(m_forward.z) + ")");
    return QString::fromStdString(str);
}
