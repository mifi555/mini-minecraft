#include <scene/chunk.h>
#include <array>
#include "chunkconstants.h"

std::unordered_map<Direction, Chunk *, EnumHash>& Chunk::neighbors()
{
    return m_neighbors;
}

int Chunk::getMinX() const
{
    return minX;
}

int Chunk::getMinZ() const
{
    return minZ;
}

Chunk::Chunk(OpenGLContext* context, int x, int z)
    : Drawable(context),
    m_blocks(),
    minX(x),
    minZ(z),
    m_neighbors{{XPOS, nullptr}, {XNEG, nullptr}, {ZPOS, nullptr}, {ZNEG, nullptr}}
{
    std::fill_n(m_blocks.begin(), 65536, EMPTY);
}

// Does bounds checking with at()
BlockType Chunk::getBlockAt(unsigned int x, unsigned int y, unsigned int z) const {
    return m_blocks.at(x + 16 * y + 16 * 256 * z);
}

// Exists to get rid of compiler warnings about int -> unsigned int implicit conversion
BlockType Chunk::getBlockAt(int x, int y, int z) const {
    return getBlockAt(static_cast<unsigned int>(x), static_cast<unsigned int>(y), static_cast<unsigned int>(z));
}

// Does bounds checking with at()
void Chunk::setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t) {
    m_blocks.at(x + 16 * y + 16 * 256 * z) = t;
}


const static std::unordered_map<Direction, Direction, EnumHash> oppositeDirection {
    {XPOS, XNEG},
    {XNEG, XPOS},
    {YPOS, YNEG},
    {YNEG, YPOS},
    {ZPOS, ZNEG},
    {ZNEG, ZPOS}
};

void Chunk::linkNeighbor(uPtr<Chunk> &neighbor, Direction dir) {
    if(neighbor != nullptr) {
        this->m_neighbors[dir] = neighbor.get();
        neighbor->m_neighbors[oppositeDirection.at(dir)] = this;
    }
}

void Chunk::createVBOBuffer(std::vector<GLfloat>& vertexDataOpaque, std::vector<GLfloat>& vertexDataTransparent,
                            std::vector<GLuint> &idxDataOpaque, std::vector<GLuint> &idxDataTransparent){


    Drawable::m_count = idxDataOpaque.size();

    Drawable::generateInterleaved();
    if (Drawable::bindInterleaved()) {
        mp_context->glBufferData(
            GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexDataOpaque.size(), vertexDataOpaque.data(), GL_STATIC_DRAW
            );
    }

    Drawable::generateIdx();
    if (Drawable::bindIdx()) {
        mp_context->glBufferData(
            GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * idxDataOpaque.size(), idxDataOpaque.data(), GL_STATIC_DRAW
            );
    }

    Drawable::m_countTransparent = idxDataTransparent.size();

    Drawable::generateTransparent();
    if (Drawable::bindTransparent()) {
        mp_context->glBufferData(
            GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexDataTransparent.size(), vertexDataTransparent.data(), GL_STATIC_DRAW
            );
    }

    Drawable::generateIdxTransparent();
    if (Drawable::bindIdxTransparent()) {
        mp_context->glBufferData(
            GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat) * idxDataTransparent.size(), idxDataTransparent.data(), GL_STATIC_DRAW
            );
    }

}

#if 0
void Chunk::createVBOBuffer(std::vector<GLfloat> &vertexData, std::vector<GLuint> &idxData) {

    Drawable::m_count = idxData.size();

    Drawable::generateInterleaved();
    if (Drawable::bindInterleaved()) {
        mp_context->glBufferData(
            GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexData.size(), vertexData.data(), GL_STATIC_DRAW
        );
    }

    Drawable::generateIdx();
    if (Drawable::bindIdx()) {
        mp_context->glBufferData(
            GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * idxData.size(), idxData.data(), GL_STATIC_DRAW
        );
    }
}
#endif

void insertVec4(std::vector<GLfloat> &v, const glm::vec4 &data) {
    v.push_back(data.x);
    v.push_back(data.y);
    v.push_back(data.z);
    v.push_back(data.w);
}

void createFaceIndices(std::vector<GLuint>& idxData, const std::array<GLuint, ChunkConstants::VERT_COUNT> &faceIndices) {
    // 0: UR, 1: LR, 2: LL, 3: UL
    // First Triangle: 0, 3, 1
    idxData.push_back(faceIndices.at(0));
    idxData.push_back(faceIndices.at(3));
    idxData.push_back(faceIndices.at(1));

    // Second Triangle: 1, 3, 2
    idxData.push_back(faceIndices.at(1));
    idxData.push_back(faceIndices.at(3));
    idxData.push_back(faceIndices.at(2));
}

void Chunk::createMultithreaded(ChunkVBOData& data) {
    // TODO: For transparent types, we'll need to populate a "transparent" buffer that will be drawn seperately.

    // modulo operator that returns a remainder that is the same sign as it's operand
    auto mod = [](int a, int b) { return (a % b + b) % b; };

    int idxCounterOpaque = 0;
    int idxCounterTransparent = 0;

    // change this so that it vertices are drawn relative to worldspace (using minX / minZ)
    // zyx because it's more cache efficient
    for (int z = 0; z < 16; z++) {
        for (int y = 0; y < 256; y++) {
            for (int x = 0; x < 16; x++) {
                BlockType current = this->getBlockAt(x, y, z);

                std::vector<GLfloat> &vboData = current == WATER ? data.vboDataTransparent : data.vboDataOpaque;
                std::vector<GLuint> &idxData = current == WATER ? data.idxDataTransparent : data.idxDataOpaque;
                int &idxCounter = current == WATER ? idxCounterTransparent : idxCounterOpaque;

                if (current != EMPTY) {
                    // Choose the right buffer and counter based on transparency

                    for (const ChunkConstants::BlockFace &n : ChunkConstants::neighbouringFaces) {

                        // this is hacky, but for water, we only care about drawing it's top face
                        if ((current == WATER && n.pos != ChunkConstants::TopFace)) {
                            continue;
                        }

                        glm::ivec3 offset = glm::ivec3(x, y, z) + n.direction;
                        BlockType neighbour;

                        // we have to check if the neighbouring block belongs to another chunk
                        if (offset.y < 0 || offset.y > 255) { // the y limits of our world, it's always gonna be empty
                            neighbour = EMPTY;
                        } else if (offset.x < 0 || offset.x > 15) {  // east and west neighbouring chunks
                            Direction direction = offset.x < 0 ? XNEG : XPOS;
                            Chunk* neighbouringChunk = m_neighbors.at(direction);
                            neighbour = neighbouringChunk ? neighbouringChunk->getBlockAt(mod(offset.x, 16), y, z) : EMPTY;
                        } else if (offset.z < 0 || offset.z > 15) {  // north and south neighbouring chunks
                            Direction direction = offset.z < 0 ? ZNEG : ZPOS;
                            Chunk* neighbouringChunk = m_neighbors.at(direction);
                            neighbour = neighbouringChunk ? neighbouringChunk->getBlockAt(x, y, mod(offset.z, 16)) : EMPTY;
                        } else {                                     // in the current chunk
                            neighbour = this->getBlockAt(offset.x, offset.y, offset.z);
                        }

                        if (neighbour == EMPTY || (!hasAlpha[current] && hasAlpha[neighbour])) {
                            std::array<GLuint, ChunkConstants::VERT_COUNT> faceIndices;
                            for (size_t i = 0; i < n.pos.size(); i++) {

                                glm::vec4 pos = glm::vec4(minX + x, y, minZ + z, 1.f) + n.pos[i];

                                //convert direction
                                Direction dir = convertToDirection(n.direction);

                                //offset: 1/16.f:
                                //glm::vec2 uv = blockFaceUVs.at(current).at(dir);

                                // Retrieve the bottom-left corner UV for this face of the block
                                glm::vec2 uvBottomLeft = blockFaceUVs.at(current).at(dir);
                                // Calculate UVs for all four corners
                                glm::vec2 uvTopLeft = uvBottomLeft + glm::vec2(0, 1/16.f);
                                glm::vec2 uvTopRight = uvBottomLeft + glm::vec2(1/16.f, 1/16.f);
                                glm::vec2 uvBottomRight = uvBottomLeft + glm::vec2(1/16.f, 0);

                                glm::vec2 uv;

                                switch (i) {
                                case 0:
                                    uv = uvTopLeft;
                                    break;
                                case 1:
                                    uv = uvBottomLeft;
                                    break;
                                case 2:
                                    uv = uvBottomRight;
                                    break;
                                case 3:
                                    uv = uvTopRight;
                                    break;
                                }

                                bool animatable = isBlockAnimateable(current);

                                insertVec4(vboData, pos);  // vertex position
                                insertVec4(vboData, n.nor); // vertex normal
                                insertVec4(vboData, glm::vec4(uv[0], uv[1], animatable, 0)); // uv coords. z coordinate is used as a "animateable" flag
                                faceIndices.at(i) = idxCounter++;
                            }
                            // add index data for this face
                            createFaceIndices(idxData, faceIndices);
                        }
                    }
                }
            }
        }
    }
    return;
}


bool Chunk::isBlockAnimateable(BlockType type) {
    return type == WATER || type == LAVA;
}

Direction Chunk::convertToDirection(const glm::ivec3& dirVec) {
    if (dirVec == glm::ivec3(1, 0, 0)) {
        return XPOS;
    } else if (dirVec == glm::ivec3(-1, 0, 0)) {
        return XNEG;
    } else if (dirVec == glm::ivec3(0, 1, 0)) {
        return YPOS;
    } else if (dirVec == glm::ivec3(0, -1, 0)) {
        return YNEG;
    } else if (dirVec == glm::ivec3(0, 0, 1)) {
        return ZPOS;
    } else if (dirVec == glm::ivec3(0, 0, -1)) {
        return ZNEG;
    }
}

// Chunk information available to us:
// - all block data in 16 x 256 x 16
// - x, z coords of chunk in world space (min bounding corner)
void Chunk::createVBOdata() {
    // check every block to see if it's NOT empty
    // check the neighbours of each non-empty block to see if they ARE empty
    // if a nebour is empty, add VBO data for a face in that direction
        // vertex pos, vertex col, v normal, idx
    std::exit(-1);

#if 0
    std::vector<GLfloat> vertexData;
    std::vector<GLuint> idxData;
    int idxCounter = 0;

    // change this so that it vertices are drawn relative to worldspace (using minX / minZ)
    // zyx because it's more cache efficient
    for (int z = 0; z < 16; z++) {
        for (int y = 0; y < 256; y++) {
            for (int x = 0; x < 16; x++) {
                BlockType current = this->getBlockAt(x, y, z);
                if (current != EMPTY) {
                    for (const ChunkConstants::BlockFace &n : ChunkConstants::neighbouringFaces) {
                        glm::ivec3 offset = glm::ivec3(x, y, z) + n.direction;

                        BlockType neighbour;

                        if (offset.x < 0 || offset.x > 15 ||
                            offset.y < 0 || offset.y > 255 ||
                            offset.z < 0 || offset.z > 15) {
                            neighbour = EMPTY;
                        } else {
                            neighbour = this->getBlockAt(offset.x, offset.y, offset.z);
                        }


                        if (neighbour == EMPTY) {
                            std::array<GLuint, ChunkConstants::VERT_COUNT> faceIndices;
                            for (size_t i = 0; i < n.pos.size(); i++) {
                                insertVec4(vertexData, glm::vec4(minX + x, y, minZ + z, 1.f) + n.pos[i]);  // vertex position
                                insertVec4(vertexData, n.nor);                               // vertex normal
                                insertVec4(                                                  // vertex color
                                    vertexData,
                                    ChunkConstants::blocktype_to_color.at(current)
                                );
                                faceIndices.at(i) = idxCounter++;
                            }
                            // add index data for this face
                            createFaceIndices(idxData, faceIndices);
                        }
                    }
                }
            }
        }
    }
#endif
}
