#pragma once
#include "glm_includes.h"

struct InputBundle {
    bool wPressed, aPressed, sPressed, dPressed;
    bool ePressed, qPressed, fPressed;
    bool spacePressed;
    float mouseX, mouseY, mouseXpreviousPos, mouseYpreviousPos;

    //flags for flight and ground mode
    bool flight, ground;

    InputBundle()
        : wPressed(false), aPressed(false), sPressed(false),
        dPressed(false), ePressed(false), qPressed(false), fPressed(false), spacePressed(false), mouseX(0.f), mouseY(0.f),
        flight(true), ground(true)
    {}
};

class Entity {
protected:
    // Vectors that define the axes of our local coordinate system
    glm::vec3 m_forward, m_right, m_up;

    glm::vec3 m_position;               // world position
    glm::vec3 m_positionPrevious;       // previous world position

    friend class MyGL;

public:
    // A readonly reference to position for external use
    const glm::vec3& mcr_position;
    const glm::vec3& mcr_positionPrevious;

    // Various constructors
    Entity();
    Entity(glm::vec3 pos);
    Entity(const Entity &e);
    virtual ~Entity();

    // To be called by MyGL::tick()
    virtual void tick(float dT, InputBundle &input) = 0;

    // Translate along the given vector
    virtual void moveAlongVector(glm::vec3 dir);

    // Translate along one of our local axes
    virtual void moveForwardLocal(float amount);
    virtual void moveRightLocal(float amount);
    virtual void moveUpLocal(float amount);

    // Translate along one of the world axes
    virtual void moveForwardGlobal(float amount);
    virtual void moveRightGlobal(float amount);
    virtual void moveUpGlobal(float amount);

    // Rotate about one of our local axes
    virtual void rotateOnForwardLocal(float degrees);
    virtual void rotateOnRightLocal(float degrees);
    virtual void rotateOnUpLocal(float degrees);

    // Rotate about one of the world axes
    virtual void rotateOnForwardGlobal(float degrees);
    virtual void rotateOnRightGlobal(float degrees);
    virtual void rotateOnUpGlobal(float degrees);
};
