#ifndef CHUNKCONSTANTS_H
#define CHUNKCONSTANTS_H

#include <scene/chunk.h>
#include <array>
#include <glm/glm.hpp>

namespace ChunkConstants {

    constexpr int VERT_COUNT = 4;

    using Vec4Array = std::array<glm::vec4, ChunkConstants::VERT_COUNT>;

    const static Vec4Array RightFace = {
        glm::vec4(1.0f, 1.0f, 0.0f, 0.0f),  // UR
        glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),  // LR
        glm::vec4(1.0f, 0.0f, 1.0f, 0.0f),  // LL
        glm::vec4(1.0f, 1.0f, 1.0f, 0.0f)   // UL
    };

    const static Vec4Array LeftFace = {
        glm::vec4(0.0f, 1.0f, 1.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)
    };

    const static Vec4Array FrontFace = {
        glm::vec4(1.0f, 1.0f, 1.0f, 0.0f),
        glm::vec4(1.0f, 0.0f, 1.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
        glm::vec4(0.0f, 1.0f, 1.0f, 0.0f)
    };

    const static Vec4Array BackFace = {
        glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 0.0f, 0.0f),
        glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec4(1.0f, 1.0f, 0.0f, 0.0f)
    };

    const static Vec4Array TopFace = {
        glm::vec4(1.0f, 1.0f, 0.0f, 0.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 0.0f),
        glm::vec4(0.0f, 1.0f, 1.0f, 0.0f),
        glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)
    };

    const static Vec4Array BottomFace = {
        glm::vec4(1.0f, 0.0f, 1.0f, 0.0f),
        glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)
    };

    // for use in createVBOdata()
    struct BlockFace {
        glm::ivec3 direction;
        std::array<glm::vec4, ChunkConstants::VERT_COUNT> pos;
        glm::vec4 nor;
    };

    const static std::array<BlockFace, 6> neighbouringFaces = { {
        { glm::ivec3( 1, 0, 0), ChunkConstants::RightFace,  glm::vec4(1,0,0,0) },                   // xpos  right face
        { glm::ivec3(-1, 0, 0), ChunkConstants::LeftFace,   glm::vec4(-1,0,0,0)  },                 // xneg  left face
        { glm::ivec3(0,  1, 0), ChunkConstants::TopFace,    glm::vec4(0,1,0,0) },                   // ypos  top face
        { glm::ivec3(0, -1, 0), ChunkConstants::BottomFace, glm::vec4(0,-1,0,0)  },                 // yneg  bottom face
        { glm::ivec3(0, 0,  1), ChunkConstants::FrontFace,  glm::vec4(0,0,1,0) },                   // zpos  front face
        { glm::ivec3(0, 0, -1), ChunkConstants::BackFace,   glm::vec4(0,0,-1,0)  }                  // zneg  back face
    }};

    const static std::unordered_map<BlockType, glm::vec4> blocktype_to_color {
            { GRASS, glm::vec4(0.0431f, 0.51373f, 0.23137f, 1.f) },
            { DIRT,  glm::vec4(0.5373f, 0.3176f, 0.0392f, 1.f) },
            { STONE, glm::vec4(0.27f, 0.3568f, 0.3804f, 1.f) },
            { WATER, glm::vec4(0.04706f, 0.3647f, 0.5216f, 1.f) },
            { SNOW, glm::vec4(1.f, 1.f, 1.f, 1.f) },
            { EMPTY, glm::vec4(1.f)                }
        };

} // namespace constants

#endif // CHUNKCONSTANTS_H
