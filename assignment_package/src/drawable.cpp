#include "drawable.h"
#include <glm_includes.h>

Drawable::Drawable(OpenGLContext* context)
    : m_countOpaque(-1),
      m_countTransparent(-1),
      m_bufOpaque(-1),
      m_bufIdxOpaque(-1),
      m_bufTransparent(-1),
      m_bufIdxTransparent(-1),
      m_opaqueGenerated(-1),
      m_idxOpaqueGenerated(-1),
      m_transparentGenerated(-1),
      m_idxTransparentGenerated(-1),
      m_count(-1),
      m_bufIdx(-1), m_bufPos(-1), m_bufNor(-1), m_bufCol(-1), m_bufUV(-1),
      m_idxGenerated(false),
      m_posGenerated(false),
      m_norGenerated(false),
      m_colGenerated(false),
      m_bufInterleaved(-1),
      m_interleavedGenerated(false),
      m_UVGenerated(false),
      mp_context(context)
{}

Drawable::~Drawable()
{}


void Drawable::destroyVBOdata()
{
    if (m_idxGenerated) {
        mp_context->glDeleteBuffers(1, &m_bufIdx);
        m_idxGenerated = false;
    }

    if (m_posGenerated) {
        mp_context->glDeleteBuffers(1, &m_bufPos);
        m_posGenerated = false;
    }

    if (m_norGenerated) {
        mp_context->glDeleteBuffers(1, &m_bufNor);
        m_norGenerated = false;
    }

    if (m_colGenerated) {
        mp_context->glDeleteBuffers(1, &m_bufCol);
        m_colGenerated = false;
    }

    if (m_interleavedGenerated) {
        mp_context->glDeleteBuffers(1, &m_bufInterleaved);
        m_interleavedGenerated = false;
    }

    if (m_UVGenerated) {
        mp_context->glDeleteBuffers(1, &m_bufUV);
        m_UVGenerated = false;
    }

    if (m_opaqueGenerated) {
        mp_context->glDeleteBuffers(1, &m_bufOpaque);
        m_opaqueGenerated = false;
    }

    if (m_transparentGenerated) {
        mp_context->glDeleteBuffers(1, &m_bufTransparent);
        m_transparentGenerated = false;
    }

    if (m_idxOpaqueGenerated) {
        mp_context->glDeleteBuffers(1, &m_bufIdxOpaque);
        m_idxOpaqueGenerated = false;
    }

    if (m_idxTransparentGenerated) {
        mp_context->glDeleteBuffers(1, &m_bufIdxTransparent);
        m_idxTransparentGenerated = false;
    }

    m_count = -1;
    m_countOpaque = -1;
    m_countTransparent = -1;
}

GLenum Drawable::drawMode()
{
    // Since we want every three indices in bufIdx to be
    // read to draw our Drawable, we tell that the draw mode
    // of this Drawable is GL_TRIANGLES

    // If we wanted to draw a wireframe, we would return GL_LINES

    return GL_TRIANGLES;
}

int Drawable::elemCount()
{
    return m_count;
}

int Drawable::elemCountOpaque(){
    return m_countOpaque;

}

int Drawable::elemCountTransparent(){
    return m_countTransparent;
}

void Drawable::generateIdx()
{
    m_idxGenerated = true;
    // Create a VBO on our GPU and store its handle in bufIdx
    mp_context->glGenBuffers(1, &m_bufIdx);
}

void Drawable::generatePos()
{
    m_posGenerated = true;
    // Create a VBO on our GPU and store its handle in bufPos
    mp_context->glGenBuffers(1, &m_bufPos);
}

void Drawable::generateNor()
{
    m_norGenerated = true;
    // Create a VBO on our GPU and store its handle in bufNor
    mp_context->glGenBuffers(1, &m_bufNor);
}

void Drawable::generateCol()
{
    m_colGenerated = true;
    // Create a VBO on our GPU and store its handle in bufCol
    mp_context->glGenBuffers(1, &m_bufCol);
}

void Drawable::generateInterleaved()
{
    m_interleavedGenerated = true;
    mp_context->glGenBuffers(1, &m_bufInterleaved);
}

void Drawable::generateUV()
{
    m_UVGenerated = true;
    // Create a VBO on our GPU and store its handle in bufUV.
    mp_context->glGenBuffers(1, &m_bufUV);
}





void Drawable::generateOpaque(){
    m_opaqueGenerated = true;
    mp_context->glGenBuffers(1, &m_bufOpaque);
}

void Drawable::generateTransparent(){
    m_transparentGenerated = true;
    mp_context->glGenBuffers(1, &m_bufTransparent);
}

//  texturing
void Drawable::generateIdxOpaque(){
    m_idxOpaqueGenerated = true;
    mp_context->glGenBuffers(1, &m_bufIdxOpaque);
}

void Drawable::generateIdxTransparent(){
    m_idxTransparentGenerated = true;
    mp_context->glGenBuffers(1, &m_bufIdxTransparent);
}

bool Drawable::bindIdxOpaque(){
    if(m_idxOpaqueGenerated) {
        mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdxOpaque);
    }
    return m_idxOpaqueGenerated;
}

bool Drawable::bindIdxTransparent(){
    if(m_idxTransparentGenerated) {
        mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdxTransparent);
    }
    return m_idxTransparentGenerated;
}

bool Drawable::bindOpaque(){
    if(m_opaqueGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufOpaque);
    }
    return m_opaqueGenerated;
}

bool Drawable::bindTransparent() {
    if(m_transparentGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufTransparent);
    }
    return m_transparentGenerated;
}





bool Drawable::bindIdx()
{
    if(m_idxGenerated) {
        mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdx);
    }
    return m_idxGenerated;
}

bool Drawable::bindPos()
{
    if(m_posGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);
    }
    return m_posGenerated;
}

bool Drawable::bindNor()
{
    if(m_norGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufNor);
    }
    return m_norGenerated;
}

bool Drawable::bindCol()
{
    if(m_colGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
    }
    return m_colGenerated;
}

bool Drawable::bindInterleaved()
{
    if (m_interleavedGenerated) {
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufInterleaved);
    }
    return m_interleavedGenerated;
}

bool Drawable::bindUV()
{
    if(m_UVGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufUV);
    }
    return m_UVGenerated;
}

InstancedDrawable::InstancedDrawable(OpenGLContext *context)
    : Drawable(context), m_numInstances(0), m_bufPosOffset(-1), m_offsetGenerated(false)
{}

InstancedDrawable::~InstancedDrawable(){}

int InstancedDrawable::instanceCount() const {
    return m_numInstances;
}

void InstancedDrawable::generateOffsetBuf() {
    m_offsetGenerated = true;
    mp_context->glGenBuffers(1, &m_bufPosOffset);
}

bool InstancedDrawable::bindOffsetBuf() {
    if(m_offsetGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufPosOffset);
    }
    return m_offsetGenerated;
}


void InstancedDrawable::clearOffsetBuf() {
    if(m_offsetGenerated) {
        mp_context->glDeleteBuffers(1, &m_bufPosOffset);
        m_offsetGenerated = false;
    }
}
void InstancedDrawable::clearColorBuf() {
    if(m_colGenerated) {
        mp_context->glDeleteBuffers(1, &m_bufCol);
        m_colGenerated = false;
    }
}
