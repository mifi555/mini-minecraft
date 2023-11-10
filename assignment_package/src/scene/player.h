#pragma once
#include "entity.h"
#include "camera.h"
#include "terrain.h"

#include "glm_includes.h"


class Player : public Entity {
private:
    glm::vec3 m_velocity, m_acceleration;
    Camera m_camera;
    const Terrain &mcr_terrain;

    //boolean flag to indicate if the player has jumped or not
    bool m_hasJumped;

    //for block placement
    float m_interfaceAxis;

    void processInputs(const Terrain &terrain, InputBundle &inputs);
    void computePhysics(float dT, const Terrain &terrain, InputBundle &input);


public:
    // Readonly public reference to our camera
    // for easy access from MyGL
    const Camera& mcr_camera;

    //boolean flag for flightmode
    bool m_flightMode;

    Player(glm::vec3 pos, const Terrain &terrain);
    virtual ~Player() override;

    void setCameraWidthHeight(unsigned int w, unsigned int h);

    void tick(float dT, InputBundle &input) override;

    //**

    //check if player is currently touching the ground
    bool playerOnGround(const Terrain &terrain, InputBundle &input);

    //check for collision with blocks
    void collision(const Terrain &terrain, glm::vec3 *rayDir);

    //gird march for collision
    bool gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection, const Terrain &terrain, float *out_dist, glm::ivec3 *out_blockHit);

    //toggle flight mode ON/OFF
    void toggleFlightMode();

    //remove block that the player is currently facing
    void removeBlock(Terrain *terrain);

    //place block adjacent to the block the player is facing
    void placeBlock(Terrain *terrain, BlockType blockType);

    //**

    // Player overrides all of Entity's movement
    // functions so that it transforms its camera
    // by the same amount as it transforms itself.
    void moveAlongVector(glm::vec3 dir) override;
    void moveForwardLocal(float amount) override;
    void moveRightLocal(float amount) override;
    void moveUpLocal(float amount) override;
    void moveForwardGlobal(float amount) override;
    void moveRightGlobal(float amount) override;
    void moveUpGlobal(float amount) override;
    void rotateOnForwardLocal(float degrees) override;
    void rotateOnRightLocal(float degrees) override;
    void rotateOnUpLocal(float degrees) override;
    void rotateOnForwardGlobal(float degrees) override;
    void rotateOnRightGlobal(float degrees) override;
    void rotateOnUpGlobal(float degrees) override;

    // For sending the Player's data to the GUI
    // for display
    QString posAsQString() const;
    QString velAsQString() const;
    QString accAsQString() const;
    QString lookAsQString() const;
};
