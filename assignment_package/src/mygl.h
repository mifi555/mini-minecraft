#ifndef MYGL_H
#define MYGL_H

#include "openglcontext.h"
#include "shaderprogram.h"
#include "scene/worldaxes.h"
#include "scene/camera.h"
#include "scene/terrain.h"
#include "scene/player.h"
#include "postprocessshader.h"
#include "scene/framebuffer.h"
#include "scene/quad.h"

#include "texture.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <smartpointerhelp.h>

class MyGL : public OpenGLContext
{
    Q_OBJECT
private:
    friend ShaderProgram;

    WorldAxes m_worldAxes; // A wireframe representation of the world axes. It is hard-coded to sit centered at (32, 128, 32).
    ShaderProgram m_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram m_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)
    ShaderProgram m_progInstanced;// A shader program that is designed to be compatible with instanced rendering
    ShaderProgram m_progSky; // A screen-space shader for creating the sky background

    // ~~~
    PostProcessShader m_progWater;// A shader program used for post-process water effect rendering.
    PostProcessShader m_progLava;// A shader program used for post-process lava effect rendering.
    GLuint m_renderedTexture;
    int m_time;
    FrameBuffer m_frameBuffer; // Frame buffer used for scene rendering.


    Quad m_quad;  // Quadrangle used for post-process rendering.
    // ~~~

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Terrain m_terrain; // All of the Chunks that currently comprise the world.
    Player m_player; // The entity controlled by the user. Contains a camera to display what it sees as well.
    InputBundle m_inputs; // A collection of variables to be updated in keyPressEvent, mouseMoveEvent, mousePressEvent, etc.

    qint64 m_currMSecSinceEpoch;

    BlockType m_blockType;

    QTimer m_timer; // Timer linked to tick(). Fires approximately 60 times per second.

    Texture m_texture;

public:
    void prepFrameBuffer();
    void render3DScene();
    void performPostprocessRenderPass();
    void moveMouseToCenter();
    void sendPlayerDataToGUI() const;

    // helper functions
    glm::ivec2 playerCurrentChunk();
    glm::ivec2 playerCurrentZone();
public:
    explicit MyGL(QWidget *parent = nullptr);
    ~MyGL();

    // Called once when MyGL is initialized.
    // Once this is called, all OpenGL function
    // invocations are valid (before this, they
    // will cause segfaults)
    void initializeGL() override;
    // Called whenever MyGL is resized.
    void resizeGL(int w, int h) override;
    // Called whenever MyGL::update() is called.
    // In the base code, update() is called from tick().
    void paintGL() override;

    // Called from paintGL().
    // Calls Terrain::draw().
    void renderTerrain();

    friend class Entity;
protected:
    // Automatically invoked when the user
    // presses a key on the keyboard
    void keyPressEvent(QKeyEvent *e);
    // Automatically invoked when the user
    // moves the mouse
    void mouseMoveEvent(QMouseEvent *e);
    // Automatically invoked when the user
    // presses a mouse button
    void mousePressEvent(QMouseEvent *e);

    void keyReleaseEvent(QKeyEvent *e);

private slots:
    void tick(); // Slot that gets called ~60 times per second by m_timer firing.

signals:
    void sig_sendPlayerPos(QString) const;
    void sig_sendPlayerVel(QString) const;
    void sig_sendPlayerAcc(QString) const;
    void sig_sendPlayerLook(QString) const;
    void sig_sendPlayerChunk(QString) const;
    void sig_sendPlayerTerrainZone(QString) const;
};


#endif // MYGL_H
