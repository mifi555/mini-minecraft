#include <scene/chunk.h>
#include <array>


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

#define ARR_SIZE 4

// for use in createVBOdata()
struct BlockFace {
    glm::ivec3 direction;
    std::array<glm::vec4, ARR_SIZE> pos;
    glm::vec4 nor;
};

// todo: values for pos and nor????
std::array<BlockFace, 6> neighbouringFaces = { {
    { glm::ivec3(1, 0, 0),  {}, glm::vec4(1,0,0,0) },                   // xpos  right face
    { glm::ivec3(-1, 0, 0), {}, glm::vec4(-1,0,0,0)  },                 // xneg  left face
    { glm::ivec3(0, 1, 0),  {}, glm::vec4(0,1,0,0) },                   // ypos  top face
    { glm::ivec3(0, -1, 0), {}, glm::vec4(0,-1,0,0)  },                 // yneg  bottom face
    { glm::ivec3(0, 0, 1),  {}, glm::vec4(0,0,1,0) },                   // zpos  front face
    { glm::ivec3(0, 0, -1), {}, glm::vec4(0,0,-1,0)  }                  // zneg  back face
}};

// todo: this is temporary, before we reach milestone for defining UV coords for textured blocks
std::unordered_map<BlockType, glm::vec4> blocktype_to_color;

// Chunk information available to us:
// - all block data in 16 x 256 x 16
// - x, z coords of chunk in world space (min bounding corner)
void Chunk::createVBOdata() {
    // todo: creates the interleaved vbo data
    // check every block to see if it's NOT empty
    // check the neighbours of each non-empty block to see if they ARE empty
    // if a nebour is empty, add VBO data for a face in that direction
        // vertex pos, vertex col, v normal, idx

    // zyx because it's more cache efficient
    for (int z = 0; z < 16; z++) {
        for (int y = 0; y < 256; y++) {
            for (int x = 0; x < 16; x++) {
                BlockType current = this->getBlockAt(x, y, z);

                if (current != EMPTY) {
                    for (BlockFace& n : neighbouringFaces) {
                        glm::ivec3 offset = glm::ivec3(x, y, z) + n.direction;
                        BlockType neighbour = this->getBlockAt(offset.x, offset.y, offset.z);
                        if (neighbour == EMPTY) {
                            // add to vector of vec4 blah
                        }
                    }
                    //BlockType neighbour = this->getBlockAt(x - 1, y, z);
//                    if (neighbour == EMPTY) {
//                        // add to vector of vec4 data pos, nor, col, info for x - 1 face
//                    }
                    // x - 1, x + 1, z - 1, z + 1 (4 adjacent blocks...) ????

                }
            }
        }
    }
}
