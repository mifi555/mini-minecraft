#include <scene/chunk.h>
#include <array>
#include "chunkconstants.h"

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

void Chunk::buildInterleavedVBOFromData(std::vector<float> vertexData, std::vector<GLuint> idxData) {
    // todo:
}

void insertVec4(std::vector<GLfloat> &v, const glm::vec4 &data) {
    v.push_back(data.x);
    v.push_back(data.y);
    v.push_back(data.z);
    v.push_back(data.w);
}

// Chunk information available to us:
// - all block data in 16 x 256 x 16
// - x, z coords of chunk in world space (min bounding corner)
void Chunk::createVBOdata() {
    // todo: creates the interleaved vbo data
    // check every block to see if it's NOT empty
    // check the neighbours of each non-empty block to see if they ARE empty
    // if a nebour is empty, add VBO data for a face in that direction
        // vertex pos, vertex col, v normal, idx

    std::vector<GLfloat> vertexData;
    std::vector<GLuint> idxData;
    int idxCounter = 0;

    // zyx because it's more cache efficient
    for (int z = 0; z < 16; z++) {
        for (int y = 0; y < 256; y++) {
            for (int x = 0; x < 16; x++) {
                BlockType current = this->getBlockAt(x, y, z);
                if (current != EMPTY) {
                    for (const ChunkConstants::BlockFace& n : ChunkConstants::neighbouringFaces) {
                        glm::ivec3 offset = glm::ivec3(x, y, z) + n.direction;
                        BlockType neighbour = this->getBlockAt(offset.x, offset.y, offset.z);
                        if (neighbour == EMPTY) {
                            for (auto &pos : n.pos) {
                                insertVec4(vertexData, glm::vec4(x, y, z, 1.f) + pos);  // vertex position
                                insertVec4(vertexData, n.nor);                          // vertex normal
                                insertVec4(vertexData, glm::vec4(1.f, 1.f, 1.f, 1.f));  // todo: vertex color
                                idxData.push_back(idxCounter++);
                            }
                        }
                    }
                }
            }
        }
    }

    buildInterleavedVBOFromData(vertexData, idxData);
}
