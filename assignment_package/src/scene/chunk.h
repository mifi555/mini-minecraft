#pragma once
#include "smartpointerhelp.h"
#include "glm_includes.h"
#include "drawable.h"

#include <array>
#include <unordered_map>
#include <cstddef>


//using namespace std;

// C++ 11 allows us to define the size of an enum. This lets us use only one byte
// of memory to store our different block types. By default, the size of a C++ enum
// is that of an int (so, usually four bytes). This *does* limit us to only 256 different
// block types, but in the scope of this project we'll never get anywhere near that many.
enum BlockType : unsigned char
{
    EMPTY, GRASS, DIRT, STONE, WATER, SNOW, LAVA, BEDROCK, SAND
};

// The six cardinal directions in 3D space
enum Direction : unsigned char
{
    XPOS, XNEG, YPOS, YNEG, ZPOS, ZNEG
};



// Lets us use any enum class as the key of a
// std::unordered_map
struct EnumHash {
    template <typename T>
    size_t operator()(T t) const {
        return static_cast<size_t>(t);
    }
};


const static std::unordered_map<BlockType, std::unordered_map<Direction, glm::vec2, EnumHash>, EnumHash> blockFaceUVs {
  {GRASS, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(3.f/16.f, 15.f/16.f)}, //right side of the grass
                                                             {XNEG, glm::vec2(3.f/16.f, 15.f/16.f)}, //left side of the grass
                                                             {YPOS, glm::vec2(8.f/16.f, 13.f/16.f)}, //top of grass
                                                             {YNEG, glm::vec2(2.f/16.f, 15.f/16.f)}, //bottom of grass
                                                             {ZPOS, glm::vec2(3.f/16.f, 15.f/16.f)}, //front side of grass
                                                             {ZNEG, glm::vec2(3.f/16.f, 15.f/16.f)}}}, //back side of grass

  {DIRT, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(2.f/16.f, 15.f/16.f)},
                                                            {XNEG, glm::vec2(2.f/16.f, 15.f/16.f)},
                                                            {YPOS, glm::vec2(2.f/16.f, 15.f/16.f)},
                                                            {YNEG, glm::vec2(2.f/16.f, 15.f/16.f)},
                                                            {ZPOS, glm::vec2(2.f/16.f, 15.f/16.f)},
                                                            {ZNEG, glm::vec2(2.f/16.f, 15.f/16.f)}}},

  {STONE, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(1.f/16.f, 15.f/16.f)},
                                                             {XNEG, glm::vec2(1.f/16.f, 15.f/16.f)},
                                                             {YPOS, glm::vec2(1.f/16.f, 15.f/16.f)},
                                                             {YNEG, glm::vec2(1.f/16.f, 15.f/16.f)},
                                                             {ZPOS, glm::vec2(1.f/16.f, 15.f/16.f)},
                                                             {ZNEG, glm::vec2(1.f/16.f, 15.f/16.f)}}},

  {LAVA, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(15.f/16.f, 1.f/16.f)},
                                                            {XNEG, glm::vec2(15.f/16.f, 1.f/16.f)},
                                                            {YPOS, glm::vec2(15.f/16.f, 1.f/16.f)},
                                                            {YNEG, glm::vec2(15.f/16.f, 1.f/16.f)},
                                                            {ZPOS, glm::vec2(15.f/16.f, 1.f/16.f)},
                                                            {ZNEG, glm::vec2(15.f/16.f, 1.f/16.f)}}},

  {WATER, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(15.f/16.f, 3.f/16.f)},
                                                             {XNEG, glm::vec2(15.f/16.f, 3.f/16.f)},
                                                             {YPOS, glm::vec2(15.f/16.f, 3.f/16.f)},
                                                             {YNEG, glm::vec2(15.f/16.f, 3.f/16.f)},
                                                             {ZPOS, glm::vec2(15.f/16.f, 3.f/16.f)},
                                                             {ZNEG, glm::vec2(15.f/16.f, 3.f/16.f)}}},

  {SNOW, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(2.f/16.f, 11.f/16.f)},
                                                            {XNEG, glm::vec2(2.f/16.f, 11.f/16.f)},
                                                            {YPOS, glm::vec2(2.f/16.f, 11.f/16.f)},
                                                            {YNEG, glm::vec2(2.f/16.f, 11.f/16.f)},
                                                            {ZPOS, glm::vec2(2.f/16.f, 11.f/16.f)},
                                                            {ZNEG, glm::vec2(2.f/16.f, 11.f/16.f)}}},

  {SAND, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(2.f/16.f, 14.f/16.f)},
                                                            {XNEG, glm::vec2(2.f/16.f, 14.f/16.f)},
                                                            {YPOS, glm::vec2(2.f/16.f, 14.f/16.f)},
                                                            {YNEG, glm::vec2(2.f/16.f, 14.f/16.f)},
                                                            {ZPOS, glm::vec2(2.f/16.f, 14.f/16.f)},
                                                            {ZNEG, glm::vec2(2.f/16.f, 14.f/16.f)}}},

  {BEDROCK, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(1.f/16.f, 14.f/16.f)},
                                                               {XNEG, glm::vec2(1.f/16.f, 14.f/16.f)},
                                                               {YPOS, glm::vec2(1.f/16.f, 14.f/16.f)},
                                                               {YNEG, glm::vec2(1.f/16.f, 14.f/16.f)},
                                                               {ZPOS, glm::vec2(1.f/16.f, 14.f/16.f)},
                                                               {ZNEG, glm::vec2(1.f/16.f, 14.f/16.f)}}},
};

class Chunk;

struct ChunkVBOData {
    Chunk* chunk;
    std::vector<GLfloat> vboDataOpaque, vboDataTransparent;
    std::vector<GLuint> idxDataOpaque, idxDataTransparent;

    ChunkVBOData(Chunk *c) : chunk(c),
        vboDataOpaque{}, vboDataTransparent{},
        idxDataOpaque{}, idxDataTransparent{}
    {}
};

// One Chunk is a 16 x 256 x 16 section of the world,
// containing all the Minecraft blocks in that area.
// We divide the world into Chunks in order to make
// recomputing its VBO data faster by not having to
// render all the world at once, while also not having
// to render the world block by block.

class Chunk : public Drawable {
private:
    // All of the blocks contained within this Chunk
    std::array<BlockType, 65536> m_blocks;
    int minX, minZ;
    // This Chunk's four neighbors to the north, south, east, and west
    // The third input to this map just lets us use a Direction as
    // a key for this map.
    // These allow us to properly determine
    std::unordered_map<Direction, Chunk*, EnumHash> m_neighbors;

public:
    Chunk(OpenGLContext* context, int x, int z);
    BlockType getBlockAt(unsigned int x, unsigned int y, unsigned int z) const;
    BlockType getBlockAt(int x, int y, int z) const;
    void setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t);
    void linkNeighbor(uPtr<Chunk>& neighbor, Direction dir);
    void createVBOBuffer(std::vector<GLfloat>& vertexDataOpaque, std::vector<GLfloat>& vertexDataTransparent,
                         std::vector<GLuint> &idxDataOpaque, std::vector<GLuint> &idxDataTransparent);
    bool isBlockAnimateable(BlockType type);
    Direction convertToDirection(const glm::ivec3& dirVec);

    glm::vec2 calculateUV(BlockType current, glm::ivec3 direction);

    // Drawable interface
public:
    // fills `data` with vertex and index data necessary to create a VBO
    void createMultithreaded(ChunkVBOData &data);
    void createVBOdata() override;
    GLenum drawMode() override { return GL_TRIANGLES; }
    std::unordered_map<Direction, Chunk *, EnumHash>& neighbors();
    int getMinX() const;
    int getMinZ() const;
};
