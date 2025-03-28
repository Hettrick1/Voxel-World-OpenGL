#include "Cactus.h"

Cactus::Cactus(glm::vec3 initialPos, float blockSize, float textureWidth, float heightProbability)
    : mTextureWidth(textureWidth), mBlockSize(blockSize)
{
    glm::vec3 directionsBottom[4] = {
        {1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}
    };
    glm::vec3 directionsTop[5] = {
        {1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {0,0,1}
    };
    if (heightProbability <= 0.5)
    {
        mCactusHeight = 3;
    }
    else {
        mCactusHeight = 2;
    }
    for (int i = 1; i < mCactusHeight + 1; i++) 
    {
        if (i < mCactusHeight)
        {
            for (auto direction : directionsBottom)
            {
                AddFace(initialPos.x, initialPos.y, initialPos.z + i, direction);
            }
        }
        else 
        {
            for (auto direction : directionsTop)
            {
                AddFace(initialPos.x, initialPos.y, initialPos.z + i, direction);
            }
        }
    }
}

Cactus::~Cactus()
{
    mCactusVertices.clear();
}

void Cactus::AddFace(int x, int y, int z, glm::ivec3 direction)
{
    float shrink = 0.063;
    static const glm::vec3 vertexOffsets[6][4] = {
        // Right Face (+X)
        {{1.0f - shrink, 0.0f, 0.0f}, {1.0f - shrink, 1.0f, 0.0f}, {1.0f - shrink, 1.0f, 1.0f}, {1.0f - shrink, 0.0f, 1.0f}},
        // Left Face (-X)
        {{0.0f + shrink, 0.0f, 0.0f}, {0.0f + shrink, 1.0f, 0.0f}, {0.0f + shrink, 1.0f, 1.0f}, {0.0f + shrink, 0.0f, 1.0f}},
        // Backward Face (+Y)
        {{0.0f, 1.0f - shrink, 0.0f}, {1.0f, 1.0f - shrink, 0.0f}, {1.0f, 1.0f - shrink, 1.0f}, {0.0f, 1.0f - shrink, 1.0f}},
        // Forward Face (-Y)
        {{0.0f, 0.0f + shrink, 0.0f}, {1.0f, 0.0f + shrink, 0.0f}, {1.0f, 0.0f + shrink, 1.0f}, {0.0f, 0.0f + shrink, 1.0f}},
        // Top Face (+Z)
        {{0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},
        // Bottom Face (-Z)
        {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}}
    };

    int faceIndex = -1;
    if (direction == glm::ivec3(1, 0, 0)) faceIndex = 0;   // Right
    if (direction == glm::ivec3(-1, 0, 0)) faceIndex = 1;  // Left
    if (direction == glm::ivec3(0, 1, 0)) faceIndex = 2;   // Backward
    if (direction == glm::ivec3(0, -1, 0)) faceIndex = 3;  // Forward
    if (direction == glm::ivec3(0, 0, 1)) faceIndex = 4;   // Top

    int blockIndex = 0;

        switch (faceIndex) 
        {
            case 0: 
                blockIndex = static_cast<int>(Block::CactusShadow); 
                break;
            case 1: 
                blockIndex = static_cast<int>(Block::Cactus); 
                break;
            case 2: 
                blockIndex = static_cast<int>(Block::Cactus);
                break;
            case 3: 
                blockIndex = static_cast<int>(Block::CactusShadow);
                break;
            case 4: 
                blockIndex = static_cast<int>(Block::CactusTop); 
                break;
            default: 
                blockIndex = static_cast<int>(Block::Cobblestone); 
                break;
        }

    float uMin = (blockIndex * mBlockSize) / mTextureWidth;
    float uMax = ((blockIndex + 1) * mBlockSize) / mTextureWidth;

    glm::vec2 uvCoords[4] = {
        {uMax, 1.0f},
        {uMin, 1.0f},
        {uMin, 0.0f},
        {uMax, 0.0f},
    };

    if (faceIndex >= 0) {
        const glm::vec3* offsets = vertexOffsets[faceIndex];

        // Add the 6 vertices of a face into the vertex vector
        mCactusVertices.push_back(Vertex{
            fVec3{x + offsets[0].x,
                  y + offsets[0].y,
                  z + offsets[0].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[0].x * 65535),
                    static_cast<uint16_t>(uvCoords[0].y * 65535)}
            });
        mCactusVertices.push_back(Vertex{
            fVec3{x + offsets[1].x,
                  y + offsets[1].y,
                  z + offsets[1].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[1].x * 65535),
                    static_cast<uint16_t>(uvCoords[1].y * 65535)}
            });
        mCactusVertices.push_back(Vertex{
            fVec3{x + offsets[2].x,
                  y + offsets[2].y,
                  z + offsets[2].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[2].x * 65535),
                    static_cast<uint16_t>(uvCoords[2].y * 65535)}
            });
        mCactusVertices.push_back(Vertex{
            fVec3{x + offsets[0].x,
                  y + offsets[0].y,
                  z + offsets[0].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[0].x * 65535),
                    static_cast<uint16_t>(uvCoords[0].y * 65535)}
            });
        mCactusVertices.push_back(Vertex{
            fVec3{x + offsets[2].x,
                  y + offsets[2].y,
                  z + offsets[2].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[2].x * 65535),
                    static_cast<uint16_t>(uvCoords[2].y * 65535)}
            });
        mCactusVertices.push_back(Vertex{
            fVec3{x + offsets[3].x,
                  y + offsets[3].y,
                  z + offsets[3].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[3].x * 65535),
                    static_cast<uint16_t>(uvCoords[3].y * 65535)}
            });
    }
}