#include "Tree.h"
#include "TreeLeavesTemplate.h"

Tree::Tree(glm::vec3 initialPos, float blockSize, float textureWidth, float heightProbability)
    : mTextureWidth(textureWidth), mBlockSize(blockSize), mTruncHeight(4)
{
    if (heightProbability <= 0.3)
    {
        memcpy(mLeaves, OAK_LEAVES, sizeof(OAK_LEAVES));
    }
    else {
        memcpy(mLeaves, OAK_LEAVES_2, sizeof(OAK_LEAVES_2));;
    }

    glm::vec3 directionsBottom[4] = {
        {1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}
    };
    glm::vec3 directionsTop[5] = {
        {1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {0,0,1} // is it's the top block add the top face
    };

    for (int i = 1; i < mTruncHeight + 1; i++)
    {
        if (i < mTruncHeight)
        {
            for (auto direction : directionsBottom)
            {
                AddLogFace(static_cast<int>(initialPos.x), static_cast<int>(initialPos.y), static_cast<int>(initialPos.z + i), direction);
            }
        }
        else 
        {
            for (auto direction : directionsTop)
            {
                AddLogFace(static_cast<int>(initialPos.x), static_cast<int>(initialPos.y), static_cast<int>(initialPos.z + i), direction);
            }   
        }
    }

    int centerX = 2, centerY = 2;  // Centre de la matrice de feuilles
    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            for (int z = 0; z < 6; z++) {
                if (mLeaves[x][y][z] == 1) {
                    int worldX = static_cast<int>(initialPos.x) + x - centerX;
                    int worldY = static_cast<int>(initialPos.y) + y - centerY;
                    int worldZ = static_cast<int>(initialPos.z) + z + (int)mTruncHeight - 1;

                    glm::ivec3 directions[6] = {
                        {1, 0, 0}, {-1, 0, 0},  // Droite, Gauche
                        {0, 1, 0}, {0, -1, 0},  // Avant, Arrière
                        {0, 0, 1}, {0, 0, -1}   // Haut, Bas
                    };

                    for (int i = 0; i < 6; i++) {
                        AddLeavesFace(worldX, worldY, worldZ, directions[i]);
                    }
                }
            }
        }
    }
}

Tree::~Tree()
{
    mTreeLogVertices.clear();
    mTreeLeavesVertices.clear();
}

void Tree::AddLogFace(int x, int y, int z, glm::ivec3 direction)
{
    float shrink = 0.0f;
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
        blockIndex = static_cast<int>(Block::OakLogShadow);
        break;
    case 1:
        blockIndex = static_cast<int>(Block::OakLog);
        break;
    case 2:
        blockIndex = static_cast<int>(Block::OakLog);
        break;
    case 3:
        blockIndex = static_cast<int>(Block::OakLogShadow);
        break;
    case 4:
        blockIndex = static_cast<int>(Block::OakLogTop);
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
        mTreeLogVertices.push_back(Vertex{
            fVec3{x + offsets[0].x,
                  y + offsets[0].y,
                  z + offsets[0].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[0].x * 65535),
                    static_cast<uint16_t>(uvCoords[0].y * 65535)}
            });
        mTreeLogVertices.push_back(Vertex{
            fVec3{x + offsets[1].x,
                  y + offsets[1].y,
                  z + offsets[1].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[1].x * 65535),
                    static_cast<uint16_t>(uvCoords[1].y * 65535)}
            });
        mTreeLogVertices.push_back(Vertex{
            fVec3{x + offsets[2].x,
                  y + offsets[2].y,
                  z + offsets[2].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[2].x * 65535),
                    static_cast<uint16_t>(uvCoords[2].y * 65535)}
            });
        mTreeLogVertices.push_back(Vertex{
            fVec3{x + offsets[0].x,
                  y + offsets[0].y,
                  z + offsets[0].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[0].x * 65535),
                    static_cast<uint16_t>(uvCoords[0].y * 65535)}
            });
        mTreeLogVertices.push_back(Vertex{
            fVec3{x + offsets[2].x,
                  y + offsets[2].y,
                  z + offsets[2].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[2].x * 65535),
                    static_cast<uint16_t>(uvCoords[2].y * 65535)}
            });
        mTreeLogVertices.push_back(Vertex{
            fVec3{x + offsets[3].x,
                  y + offsets[3].y,
                  z + offsets[3].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[3].x * 65535),
                    static_cast<uint16_t>(uvCoords[3].y * 65535)}
            });
    }
}

void Tree::AddLeavesFace(int x, int y, int z, glm::ivec3 direction)
{
    float shrink = 0.001f;
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
        blockIndex = static_cast<int>(Block::OakLeavesShadow);
        break;
    case 1:
        blockIndex = static_cast<int>(Block::OakLeaves);
        break;
    case 2:
        blockIndex = static_cast<int>(Block::OakLeaves);
        break;
    case 3:
        blockIndex = static_cast<int>(Block::OakLeavesShadow);
        break;
    case 4:
        blockIndex = static_cast<int>(Block::OakLeaves);
        break;
    case 5:
        blockIndex = static_cast<int>(Block::OakLeaves);
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
        mTreeLeavesVertices.push_back(Vertex{
            fVec3{x + offsets[0].x,
                  y + offsets[0].y,
                  z + offsets[0].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[0].x * 65535),
                    static_cast<uint16_t>(uvCoords[0].y * 65535)}
            });
        mTreeLeavesVertices.push_back(Vertex{
            fVec3{x + offsets[1].x,
                  y + offsets[1].y,
                  z + offsets[1].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[1].x * 65535),
                    static_cast<uint16_t>(uvCoords[1].y * 65535)}
            });
        mTreeLeavesVertices.push_back(Vertex{
            fVec3{x + offsets[2].x,
                  y + offsets[2].y,
                  z + offsets[2].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[2].x * 65535),
                    static_cast<uint16_t>(uvCoords[2].y * 65535)}
            });
        mTreeLeavesVertices.push_back(Vertex{
            fVec3{x + offsets[0].x,
                  y + offsets[0].y,
                  z + offsets[0].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[0].x * 65535),
                    static_cast<uint16_t>(uvCoords[0].y * 65535)}
            });
        mTreeLeavesVertices.push_back(Vertex{
            fVec3{x + offsets[2].x,
                  y + offsets[2].y,
                  z + offsets[2].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[2].x * 65535),
                    static_cast<uint16_t>(uvCoords[2].y * 65535)}
            });
        mTreeLeavesVertices.push_back(Vertex{
            fVec3{x + offsets[3].x,
                  y + offsets[3].y,
                  z + offsets[3].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[3].x * 65535),
                    static_cast<uint16_t>(uvCoords[3].y * 65535)}
            });
    }
}
