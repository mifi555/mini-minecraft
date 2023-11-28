#include "terrain.h"
#include "blocktypeworker.h"
#include "vboworker.h"

#include <stdexcept>
#include <iostream>
#include <unordered_map>

#define TERRAIN_DRAW_RADIUS 1       // (1) 3x3 radius (terrain that is created and drawn)
#define TERRAIN_CREATE_RADIUS 2    // (2) 5x5 radius (terrain that is created)

// util

namespace TerrainConstants {

// NOTE: We are treating +Z as NORTH and +X as WEST

enum Direction {
    NORTH,
    SOUTH,
    EAST,
    WEST,
    NIL
};

// the range of our create radius
const static glm::ivec2 topLeft(TERRAIN_CREATE_RADIUS * 64, TERRAIN_CREATE_RADIUS * 64);
const static glm::ivec2 bottomLeft(TERRAIN_CREATE_RADIUS * 64, -TERRAIN_CREATE_RADIUS * 64);
const static glm::ivec2 topRight(-TERRAIN_CREATE_RADIUS * 64, TERRAIN_CREATE_RADIUS * 64);
const static glm::ivec2 bottomRight(-TERRAIN_CREATE_RADIUS * 64, -TERRAIN_CREATE_RADIUS * 64);

const static glm::ivec2 northVec(0, 64);
const static glm::ivec2 southVec(0, -64);
const static glm::ivec2 eastVec(-64, 0);
const static glm::ivec2 westVec(64, 0);

const static std::unordered_map<Direction, Direction> direction_to_opposite = {
    { NORTH, SOUTH },
    { SOUTH, NORTH },
    { EAST,  WEST },
    { WEST,  EAST }
};

const static std::unordered_map<Direction, std::pair<glm::ivec2, glm::ivec2>> direction_to_range = {
    { NORTH, {topRight, topLeft} },
    { SOUTH, {bottomRight, bottomLeft} },
    { EAST,  {bottomRight, topRight} },
    { WEST,  {bottomLeft, topLeft} }
};

}

// get terrain zone at specified position
glm::ivec2 terrainAtXZPos(glm::vec2 p) {
    return glm::ivec2(64 * glm::ivec2(glm::floor(p / 64.f)));
}

Terrain::Terrain(OpenGLContext *context)
    : m_chunks(),
    m_generatedTerrain(),
    mp_context(context),
    m_chunksThatHaveBlockData(),
    m_chunksThatHaveBlockDataLock(),
    m_chunksThatHaveVBOs(),
    m_chunksThatHaveVBOsLock()
{}

Terrain::~Terrain() {
}

// Combine two 32-bit ints into one 64-bit int
// where the upper 32 bits are X and the lower 32 bits are Z
int64_t toKey(int x, int z) {
    int64_t xz = 0xffffffffffffffff;
    int64_t x64 = x;
    int64_t z64 = z;

    // Set all lower 32 bits to 1 so we can & with Z later
    xz = (xz & (x64 << 32)) | 0x00000000ffffffff;

    // Set all upper 32 bits to 1 so we can & with XZ
    z64 = z64 | 0xffffffff00000000;

    // Combine
    xz = xz & z64;
    return xz;
}

glm::ivec2 toCoords(int64_t k) {
    // Z is lower 32 bits
    int64_t z = k & 0x00000000ffffffff;
    // If the most significant bit of Z is 1, then it's a negative number
    // so we have to set all the upper 32 bits to 1.
    // Note the 8    V
    if(z & 0x0000000080000000) {
        z = z | 0xffffffff00000000;
    }
    int64_t x = (k >> 32);

    return glm::ivec2(x, z);
}

// Surround calls to this with try-catch if you don't know whether
// the coordinates at x, y, z have a corresponding Chunk
BlockType Terrain::getBlockAt(int x, int y, int z) const
{
    if(hasChunkAt(x, z)) {
        // Just disallow action below or above min/max height,
        // but don't crash the game over it.
        if(y < 0 || y >= 256) {
            return EMPTY;
        }
        const uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        return c->getBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                             static_cast<unsigned int>(y),
                             static_cast<unsigned int>(z - chunkOrigin.y));
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
}

BlockType Terrain::getBlockAt(glm::vec3 p) const {
    return getBlockAt(p.x, p.y, p.z);
}

bool Terrain::hasChunkAt(int x, int z) const {
    // Map x and z to their nearest Chunk corner
    // By flooring x and z, then multiplying by 16,
    // we clamp (x, z) to its nearest Chunk-space corner,
    // then scale back to a world-space location.
    // Note that floor() lets us handle negative numbers
    // correctly, as floor(-1 / 16.f) gives us -1, as
    // opposed to (int)(-1 / 16.f) giving us 0 (incorrect!).
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks.find(toKey(16 * xFloor, 16 * zFloor)) != m_chunks.end();
}

uPtr<Chunk>& Terrain::getChunkAt(int x, int z) {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks[toKey(16 * xFloor, 16 * zFloor)];
}


const uPtr<Chunk>& Terrain::getChunkAt(int x, int z) const {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks.at(toKey(16 * xFloor, 16 * zFloor));
}

void Terrain::setBlockAt(int x, int y, int z, BlockType t)
{
    if(hasChunkAt(x, z)) {
        uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        c->setBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                      static_cast<unsigned int>(y),
                      static_cast<unsigned int>(z - chunkOrigin.y),
                      t);
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
}

void Terrain::multithreadedWork(glm::vec3 playerPosition, glm::vec3 playerPreviousPosition)
{
    tryExpansion(playerPosition, playerPreviousPosition);
    checkThreadResults();
}

void Terrain::tryExpansion(glm::vec3 pos, glm::vec3 posPrev)
{   
    glm::ivec2 currentZone = terrainAtXZPos(glm::vec2(pos.x, pos.z));
    glm::ivec2 prevZone    = terrainAtXZPos(glm::vec2(posPrev.x, posPrev.z));

    if (currentZone == prevZone) {
        return;
    }

    // if the player is stepping into a new zone, we need to spawn threads to create new zones / create VBO data

    TerrainConstants::Direction direction = TerrainConstants::NIL;

    {
        glm::ivec2 dir = currentZone - prevZone;

        if (dir == TerrainConstants::northVec) {
            direction = TerrainConstants::NORTH;
        } else if (dir == TerrainConstants::southVec) {
            direction = TerrainConstants::SOUTH;
        } else if (dir == TerrainConstants::eastVec) {
            direction = TerrainConstants::EAST;
        } else if (dir == TerrainConstants::westVec) {
            direction = TerrainConstants::WEST;
        } else {
            qCritical() << "Could not discern direction:" << glm::to_string(dir).c_str();
            return;
        }
    }

    // destroy vbo data of zones that are outside of our radius (the zones in the opposite direction of player)
    {
        TerrainConstants::Direction oppositeDirection = TerrainConstants::direction_to_opposite.at(direction);
        glm::ivec2 min = prevZone + TerrainConstants::direction_to_range.at(oppositeDirection).first;
        glm::ivec2 max = prevZone + TerrainConstants::direction_to_range.at(oppositeDirection).second;

        for (int x = min.x; x <= max.x; x += 64) {
            for (int z = min.y; z <= max.y; z += 64) {
                auto it = m_generatedTerrain.find(toKey(x, z));
                if (it != m_generatedTerrain.end()) {
                    destroyVBOsAtTerrain(x, z);
                }
            }
        }
    }

    glm::ivec2 min = currentZone + TerrainConstants::direction_to_range.at(direction).first;
    glm::ivec2 max = currentZone + TerrainConstants::direction_to_range.at(direction).second;

    // loop through create radius and check for uncreated zones or zones with chunks without vbo data
    for (int x = min.x; x <= max.x; x += 64) {
        for (int z = min.y; z <= max.y; z += 64) {
            if (m_generatedTerrain.find(toKey(x, z)) == m_generatedTerrain.end()) {
                m_generatedTerrain.insert(toKey(x, z));
                std::vector<Chunk*> chunksToFill;

                // fill chunksToFill with new chunks of terrain
                instantiateChunksAtTerrain(x, z, chunksToFill);

                // spawn blocktype workers
                BlockTypeWorker* worker = new BlockTypeWorker(
                    x, z, chunksToFill, &m_chunksThatHaveBlockData, &m_chunksThatHaveBlockDataLock);
                QThreadPool::globalInstance()->start(worker);
            } else {
                // create VBO data for this terrain zone if there isn't already
                std::vector<Chunk*> chunksWithoutVBO;

                // check for chunks that don't have VBO's for this terrain zone
                checkForChunksWithoutVBOs(x, z, chunksWithoutVBO);

                // spawn VBO workers
                for (Chunk* &c : chunksWithoutVBO) {
                    VBOWorker* worker = new VBOWorker(c, &m_chunksThatHaveVBOs, &m_chunksThatHaveVBOsLock);
                    QThreadPool::globalInstance()->start(worker);
                }
            }
        }
    }
}

void Terrain::checkThreadResults()
{
    m_chunksThatHaveBlockDataLock.lock();
    // spawn VBO workers for chunks
    for (Chunk* c : m_chunksThatHaveBlockData) {
        VBOWorker* worker = new VBOWorker(c, &m_chunksThatHaveVBOs, &m_chunksThatHaveVBOsLock);
        QThreadPool::globalInstance()->start(worker);
    }
    // clear chunks list
    m_chunksThatHaveBlockData.clear();
    m_chunksThatHaveBlockDataLock.unlock();

    m_chunksThatHaveVBOsLock.lock();
    // loop through VBO data and send vbo data to GPU
    for (ChunkVBOData& vbo : m_chunksThatHaveVBOs) {
        // TODO:
        vbo.chunk->createVBOBuffer(vbo.vboDataOpaque, vbo.vboDataTransparent, vbo.idxDataOpaque, vbo.idxDataTransparent);
    }
    // clear vbo list
    m_chunksThatHaveVBOs.clear();
    m_chunksThatHaveVBOsLock.unlock();
}

void Terrain::instantiateChunksAtTerrain(int terrainX, int terrainZ, std::vector<Chunk *> &chunksToFill)
{
    // instantiate 4x4 chunks starting from terrain origin terrainX, terrainZ
    for (int x = terrainX; x < terrainX + 4 * 16; x += 16) {
        for (int z = terrainZ; z < terrainZ + 4 * 16; z += 16) {
            chunksToFill.push_back(instantiateChunkAt(x, z));
        }
    }
}

void Terrain::checkForChunksWithoutVBOs(int terrainX, int terrainZ, std::vector<Chunk *> &chunksWithoutVBO)
{
    // check 4x4 chunks starting from terrain origin terrainX, terrainZ
    for (int x = terrainX; x < terrainX + 4 * 16; x += 16) {
        for (int z = terrainZ; z < terrainZ + 4 * 16; z += 16) {
            if (hasChunkAt(x, z)) {
                uPtr<Chunk> &chunk = getChunkAt(x, z);
                if (chunk->elemCount() == -1) {
                    chunksWithoutVBO.push_back(chunk.get());
                }
            } else {
                qCritical() << "No chunk found at this terrain zone. Something is wrong.";
                std::exit(-1);
            }
        }
    }
}

void Terrain::destroyVBOsAtTerrain(int terrainX, int terrainZ) {
    // check 4x4 chunks starting from terrain origin terrainX, terrainZ
    for (int x = terrainX; x < terrainX + 4 * 16; x += 16) {
        for (int z = terrainZ; z < terrainZ + 4 * 16; z += 16) {
            if (hasChunkAt(x, z)) {
                uPtr<Chunk> &chunk = getChunkAt(x, z);
                if (chunk->elemCount() != -1) {
                    chunk->destroyVBOdata();
                } else {
                    qWarning() << "No VBO data for deletion found for this chunk. Something wrong?";
                }
            } else {
                qCritical() << "No chunk found at this terrain zone. Something is wrong.";
                std::exit(-1);
            }
        }
    }
}

void Terrain::generateChunkTerrain(Chunk* chunk) {
    int minX, minZ;

    minX = chunk->getMinX();
    minZ = chunk->getMinZ();

    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {

            glm::vec2 worldPos = glm::vec2(minX + x, minZ + z);

            float grass = grasslandsYValue(glm::vec2(worldPos[0], worldPos[1]));
            float mountains = mountainsYValue(glm::vec2(worldPos[0], worldPos[1]));
            float t = biomeBlender(glm::vec2(minX + x, minZ + z));
            t = glm::smoothstep(0.6f, 0.4f, t);
            int yMax = glm::mix(grass, mountains, t);

            yMax = int(glm::clamp(float(yMax), 0.f, 255.f));

            for (int y = 0; y < yMax + 1; y++) {
                // Bedrock layer.
                if (y == 0) {
                    chunk->setBlockAt(x, y, z, BEDROCK);
                    // Lava flow - mountains biome.
                } else if ((yMax == 160 && y == 160) || (yMax == 175 && y == 175) || (yMax == 190 && y == 190)) {
                    chunk->setBlockAt(x, y, z, LAVA);
                    // Stone layer.
                } else if (y <= 128) {
                    chunk->setBlockAt(x, y, z, STONE);
                    // Water pools.
                } else if (y > 128 && yMax <= 133) {
                    // Sand layer.
                    chunk->setBlockAt(x, y, z, SAND);

                    if (y == yMax) {
                        for (int i = y + 1; i <= 134; i++) {
                            chunk->setBlockAt(x, i, z, WATER);
                        }
                    }
                    // Dirt layer - grass biome and pools.
                } else if (y > 128 && yMax <= 150 && y < yMax) {
                    chunk->setBlockAt(x, y, z, DIRT);
                    // Grass layer.
                } else if (y > 128 && yMax <= 150 && y == yMax) {
                    chunk->setBlockAt(x, y, z, GRASS);
                    // Snow layer - mountains biome.
                } else if (y > 200 && y == yMax) {
                    chunk->setBlockAt(x, y, z, SNOW);
                    // Mountains layer.
                } else if (y > 128 && yMax > 150) {
                    chunk->setBlockAt(x, y, z, STONE);
                }
            }

            // Cave generation.
            for (int y = 1; y <= 128; y++) {
                float noise3D = perlinNoise3D(glm::vec3(worldPos[0], y, worldPos[1]));

                if (noise3D < 0.f) {
                    // Lava layer.
                    if (y < 25) {
                        chunk->setBlockAt(x, y, z, LAVA);
                        // Set empty for cave section.
                    } else {
                        chunk->setBlockAt(x, y, z, EMPTY);
                    }
                }
            }
        }
    }
}

Chunk* Terrain::instantiateChunkAt(int x, int z) {
    uPtr<Chunk> chunk = mkU<Chunk>(this->mp_context, x, z);
    Chunk *cPtr = chunk.get();

    m_chunks[toKey(x, z)] = std::move(chunk);
    // Set the neighbor pointers of itself and its neighbors
    if(hasChunkAt(x, z + 16)) {
        auto &chunkNorth = m_chunks[toKey(x, z + 16)];
        cPtr->linkNeighbor(chunkNorth, ZPOS);
    }
    if(hasChunkAt(x, z - 16)) {
        auto &chunkSouth = m_chunks[toKey(x, z - 16)];
        cPtr->linkNeighbor(chunkSouth, ZNEG);
    }
    if(hasChunkAt(x + 16, z)) {
        auto &chunkEast = m_chunks[toKey(x + 16, z)];
        cPtr->linkNeighbor(chunkEast, XPOS);
    }
    if(hasChunkAt(x - 16, z)) {
        auto &chunkWest = m_chunks[toKey(x - 16, z)];
        cPtr->linkNeighbor(chunkWest, XNEG);
    }

    return cPtr;
}

void Terrain::drawTerrainZone(int minX, int minZ, ShaderProgram *shaderProgram) {
    int maxX = minX + 64;
    int maxZ = minZ + 64;

    for(int x = minX; x < maxX; x += 16) {
        for(int z = minZ; z < maxZ; z += 16) {
            if (hasChunkAt(x, z)) {
                const uPtr<Chunk> &chunk = getChunkAt(x, z);
                if (chunk->elemCount() != -1) {
                    shaderProgram->drawInterleaved(*chunk);
                }
            }
        }
    }

    // TODO:
    for(int x = minX; x < maxX; x += 16) {
        for(int z = minZ; z < maxZ; z += 16) {
            if (hasChunkAt(x, z)) {
                const uPtr<Chunk> &chunk = getChunkAt(x, z);
                if (chunk->elemCountTransparent() != -1) {
                    shaderProgram->drawInterleavedTransparent(*chunk);
                }
            }
        }
    }
}

void Terrain::draw(glm::vec3 playerPosition, ShaderProgram *shaderProgram) {
    glm::ivec2 currentZone = terrainAtXZPos(glm::vec2(playerPosition.x, playerPosition.z));

    int radius = 64 * TERRAIN_DRAW_RADIUS;
    glm::ivec2 min = currentZone - radius;
    glm::ivec2 max = currentZone + radius;

    for (int x = min.x; x <= max.x; x += 64) {
        for (int z = min.y; z <= max.y; z += 64) {
            drawTerrainZone(x, z, shaderProgram);
        }
    }
}

void Terrain::initializeTerrain()
{
    int radius = 64 * TERRAIN_CREATE_RADIUS;
    glm::ivec2 min = glm::ivec2(0, 0) - radius;
    glm::ivec2 max = glm::ivec2(0, 0) + radius;

    for (int x = min.x; x <= max.x; x += 64) {
        for (int z = min.y; z <= max.y; z += 64) {
            m_generatedTerrain.insert(toKey(x, z));
            std::vector<Chunk*> chunksToFill;

            // fill chunksToFill with new chunks of terrain
            instantiateChunksAtTerrain(x, z, chunksToFill);

            // spawn blocktype workers
            BlockTypeWorker* worker = new BlockTypeWorker(
                x, z, chunksToFill, &m_chunksThatHaveBlockData, &m_chunksThatHaveBlockDataLock);
            QThreadPool::globalInstance()->start(worker);
        }
    }
}


// Various noise functions used for terrain biome generation.
glm::vec2 Terrain::smoothF(glm::vec2 coords) {
    return coords * coords * (3.f - 2.f * coords);
}

float Terrain::noise(glm::vec2 coords) {
    const float k = 257.;
    glm::vec4 l  = glm::vec4(glm::floor(coords), glm::fract(coords));
    float u = l.x + l.y * k;
    glm::vec4 v  = glm::vec4(u, u + 1.f, u + k, u + k + 1.f);
    glm::vec2 newZW;

    v = glm::fract(glm::fract(float(1.23456789) * v) * v / float(0.987654321));
    newZW = smoothF(glm::vec2(l.z, l.w));
    l.z = newZW[0];
    l.w = newZW[1];
    l.x = glm::mix(v.x, v.y, l.z);
    l.y = glm::mix(v.z, v.w, l.z);

    return glm::mix(l.x, l.y, l.w);
}

float Terrain::fbm(glm::vec2 coords) {
    float a = 0.5;
    float f = 5.0;
    float n = 0.f;
    int it = 8;

    for(int i = 0; i < 32; i++)
    {
        if(i<it)
        {
            n += noise(coords * f) * a;
            a *= .5;
            f *= 2.;
        }
    }

    return n;
}

glm::vec2 Terrain::random2(glm::vec2 coords) {
    return glm::normalize(2.f * glm::fract(glm::sin(glm::vec2(glm::dot(coords, glm::vec2(127.1,311.7)), glm::dot(coords, glm::vec2(269.5,183.3)))) * float(43758.5453)) - 1.f);
}

glm::vec3 Terrain::random3(glm::vec3 coords) {
    float j = 4096.0*sin(glm::dot(coords , glm::vec3(17.0, 59.4, 15.0)));
    glm::vec3 r;
    r.z = glm::fract(512.0*j);
    j *= .125;
    r.x = glm::fract(512.0*j);
    j *= .125;
    r.y = glm::fract(512.0*j);
    return r-0.5f;
}

float Terrain::surflet(glm::vec2 p, glm::vec2 gridPoint) {
    // Compute falloff function by converting linear distance to a polynomial (quintic smootherstep function)
    float distX = abs(p.x - gridPoint.x);
    float distY = abs(p.y - gridPoint.y);
    float tX = 1 - 6 * pow(distX, 5.0) + 15 * pow(distX, 4.0) - 10 * pow(distX, 3.0);
    float tY = 1 - 6 * pow(distY, 5.0) + 15 * pow(distY, 4.0) - 10 * pow(distY, 3.0);

    // Get the random vector for the grid point
    glm::vec2 gradient = random2(gridPoint);
    // Get the vector from the grid point to P
    glm::vec2 diff = p - gridPoint;
    // Get the value of our height field by dotting grid->P with our gradient
    float height = glm::dot(diff, gradient);
    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * tX * tY;
}

float Terrain::perlinNoise(glm::vec2 coords) {
    // Tile the space
    glm::vec2 coordsXLYL = glm::floor(coords);
    glm::vec2 coordsXHYL = coordsXLYL + glm::vec2(1,0);
    glm::vec2 coordsXHYH = coordsXLYL + glm::vec2(1,1);
    glm::vec2 coordsXLYH = coordsXLYL + glm::vec2(0,1);

    return surflet(coords, coordsXLYL) + surflet(coords, coordsXHYL)
           + surflet(coords, coordsXHYH) + surflet(coords, coordsXLYH);
}

float Terrain::perlinNoise3D(glm::vec3 p)
{
    float surfletSum = 0.f;

    p[0] = p[0] * 0.03f;
    p[1] = p[1] * 0.07f;
    p[2] = p[2] * 0.03f;

    // Iterate over the four integer corners surrounding uv
    for(int dx = 0; dx <= 1; ++dx) {
        for(int dy = 0; dy <= 1; ++dy) {
            for(int dz = 0; dz <= 1; ++dz) {
                surfletSum += surflet3D(p, glm::floor(p) + glm::vec3(dx, dy, dz));
            }
        }
    }

    return surfletSum;
}

float Terrain::surflet3D(glm::vec3 p, glm::vec3 gridPoint)
{
    // Compute the distance between p and the grid point along each axis, and warp it with a
    // quintic function so we can smooth our cells
    glm::vec3 t2 = glm::abs(p - gridPoint);
    glm::vec3 t = glm::vec3(1.f) -
                  6.f * glm::vec3(pow(t2[0], 5.f), pow(t2[1], 5.f), pow(t2[2], 5.f)) +
                  15.f * glm::vec3(pow(t2[0], 4.f), pow(t2[1], 4.f), pow(t2[2], 4.f)) -
                  10.f * glm::vec3(pow(t2[0], 3.f), pow(t2[1], 3.f), pow(t2[2], 3.f));
    // Get the random vector for the grid point (assume we wrote a function random2
    // that returns a vec2 in the range [0, 1])
    glm::vec3 gradient = random3(gridPoint) * 2.f - glm::vec3(1.f, 1.f, 1.f);
    // Get the vector from the grid point to P
    glm::vec3 diff = p - gridPoint;
    // Get the value of our height field by dotting grid->P with our gradient
    float height = glm::dot(diff, gradient);
    // Scale our height field (i.e. reduce it) by our polynomial falloff function

    return height * t.x * t.y * t.z;
}

float Terrain::worleyNoise(glm::vec2 coords) {
    // Tile the space
    glm::vec2 coordInt = glm::floor(coords);
    glm::vec2 coordFract = glm::fract(coords);
    float minDist = 1.0; // Minimum distance initialized to max.
    float secondMinDist = 1.0;
    glm::vec2 closestPoint;

    // Search all neighboring cells and this cell for their point
    for(int y = -1; y <= 1; y++)
    {
        for(int x = -1; x <= 1; x++)
        {
            glm::vec2 neighbor = glm::vec2(float(x), float(y));

            // Random point inside current neighboring cell
            glm::vec2 point = random2(coordInt + neighbor);

            // Compute the distance b/t the point and the fragment
            // Store the min dist thus far
            glm::vec2 diff = neighbor + point - coordFract;
            float dist = glm::length(diff);

            if(dist < minDist) {
                secondMinDist = minDist;
                minDist = dist;
                closestPoint = point;
            }
            else if(dist < secondMinDist) {
                secondMinDist = dist;
            }
        }
    }

    float height = glm::length(closestPoint);

    return height;
}

float Terrain::grasslandsYValue(glm::vec2 coords) {
    float h = 0, amp = 0.5, freq = 95, yValue = 1;

    // Noise based height generation.
    for(int i = 0; i < 4; ++i) {
        glm::vec2 offset = glm::vec2(fbm(coords / 256.f), fbm(coords / 300.f) + 1000);
        float h1 = (perlinNoise((coords + offset * 25.f) / freq) * perlinNoise((coords + offset * 25.f) / freq));

        h += h1 * amp;
        amp *= 0.5;
        freq *= 0.5;
    }

    yValue = floor(124 + h * 200);

    // Enforce height bounds.
    yValue = glm::clamp(yValue, 129.f, 255.f);

    return yValue;
}

float Terrain::mountainsYValue(glm::vec2 coords) {
    float h = 0, amp = 0.5, freq = 128, yValue = 1;

    // Noise based height generation.
    for(int i = 0; i < 4; ++i) {
        glm::vec2 offset = glm::vec2(fbm(coords / 256.f), fbm(coords / 300.f) + 1000);
        float h1 = tan(perlinNoise((coords + offset * 45.f) / freq)) * perlinNoise((coords + offset * 45.f) / freq) * 5;

        h += h1 * amp;
        amp *= 0.5;
        freq *= 0.5;
    }

    yValue = floor((151 + h * 160));

    // Enforce height bounds.
    yValue = glm::clamp(yValue, 151.f, 255.f);

    return yValue;
}

float Terrain::biomeBlender(glm::vec2 coords) {
    return 0.5 * (perlinNoise(coords / 364.f) + 1.f);
}
