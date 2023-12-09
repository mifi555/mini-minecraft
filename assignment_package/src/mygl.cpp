#include "mygl.h"
#include <glm_includes.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>

#include <qdatetime.h>


MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_worldAxes(this),
      m_progLambert(this), m_progFlat(this), m_progInstanced(this),
      m_progSky(this),
      m_progWater(this),
      m_progLava(this),
      m_renderedTexture(-1),
      m_time(0.f),
      m_frameBuffer(this, this->width(), this->height(), this->devicePixelRatio()),
      m_quad(this),
      vao(-1),
      m_terrain(this), m_player(glm::vec3(48.f, 129.f, 48.f), m_terrain),
      m_currMSecSinceEpoch(QDateTime::currentMSecsSinceEpoch()), m_blockType(GRASS),
      m_timer(),
      m_texture(this)
{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
    // Tell the timer to redraw 60 times per second
    m_timer.start(16);
    setFocusPolicy(Qt::ClickFocus);

    setMouseTracking(true); // MyGL will track the mouse's movements even if a mouse button is not pressed
    setCursor(Qt::BlankCursor); // Make the cursor invisible
}

MyGL::~MyGL() {
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    //~~~~
    //**Enabling alpha blending so that transparency can be applied to WATER blocks.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_texture.create(":/textures/minecraft_textures_all.png");
    m_texture.load(0);

    glEnable(GL_LINE_SMOOTH);

    //~~~

    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.37f, 0.74f, 1.0f, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instance of the world axes
    m_worldAxes.createVBOdata();
    m_quad.createVBOdata();
    m_frameBuffer.create();

    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
//    m_progInstanced.create(":/glsl/instanced.vert.glsl", ":/glsl/lambert.frag.glsl");

    //*** Procedural sky: create sky shader
    m_progSky.create(":/glsl/sky.vert.glsl", ":/glsl/sky.frag.glsl");
    m_quad.createVBOdata();

    // Create and set up post-processing shaders.
    m_progWater.create(":/glsl/passthrough.vert.glsl", ":/glsl/water.frag.glsl");
    m_progLava.create(":/glsl/passthrough.vert.glsl", ":/glsl/lava.frag.glsl");

    m_progWater.setupMemberVars();
    m_progLava.setupMemberVars();

    // Set a color with which to draw geometry.
    // This will ultimately not be used when you change
    // your program to render Chunks with vertex colors
    // and UV coordinates
    m_progLambert.setGeometryColor(glm::vec4(0,1,0,1));

    // Alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);

    // initialize starting terrain zones at spawn
    m_terrain.initializeTerrain();
}

void MyGL::resizeGL(int w, int h) {
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_player.setCameraWidthHeight(static_cast<unsigned int>(w), static_cast<unsigned int>(h));
    glm::mat4 viewproj = m_player.mcr_camera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);

    //***Procedural Sky
    m_progSky.setViewProjMatrix(glm::inverse(viewproj));

    m_progSky.useMe();
    this->glUniform2i(m_progSky.unifDimensions, width() * this->devicePixelRatio(), height() * this->devicePixelRatio());
    this->glUniform3f(m_progSky.unifEye, m_player.mcr_camera.mcr_position.x,
                      m_player.mcr_camera.mcr_position.y,
                      m_player.mcr_camera.mcr_position.z);

    //


    m_frameBuffer.resize(this->width() * this->devicePixelRatio(), this->height() * this->devicePixelRatio(), 1);
    m_frameBuffer.create();

    printGLErrorLog();
}


// MyGL's constructor links tick() to a timer that fires 60 times per second.
// We're treating MyGL as our game engine class, so we're going to perform
// all per-frame actions here, such as performing physics updates on all
// entities in the scene.
void MyGL::tick() {
    //current_time - previously stored time
    float dT = (QDateTime::currentMSecsSinceEpoch() - m_currMSecSinceEpoch) / 1000.0f;
    m_player.tick(dT, m_inputs);
    m_currMSecSinceEpoch = QDateTime::currentMSecsSinceEpoch();

    m_terrain.multithreadedWork(m_player.mcr_position, m_player.mcr_positionPrevious);

    update(); // Calls paintGL() as part of a larger QOpenGLWidget pipeline
    sendPlayerDataToGUI(); // Updates the info in the secondary window displaying player data
//    m_time++
//    m_realTime += dT
}

glm::ivec2 MyGL::playerCurrentChunk() {
    glm::vec2 pPos(m_player.mcr_position.x, m_player.mcr_position.z);
    return glm::ivec2(16 * glm::ivec2(glm::floor(pPos / 16.f)));
}

glm::ivec2 MyGL::playerCurrentZone() {
    glm::vec2 pPos(m_player.mcr_position.x, m_player.mcr_position.z);
    return glm::ivec2(64 * glm::ivec2(glm::floor(pPos / 64.f)));
}

//provided
void MyGL::sendPlayerDataToGUI() const {
    emit sig_sendPlayerPos(m_player.posAsQString());
    emit sig_sendPlayerVel(m_player.velAsQString());
    emit sig_sendPlayerAcc(m_player.accAsQString());
    emit sig_sendPlayerLook(m_player.lookAsQString());
    glm::vec2 pPos(m_player.mcr_position.x, m_player.mcr_position.z);
    glm::ivec2 chunk(16 * glm::ivec2(glm::floor(pPos / 16.f)));
    glm::ivec2 zone(64 * glm::ivec2(glm::floor(pPos / 64.f)));
    emit sig_sendPlayerChunk(QString::fromStdString("( " + std::to_string(chunk.x) + ", " + std::to_string(chunk.y) + " )"));
    emit sig_sendPlayerTerrainZone(QString::fromStdString("( " + std::to_string(zone.x) + ", " + std::to_string(zone.y) + " )"));
}

void MyGL::prepFrameBuffer() {
    // Render to our framebuffer rather than the viewport.
    m_frameBuffer.bindFrameBuffer();
    printGLErrorLog();

    // Render on the whole framebuffer, complete from the lower left corner to the upper right
    glViewport(0, 0,this->width() * this->devicePixelRatio(), this->height() * this->devicePixelRatio());

    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void MyGL::render3DScene() {
    m_progFlat.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progLambert.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progInstanced.setViewProjMatrix(m_player.mcr_camera.getViewProj());

    //***Procedural Sky
    // Sky demo
    m_progSky.setViewProjMatrix(glm::inverse(m_player.mcr_camera.getViewProj()));
    m_progSky.useMe();
    this->glUniform3f(m_progSky.unifEye, m_player.mcr_camera.mcr_position.x,
                      m_player.mcr_camera.mcr_position.y,
                      m_player.mcr_camera.mcr_position.z);

    // pass real time into shader
    float dT = (QDateTime::currentMSecsSinceEpoch() - m_currMSecSinceEpoch) / 1000.0f;
    m_time += dT;
    this->glUniform1f(m_progSky.unifTime, m_time);

    //for pass in real time as m_time for terrain to reflect shading based on sun rotation
    this->glUniform1f(m_progLambert.unifTime, m_time);

//    std::cout << m_time << std::endl;
    //this->glUniform1f(m_progSky.unifTime, m_time++);


    m_progSky.draw(m_quad);

    m_progWater.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progLava.setViewProjMatrix(m_player.mcr_camera.getViewProj());

    renderTerrain();

    glDisable(GL_DEPTH_TEST);
    m_progFlat.setModelMatrix(glm::mat4());
    m_progFlat.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progFlat.draw(m_worldAxes);
    glEnable(GL_DEPTH_TEST);
}

void MyGL::performPostprocessRenderPass() {
    // Tell OpenGL to render to the viewport's frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, this->defaultFramebufferObject());

    // Render on the whole framebuffer, complete from the lower left corner to the upper right
    glViewport(0,0,this->width() * this->devicePixelRatio(), this->height() * this->devicePixelRatio());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_frameBuffer.bindToTextureSlot(1);

    // Display water.
    if (m_player.m_water == true) {
        m_progWater.setModelMatrix(glm::mat4());
        m_progWater.setViewProjMatrix(m_player.mcr_camera.getViewProj());

        // Set our texture sampler to user Texture Unit 1
        this->glUniform1i(m_progWater.unifSampler2D, m_frameBuffer.getTextureSlot());

        m_progWater.draw(m_quad, m_frameBuffer.getTextureSlot());
        m_progWater.setTime(m_time);
        // Display lava.
    } else if (m_player.m_lava == true) {
        m_progLava.setModelMatrix(glm::mat4());
        m_progLava.setViewProjMatrix(m_player.mcr_camera.getViewProj());

        // Set our texture sampler to user Texture Unit 1
        this->glUniform1i(m_progLava.unifSampler2D, m_frameBuffer.getTextureSlot());

        m_progLava.draw(m_quad, m_frameBuffer.getTextureSlot());
        m_progLava.setTime(m_time);
    }

    m_time++;
}


// This function is called whenever update() is called.
// MyGL's constructor links update() to a timer that fires 60 times per second,
// so paintGL() called at a rate of 60 frames per second.
void MyGL::paintGL() {
    if (m_player.m_water == true || m_player.m_lava == true) {
        prepFrameBuffer();
    }

    render3DScene();

    if (m_player.m_water == true || m_player.m_lava == true) {
        performPostprocessRenderPass();
    }
}

// TODO: Change this so it renders the nine zones of generated
// terrain that surround the player (refer to Terrain::m_generatedTerrain
// for more info)
void MyGL::renderTerrain() {
    m_texture.bind(0);
    m_progLambert.setTextureSampler(0);
    m_progLambert.setTime(m_time++);

    //***Fog
    m_progLambert.setPlayerPosition(glm::vec4(m_player.mcr_position.x,
                                              m_player.mcr_position.y,
                                              m_player.mcr_position.z, 0));

    m_progLambert.setModelMatrix(glm::mat4(1.f));
    m_terrain.draw(m_player.mcr_position, &m_progLambert);
}

void MyGL::keyPressEvent(QKeyEvent *e) {
    float amount = 2.0f;

    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }

    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used, but I really dislike their
    // syntax so I chose to be lazy and use a long
    // chain of if statements instead

    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        m_player.rotateOnUpGlobal(-amount);
    } else if (e->key() == Qt::Key_Left) {
        m_player.rotateOnUpGlobal(amount);
    } else if (e->key() == Qt::Key_Up) {
        m_player.rotateOnRightLocal(-amount);
    } else if (e->key() == Qt::Key_Down) {
        m_player.rotateOnRightLocal(amount);
    } else if (e->key() == Qt::Key_W) {
        m_inputs.wPressed = true;
    } else if (e->key() == Qt::Key_S) {
        m_inputs.sPressed = true;
    } else if (e->key() == Qt::Key_D) {
        m_inputs.dPressed = true;
    } else if (e->key() == Qt::Key_A) {
        m_inputs.aPressed = true;
    } else if (e->key() == Qt::Key_F) {
        //toggle flight mode on/off
        m_player.toggleFlightMode();
        m_player.m_water = false;
        m_player.m_lava = false;
    }
  
    //keys E and Q are specific to flightmode
    if (m_player.m_flightMode) {
        if (e->key() == Qt::Key_Q) {
            m_inputs.qPressed = true;
        } else if (e->key() == Qt::Key_E) {
            m_inputs.ePressed = true;
        }
    } else {
        if (e->key() == Qt::Key_Space) {
            m_inputs.spacePressed = true;
        }
    }
}

//Key Release Event

void MyGL::keyReleaseEvent(QKeyEvent *e) {
        if (e->key() == Qt::Key_W) {
            m_inputs.wPressed = false;
        } else if (e->key() == Qt::Key_S) {
            m_inputs.sPressed = false;
        } else if (e->key() == Qt::Key_D) {
            m_inputs.dPressed = false;
        } else if (e->key() == Qt::Key_A) {
            m_inputs.aPressed = false;
        } else if (e->key() == Qt::Key_Q) {
            m_inputs.qPressed = false;
        } else if (e->key() == Qt::Key_E) {
            m_inputs.ePressed = false;
        } else if (e->key() == Qt::Key_Space) {
            m_inputs.spacePressed = false;
        }
}

void MyGL::moveMouseToCenter() {
    QCursor::setPos(this->mapToGlobal(QPoint(width() / 2, height() / 2)));
}

void MyGL::mouseMoveEvent(QMouseEvent *e) {
    //move mouse center to pevents hitting the edges of the screen
    //moveMouseToCenter();
    float sensitivity = 0.1f;
    float dX = (e->position().x() - m_inputs.mouseX) * (width() / 360.f);
    float dY = (e->position().y() - m_inputs.mouseY) * (height() / 360.f);

    // dX = glm::clamp(dX, -360.0f, 360.0f);
    dY = glm::clamp(dY, -90.0f, 90.0f);

    m_inputs.mouseX = e->position().x();
    m_inputs.mouseY = e->position().y();

    m_player.rotateOnRightLocal(-dY * sensitivity);
    m_player.rotateOnUpGlobal(-dX * sensitivity);

    // for some reason this doesn't work on my Win11 machine -- Mikey
#ifdef __APPLE__
    moveMouseToCenter();
#endif
}

void MyGL::mousePressEvent(QMouseEvent *e) {
    if (e->button() == Qt::LeftButton) {
        m_player.removeBlock(&m_terrain);
    } else if (e->button() == Qt::RightButton) {
        m_player.placeBlock(&m_terrain, BlockType::GRASS);
    }
}
