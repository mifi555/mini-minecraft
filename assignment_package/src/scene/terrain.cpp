#include "terrain.h"
#include "cube.h"
#include <stdexcept>
#include <iostream>

bool started = false;

Terrain::Terrain(OpenGLContext *context)
    : m_chunks(), m_generatedTerrain(), m_geomCube(context), mp_context(context)
{}

Terrain::~Terrain() {
    m_geomCube.destroyVBOdata();
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

Chunk* Terrain::instantiateChunkAt(int x, int z) {
    uPtr<Chunk> chunk = mkU<Chunk>(this->mp_context, x, z);
    Chunk *cPtr = chunk.get();
    m_chunks[toKey(x, z)] = move(chunk);
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

// TODO: When you make Chunk inherit from Drawable, change this code so
// it draws each Chunk with the given ShaderProgram, remembering to set the
// model matrix to the proper X and Z translation!
void Terrain::draw(int minX, int maxX, int minZ, int maxZ, ShaderProgram *shaderProgram) {
    //m_geomCube.clearOffsetBuf();
    //m_geomCube.clearColorBuf();

    //std::vector<glm::vec3> offsets, colors;

    for(int x = minX; x < maxX; x += 16) {
        for(int z = minZ; z < maxZ; z += 16) {
            const uPtr<Chunk> &chunk = getChunkAt(x, z);
            shaderProgram->setModelMatrix(glm::translate(glm::mat4(), glm::vec3(x, 0, z)));
            shaderProgram->drawInterleaved(*chunk);
        }
    }


    //m_geomCube.createInstancedVBOdata(offsets, colors);
    //shaderProgram->drawInstanced(m_geomCube);
}

void Terrain::CreateTestScene()
{
    // TODO: DELETE THIS LINE WHEN YOU DELETE m_geomCube!
    m_geomCube.createVBOdata();

    std::vector<Chunk*> chunks;

    // Create the Chunks that will
    // store the blocks for our
    // initial world space
    for(int x = 0; x < 64; x += 16) {
        for(int z = 0; z < 64; z += 16) {
            Chunk* c = instantiateChunkAt(x, z);
            chunks.push_back(c);
        }
    }
    // Tell our existing terrain set that
    // the "generated terrain zone" at (0,0)
    // now exists.
    m_generatedTerrain.insert(toKey(0, 0));

    // Create the basic terrain floor
    for(int x = 0; x < 64; ++x) {
        for(int z = 0; z < 64; ++z) {
            if((x + z) % 2 == 0) {
                setBlockAt(x, 128, z, STONE);
            }
            else {
                setBlockAt(x, 128, z, DIRT);
            }
        }
    }
    // Add "walls" for collision testing
    for(int x = 0; x < 64; ++x) {
        setBlockAt(x, 129, 0, GRASS);
        setBlockAt(x, 130, 0, GRASS);
        setBlockAt(x, 129, 63, GRASS);
        setBlockAt(0, 130, x, GRASS);
    }
    // Add a central column
    for(int y = 129; y < 140; ++y) {
        setBlockAt(32, y, 32, GRASS);
    }

    // create vbo data for newly created chunks
    for (auto &c : chunks) {
        c->createVBOdata();
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

float Terrain::surflet(glm::vec2 point, glm::vec2 gridPoint) {
    // Compute falloff function by converting linear distance to a polynomial (quintic smootherstep function)
    float distX = abs(point.x - gridPoint.x);
    float distY = abs(point.y - gridPoint.y);
    float tX = 1 - 6 * pow(distX, 5.0) + 15 * pow(distX, 4.0) - 10 * pow(distX, 3.0);
    float tY = 1 - 6 * pow(distY, 5.0) + 15 * pow(distY, 4.0) - 10 * pow(distY, 3.0);

    // Get the random vector for the grid point
    glm::vec2 gradient = random2(gridPoint);
    // Get the vector from the grid point to P
    glm::vec2 diff = point - gridPoint;
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

    return surflet(coords, coordsXLYL) + surflet(coords, coordsXHYL) + surflet(coords, coordsXHYH) + surflet(coords, coordsXLYH);
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

    float height = 0.5 * minDist + 0.5 * secondMinDist;
    height = glm::length(closestPoint);

    return height;
}

float Terrain::grasslandsYValue(glm::vec2 coords, glm::vec3 offsetInstanced) {
    //// Remove this offsetPos if unnecessary.
    ///
    glm::vec4 offsetPos = glm::vec4(coords[0], 1, coords[1], 1) + glm::vec4(offsetInstanced, 0);
    glm::vec2 xz = glm::vec2(offsetInstanced[0], offsetInstanced[2]);
    float h = 0, amp = 0.5, freq = 128, yValue = 1;

    for(int i = 0; i < 4; ++i) {
        glm::vec2 offset = glm::vec2(fbm(xz / 256.f), fbm(xz / 300.f) + 1000);
        float h1 = (perlinNoise((xz + offset * 25.f) / freq) * perlinNoise((xz + offset * 25.f) / freq));

        h += h1 * amp;
        amp *= 0.5;
        freq *= 0.5;
    }

    offsetPos.y *= floor(128 + h * 200);
    yValue = floor(125 + h * 200);

    // Enforce min/max bounds.
    if (yValue < 129.f) {
        yValue = 129.f;
    } else if (yValue > 150.f) {
        yValue = 150.f;
    }

    return yValue;
}

float Terrain::mountainsYValue(glm::vec2 coords, glm::vec3 offsetInstanced) {
    //// Remove this offsetPos if unnecessary.
    ///
    glm::vec4 offsetPos = glm::vec4(coords[0], 1, coords[1], 1) + glm::vec4(offsetInstanced, 0);
    glm::vec2 xz = glm::vec2(offsetInstanced[0], offsetInstanced[2]);
    float h = 0, amp = 0.5, freq = 128, yValue = 1;

    for(int i = 0; i < 4; ++i) {
        glm::vec2 offset = glm::vec2(fbm(xz / 256.f), fbm(xz / 300.f) + 1000);
        float h1 = sin(perlinNoise((xz + offset * 30.f) / freq)) * offset[0] * offset[1] * 0.005;

        h += h1 * amp;
        amp *= 0.5;
        freq *= 0.5;
    }

    offsetPos.y *= floor(180 + h * 200);
    yValue = floor((197 + h * 160));

    return yValue;
}

float Terrain::biomeBlender(glm::vec2 coords) {
    return 0.5 * (perlinNoise(coords / 512.f) + 1.f);
}
