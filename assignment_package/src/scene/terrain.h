#pragma once

#include <QThreadPool>
#include <QMutex>

#include "smartpointerhelp.h"
#include "glm_includes.h"
#include "chunk.h"
#include <array>
#include <unordered_map>
#include <unordered_set>
#include "shaderprogram.h"


//using namespace std;

// Helper functions to convert (x, z) to and from hash map key
int64_t toKey(int x, int z);
glm::ivec2 toCoords(int64_t k);

// The container class for all of the Chunks in the game.
// Ultimately, while Terrain will always store all Chunks,
// not all Chunks will be drawn at any given time as the world
// expands.
class Terrain {
private:
    // Stores every Chunk according to the location of its lower-left corner
    // in world space.
    // We combine the X and Z coordinates of the Chunk's corner into one 64-bit int
    // so that we can use them as a key for the map, as objects like std::pairs or
    // glm::ivec2s are not hashable by default, so they cannot be used as keys.
    std::unordered_map<int64_t, uPtr<Chunk>> m_chunks;

    // We will designate every 64 x 64 area of the world's x-z plane
    // as one "terrain generation zone". Every time the player moves
    // near a portion of the world that has not yet been generated
    // (i.e. its lower-left coordinates are not in this set), a new
    // 4 x 4 collection of Chunks is created to represent that area
    // of the world.
    // The world that exists when the base code is run consists of exactly
    // one 64 x 64 area with its lower-left corner at (0, 0).
    // When milestone 1 has been implemented, the Player can move around the
    // world to add more "terrain generation zone" IDs to this set.
    // While only the 3 x 3 collection of terrain generation zones
    // surrounding the Player should be rendered, the Chunks
    // in the Terrain will never be deleted until the program is terminated.
    std::unordered_set<int64_t> m_generatedTerrain;

    OpenGLContext* mp_context;

    // shared resources for multithreading
    std::unordered_set<Chunk*> m_chunksThatHaveBlockData;
    QMutex m_chunksThatHaveBlockDataLock;
    std::vector<ChunkVBOData> m_chunksThatHaveVBOs;
    QMutex m_chunksThatHaveVBOsLock;

    void tryExpansion(glm::vec3 pos, glm::vec3 posPrev);
    void checkThreadResults();
    void instantiateChunksAtTerrain(int x, int z, std::vector<Chunk *>& chunksToFill);
    void checkForChunksWithoutVBOs(int x, int z, std::vector<Chunk *>& chunksWithoutVBO);
    void destroyVBOsAtTerrain(int terrainX, int terrainZ);
    void drawTerrainZone(int minX, int minZ, ShaderProgram *shaderProgram);
public:
    Terrain(OpenGLContext *context);
    ~Terrain();

    // Instantiates a new Chunk and stores it in
    // our chunk map at the given coordinates.
    // Returns a pointer to the created Chunk.
    Chunk* instantiateChunkAt(int x, int z);
    // Do these world-space coordinates lie within
    // a Chunk that exists?
    bool hasChunkAt(int x, int z) const;
    // Assuming a Chunk exists at these coords,
    // return a mutable reference to it
    uPtr<Chunk>& getChunkAt(int x, int z);
    // Assuming a Chunk exists at these coords,
    // return a const reference to it
    const uPtr<Chunk>& getChunkAt(int x, int z) const;
    // Given a world-space coordinate (which may have negative
    // values) return the block stored at that point in space.
    BlockType getBlockAt(int x, int y, int z) const;
    BlockType getBlockAt(glm::vec3 p) const;
    // Given a world-space coordinate (which may have negative
    // values) set the block at that point in space to the
    // given type.
    void setBlockAt(int x, int y, int z, BlockType t);

    void multithreadedWork(glm::vec3 playerPosition, glm::vec3 playerPreviousPosition);

    // Draws every Chunk that falls within the bounding box
    // described by the min and max coords, using the provided
    // ShaderProgram
    void draw(glm::vec3 playerPosition, ShaderProgram *shaderProgram);

    // creates and draws terrain zones at player spawn
    void initializeTerrain();

    // generate terrain block data
    static void generateChunkTerrain(Chunk *chunk);

    // Various noise functions and helpers used for terrain biome generation.

    static glm::vec2 smoothF(glm::vec2 coords);
    static float noise(glm::vec2 coords);
    static float fbm(glm::vec2 coords);
    static glm::vec2 random2(glm::vec2 coords);
    static glm::vec3 random3(glm::vec3 coords);
    static float surflet(glm::vec2 point, glm::vec2 gridPoint);
    static float perlinNoise(glm::vec2 coords);

    static float perlinNoise3D(glm::vec3 p);  // TODO:
    static float surflet3D(glm::vec3 p, glm::vec3 gridPoint); // TODO:

    static float worleyNoise(glm::vec2 coords);

    // terrain generators

    static float grasslandsYValue(glm::vec2 coords);
    static float mountainsYValue(glm::vec2 coords);
    static float biomeBlender(glm::vec2 coords);
};
