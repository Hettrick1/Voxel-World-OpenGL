#include "Chunk.h"
#include "World/Structure/Tree.h"
#include "World/Structure/Cactus.h"

Chunk::Chunk(Camera* cam, glm::vec3 pos, int seed, GLuint& texture, float& texWidth, float& texHeight) : vbo(GL_ARRAY_BUFFER), transparentVbo(GL_ARRAY_BUFFER)
{
    mCamera = cam;
    mPosition.x = pos.x * CHUNK_SIZE_X;
    mPosition.y = pos.y * CHUNK_SIZE_Y;
    mPosition.z = 0;
    mTexture = texture;
    mBlockSize = texHeight;
    mTextureWidth = texWidth;

    mBlockShader = new Shader("Core/Shaders/shader.vs", "Core/Shaders/shader.fs");
    mFolliageShader = new Shader("Core/Shaders/folliageShader.vs", "Core/Shaders/folliageShader.fs");

    heightMap.SetSeed(seed);
    heightMap.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    biome.SetSeed(seed);

    // initialize mChunk with the block type. -1 is air.
    int count = 0;
    for (int x = 0; x < CHUNK_SIZE_X; x++) {
        for (int y = 0; y < CHUNK_SIZE_Y; y++) {

            float scale = 2.0f;
            float globalX = mPosition.x + x;
            float globalY = mPosition.y + y;

            // retrieve the chunk information from the biome and the heightmap noises
            float heightValue = heightMap.GetNoise(globalX * scale, globalY * scale);
            float heightBiome = biome.GetNoise(globalX * (scale / 20), globalY * (scale / 20));
            float heightMultiplier = (heightBiome + 1.0f) / 4;
            int height = static_cast<int>((heightValue + 1.0f) * heightMultiplier * CHUNK_SIZE_Z);

            for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                if (z < height - 2) {
                    mChunk[x][y][z] = 3; // stone
                }
                else if (z >= height - 2 && z < height) {
                    mChunk[x][y][z] = (heightMultiplier < 0.1f) ? 4 : 2; // sand or dirt
                }
                else if (z == height) {
                    mChunk[x][y][z] = (heightMultiplier < 0.1f) ? 4 : 1; // sand or grass
                }
                else {
                    mChunk[x][y][z] = -1; // air
                }
            }
        }
    }

    // Calculate visible faces between chunks
    CheckWithNeighborChunk();

    // Calculate visible faces
    for (int x = 0; x < CHUNK_SIZE_X; x++) {
        for (int y = 0; y < CHUNK_SIZE_Y; y++) {
            for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                CheckForNeighborBlock(x, y, z);
                // Add folliage 1% probability
                float folliageProbability = 0.01f;

                // random number between 0 and 1
                float folliageRandomValue = static_cast<float>(rand()) / RAND_MAX;

                if (mChunk[x][y][z] == 4 && z < CHUNK_SIZE_Z - 1 && mChunk[x][y][z + 1] == -1 && folliageRandomValue < folliageProbability / 10) {
                    Cactus* cactus = new Cactus(glm::vec3(x, y, z), mBlockSize, mTextureWidth, (folliageRandomValue * 1000));
                    for (auto vertex : cactus->GetCactusVertices())
                    {
                        mChunkVertices.push_back(vertex);
                    }
                    delete cactus;
                }
                else if (mChunk[x][y][z] == 1 && z < CHUNK_SIZE_Z - 10 && folliageRandomValue < folliageProbability / 15 && CheckForTree(x, y, z)) {
                    Tree* tree = new Tree(glm::vec3(x, y, z), mBlockSize, mTextureWidth, (folliageRandomValue * 1000));
                    for (auto vertex : tree->GetTreeLogVertices())
                    {
                        mChunkVertices.push_back(vertex);
                    }
                    for (auto vertex : tree->GetTreeLeavesVertices())
                    {
                        mChunkVertices.push_back(vertex);
                    }
                    delete tree;
                }
                else if (folliageRandomValue < folliageProbability) {
                    AddFolliage(x, y, z, (folliageRandomValue * 100));
                }
            }
        }
    }

    // bind the VAOs and send all the vertex infos to the shaders
    vao.Bind(); 
    vbo.BufferData(mChunkVertices.size() * sizeof(Vertex), mChunkVertices.data(), GL_STATIC_DRAW); 
    vbo.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)); 
    vbo.VertexAttribPointer(1, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coords));
    vbo.VertexAttribPointer(2, 1, GL_INT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexIndex));
    vao.Unbind();

    transparentVao.Bind();
    transparentVbo.BufferData(mFolliageVertices.size() * sizeof(Vertex), mFolliageVertices.data(), GL_STATIC_DRAW);
    transparentVbo.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)); 
    transparentVbo.VertexAttribPointer(1, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coords));
    transparentVbo.VertexAttribPointer(2, 1, GL_INT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexIndex));
    transparentVao.Unbind();
}

Chunk::~Chunk()
{
    delete mBlockShader;
    delete mFolliageShader;
    transparentVbo.~VertexBuffer();
    vbo.~VertexBuffer();
    vao.~VertexArray();
    transparentVao.~VertexArray();
}

void Chunk::CheckForNeighborBlock(int x, int y, int z)
{
    // if the block we are cheking is air, stop checking
    if (mChunk[x][y][z] == -1) return;

    glm::ivec3 directions[6] = {
        {1, 0, 0}, {-1, 0, 0},  // Right, Left
        {0, 1, 0}, {0, -1, 0},  // Forward, Backward
        {0, 0, 1}, {0, 0, -1}, // Top, Bottom
    };

    for (int i = 0; i < 6; i++) {
        glm::ivec3 neighborPos = glm::ivec3(x, y, z) + directions[i];
        int nx = neighborPos.x, ny = neighborPos.y, nz = neighborPos.z;

        // If we are not at the chunk bounds and the block in the selected direction is air we add a face
        if (nx >= 0 && nx < CHUNK_SIZE_X && ny >= 0 && ny < CHUNK_SIZE_Y && nz >= 0 && nz < CHUNK_SIZE_Z) {
            if (mChunk[nx][ny][nz] == -1) {
                AddFace(x, y, z, directions[i], mChunk[x][y][z]);
            }
        }
    }
}

void Chunk::CheckWithNeighborChunk()
{
    // here we check if we add to add a face between chunks
    glm::ivec3 directions[4] = {
        {1, 0, 0}, {-1, 0, 0},  // Right, Left
        {0, 1, 0}, {0, -1, 0},  // Forward, Backward
    };

    for (int i = 0; i < 4; i++) {
        int directionIndex = -1;
        if (directions[i] == glm::ivec3(1, 0, 0)) directionIndex = 0;   // Right
        if (directions[i] == glm::ivec3(-1, 0, 0)) directionIndex = 1;  // Left
        if (directions[i] == glm::ivec3(0, 1, 0)) directionIndex = 2;   // Forward
        if (directions[i] == glm::ivec3(0, -1, 0)) directionIndex = 3;  // Backward
        
        int oldX = static_cast<int>(mPosition.x);
        int oldY = static_cast<int>(mPosition.y);
        int newX = static_cast<int>(mPosition.x);
        int newY = static_cast<int>(mPosition.y);
        int x = 0;
        int y = 0;

        for (int j = 0; j < 16; j++) { // Check only the border of each chunks
            switch (directionIndex)
            {
            case 0:
                oldX = static_cast<int>(mPosition.x) + 15;
                oldY = static_cast<int>(mPosition.y) + j;
                newX = static_cast<int>(mPosition.x) + 16;
                newY = static_cast<int>(mPosition.y) + j;
                x = 15;
                y = j;
                break;
            case 1:
                oldX = static_cast<int>(mPosition.x);
                oldY = static_cast<int>(mPosition.y) + j;
                newX = static_cast<int>(mPosition.x) -1;
                newY = static_cast<int>(mPosition.y) + j;
                x = 0;
                y = j;
                break;
            case 2:
                oldX = static_cast<int>(mPosition.x) + j;
                oldY = static_cast<int>(mPosition.y) + 15;
                newX = static_cast<int>(mPosition.x) + j;
                newY = static_cast<int>(mPosition.y) + 16;
                x = j;
                y = 15;
                break;
            case 3:
                oldX = static_cast<int>(mPosition.x) + j;
                oldY = static_cast<int>(mPosition.y);
                newX = static_cast<int>(mPosition.x) + j;
                newY = static_cast<int>(mPosition.y) - 1;
                x = j;
                y = 0;
                break;
            }
            float scale = 2.0f;

            // Get the noise value between two chunks to know whether or not to add faces
            float heightValue = heightMap.GetNoise(oldX * scale, oldY * scale); 
            float heightBiome = biome.GetNoise(oldX * (scale / 20), oldY * (scale / 20));
            float heightMultiplier = (heightBiome + 1.0f) / 4;
            int height = static_cast<int>((heightValue + 1.0f) * heightMultiplier * CHUNK_SIZE_Z);

            float newHeightValue = heightMap.GetNoise(newX * scale, newY * scale);
            float newHeightBiome = biome.GetNoise(newX * (scale / 20), newY * (scale / 20));
            float newHeightMultiplier = (newHeightBiome + 1.0f) / 4;
            int newHeight = static_cast<int>((newHeightValue + 1.0f) * newHeightMultiplier * CHUNK_SIZE_Z);

            for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                // If we have to add a face then we add it
                if (height > newHeight && z <= height && z > newHeight) {
                    if (mChunk[x][y][z] != -1) {
                        AddFace(x, y, z, directions[i], mChunk[x][y][z]);
                    }
                }
            }
        }
    }
}

bool Chunk::CheckForTree(int x, int y, int z)
{
    for (int xOffset = -3; xOffset != 3; xOffset++)
    {
        for (int yOffset = -3; yOffset != 3; yOffset++)
        {
            float scale = 2.0f;
            float globalX = mPosition.x + x + xOffset;
            float globalY = mPosition.y + y + yOffset;

            // retrieve the chunk information from the biome and the heightmap noises
            float heightValue = heightMap.GetNoise(globalX * scale, globalY * scale);
            float heightBiome = biome.GetNoise(globalX * (scale / 20), globalY * (scale / 20));
            float heightMultiplier = (heightBiome + 1.0f) / 4;
            int height = static_cast<int>((heightValue + 1.0f) * heightMultiplier * CHUNK_SIZE_Z);
            if (mChunk[x][y][z + 1] != -1 || z + 4 < height) {
                return false;
            }
        }
    }
    return true;
}

void Chunk::AddFolliage(int x, int y, int z, float probability)
{
    // a folliage is made of two planes crossing in the center. 
    float shrink = 0.0f;

    static const glm::vec3 vertexOffsets[2][4] = {
        // Plane 1
        {{0.0f + shrink, 0.0f + shrink, 1.0f}, {0.0f + shrink, 0.0f + shrink, 0.0f},
         {1.0f - shrink, 1.0f - shrink, 0.0f + shrink}, {1.0f - shrink, 1.0f - shrink, 1.0f - shrink}},

         // Plane 2
         {{1.0f - shrink, 0.0f + shrink, 1.0f}, {1.0f - shrink, 0.0f + shrink, 0.0f},
          {0.0f + shrink, 1.0f - shrink, 0.0f + shrink}, {0.0f + shrink, 1.0f - shrink, 1.0f - shrink}},
    };

    if (mChunk[x][y][z] != -1) { // add only if the current block is air
        if ((mChunk[x][y][z] == 4 || mChunk[x][y][z] == 1) && z < CHUNK_SIZE_Z - 1 && mChunk[x][y][z + 1] == -1) {
            int blockIndex = static_cast<int>(Block::DeadBush);
            switch (mChunk[x][y][z])
            {
            case 1:
                if (probability <= 0.3) {
                    blockIndex = static_cast<int>(Block::Dandelion);
                    break;
                }
                else  if (probability <= 0.6) {
                    blockIndex = static_cast<int>(Block::Grass);
                    break;
                }
                else  if (probability <= 1.0) {
                    blockIndex = static_cast<int>(Block::Tulip);
                    break;
                }
                break;
            case 4:
                blockIndex = static_cast<int>(Block::DeadBush);
                break;
            }

            float uMin = (blockIndex * mBlockSize + 0.5f) / mTextureWidth;
            float uMax = ((blockIndex + 1) * mBlockSize) / mTextureWidth;

            glm::vec2 uvCoords[4] = {
                {0.0f, 0.0f},
                {0.0f, 1.0f},
                {1.0f, 1.0f},
                {1.0f, 0.0f},
            };
            // add all the vertices for one face
            for (int i = 0; i < 2; i++)
            {
                const glm::vec3* offsets = vertexOffsets[i];
                // Add the 6 vertices of a face into the vertex vector
                mFolliageVertices.push_back(Vertex{
                    fVec3{x + offsets[0].x,
                          y + offsets[0].y,
                          z + 1 + offsets[0].z},
                    i16Vec2{static_cast<uint16_t>(uvCoords[0].x * 65535),
                            static_cast<uint16_t>(uvCoords[0].y * 65535)},
                            blockIndex
                    });
                mFolliageVertices.push_back(Vertex{
                    fVec3{x + offsets[1].x,
                          y + offsets[1].y,
                          z + 1 + offsets[1].z},
                    i16Vec2{static_cast<uint16_t>(uvCoords[1].x * 65535),
                            static_cast<uint16_t>(uvCoords[1].y * 65535)},
                            blockIndex
                    });
                mFolliageVertices.push_back(Vertex{
                    fVec3{x + offsets[2].x,
                          y + offsets[2].y,
                          z + 1 + offsets[2].z},
                    i16Vec2{static_cast<uint16_t>(uvCoords[2].x * 65535),
                            static_cast<uint16_t>(uvCoords[2].y * 65535)},
                            blockIndex
                    });
                mFolliageVertices.push_back(Vertex{
                    fVec3{x + offsets[0].x,
                          y + offsets[0].y,
                          z + 1 + offsets[0].z},
                    i16Vec2{static_cast<uint16_t>(uvCoords[0].x * 65535),
                            static_cast<uint16_t>(uvCoords[0].y * 65535)},
                            blockIndex
                    });
                mFolliageVertices.push_back(Vertex{
                    fVec3{x + offsets[2].x,
                          y + offsets[2].y,
                          z + 1 + offsets[2].z},
                    i16Vec2{static_cast<uint16_t>(uvCoords[2].x * 65535),
                            static_cast<uint16_t>(uvCoords[2].y * 65535)},
                            blockIndex
                    });
                mFolliageVertices.push_back(Vertex{
                    fVec3{x + offsets[3].x,
                          y + offsets[3].y,
                          z + 1 + offsets[3].z},
                    i16Vec2{static_cast<uint16_t>(uvCoords[3].x * 65535),
                            static_cast<uint16_t>(uvCoords[3].y * 65535)},
                            blockIndex
                    });
            }
        }
    }
}

void Chunk::AddFace(int x, int y, int z, glm::ivec3 direction, int8_t blockType)
{
    static const glm::vec3 vertexOffsets[6][4] = {
        // Right Face (+X)
        {{1, 0, 0}, {1, 1, 0}, {1, 1, 1}, {1, 0, 1}},
        // Left Face (-X)
        {{0, 0, 0}, {0, 1, 0}, {0, 1, 1}, {0, 0, 1}},
        // Backward Face (+Y)
        {{0, 1, 0}, {1, 1, 0}, {1, 1, 1}, {0, 1, 1}},
        // Forward Face (-Y)
        {{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1}},
        // Top Face (+Z)
        {{0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}},
        // Bottom Face (-Z)
        {{0, 0, 0}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0}}
    };

    int faceIndex = -1;
    if (direction == glm::ivec3(1, 0, 0)) faceIndex = 0;   // Right
    if (direction == glm::ivec3(-1, 0, 0)) faceIndex = 1;  // Left
    if (direction == glm::ivec3(0, 1, 0)) faceIndex = 2;   // Backward
    if (direction == glm::ivec3(0, -1, 0)) faceIndex = 3;  // Forward
    if (direction == glm::ivec3(0, 0, 1)) faceIndex = 4;   // Top
    if (direction == glm::ivec3(0, 0, -1)) faceIndex = 5;  // Bottom

    int blockIndex = 0;
    // the blockIndex is used to know which texture to use
    switch (blockType)
    {
    case 1 :
        switch (faceIndex) {
        case 0: blockIndex = static_cast<int>(Block::GrassSideShadow); break;
        case 1: blockIndex = static_cast<int>(Block::GrassSide); break;
        case 2: blockIndex = static_cast<int>(Block::GrassSide); break;
        case 3: blockIndex = static_cast<int>(Block::GrassSideShadow); break;
        case 4: blockIndex = static_cast<int>(Block::GrassTop); break;
        case 5: blockIndex = static_cast<int>(Block::GrassSide); break;
        default: blockIndex = static_cast<int>(Block::Cobblestone); break;
        }
        break;
    case 2:
        switch (faceIndex) {
        case 0: blockIndex = static_cast<int>(Block::DirtShadow); break;
        case 1: blockIndex = static_cast<int>(Block::Dirt); break;
        case 2: blockIndex = static_cast<int>(Block::Dirt); break;
        case 3: blockIndex = static_cast<int>(Block::DirtShadow); break;
        case 4: blockIndex = static_cast<int>(Block::Dirt); break;
        case 5: blockIndex = static_cast<int>(Block::Dirt); break;
        default: blockIndex = static_cast<int>(Block::Cobblestone); break;
        }
        break;
    case 3:
        switch (faceIndex) {
        case 0: blockIndex = static_cast<int>(Block::StoneShadow); break;
        case 1: blockIndex = static_cast<int>(Block::Stone); break;
        case 2: blockIndex = static_cast<int>(Block::Stone); break;
        case 3: blockIndex = static_cast<int>(Block::StoneShadow); break;
        case 4: blockIndex = static_cast<int>(Block::Stone); break;
        case 5: blockIndex = static_cast<int>(Block::Stone); break;
        default: blockIndex = static_cast<int>(Block::Cobblestone); break;
        }
        break;
    case 4:
        switch (faceIndex) {
        case 0: blockIndex = static_cast<int>(Block::SandShadow); break;
        case 1: blockIndex = static_cast<int>(Block::Sand); break;
        case 2: blockIndex = static_cast<int>(Block::Sand); break;
        case 3: blockIndex = static_cast<int>(Block::SandShadow); break;
        case 4: blockIndex = static_cast<int>(Block::Sand); break;
        case 5: blockIndex = static_cast<int>(Block::Sand); break;
        default: blockIndex = static_cast<int>(Block::Cobblestone); break;
        }
        break;
    }
    
    float uMin = (blockIndex * mBlockSize) / mTextureWidth;
    float uMax = ((blockIndex + 1) * mBlockSize) / mTextureWidth;

    glm::vec2 uvCoords[4] = {
        {1.0, 1.0f},
        {0.0, 1.0f},
        {0.0, 0.0f},
        {1.0, 0.0f},
    };

    if (faceIndex >= 0) {
        const glm::vec3* offsets = vertexOffsets[faceIndex];

        // Add the 6 vertices of a face into the vertex vector
        mChunkVertices.push_back(Vertex{
            fVec3{x + offsets[0].x,
                  y + offsets[0].y,
                  z + offsets[0].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[0].x * 65535),
                    static_cast<uint16_t>(uvCoords[0].y * 65535)},
                    blockIndex
            });
        mChunkVertices.push_back(Vertex{
            fVec3{x + offsets[1].x,
                  y + offsets[1].y,
                  z + offsets[1].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[1].x * 65535),
                    static_cast<uint16_t>(uvCoords[1].y * 65535)},
                    blockIndex
            });
        mChunkVertices.push_back(Vertex{
            fVec3{x + offsets[2].x,
                  y + offsets[2].y,
                  z + offsets[2].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[2].x * 65535),
                    static_cast<uint16_t>(uvCoords[2].y * 65535)},
                    blockIndex
            });
        mChunkVertices.push_back(Vertex{
            fVec3{x + offsets[0].x,
                  y + offsets[0].y,
                  z + offsets[0].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[0].x * 65535),
                    static_cast<uint16_t>(uvCoords[0].y * 65535)},
                    blockIndex
            });
        mChunkVertices.push_back(Vertex{
            fVec3{x + offsets[2].x,
                  y + offsets[2].y,
                  z + offsets[2].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[2].x * 65535),
                    static_cast<uint16_t>(uvCoords[2].y * 65535)},
                    blockIndex
            });
        mChunkVertices.push_back(Vertex{
            fVec3{x + offsets[3].x,
                  y + offsets[3].y,
                  z + offsets[3].z},
            i16Vec2{static_cast<uint16_t>(uvCoords[3].x * 65535),
                    static_cast<uint16_t>(uvCoords[3].y * 65535)},
                    blockIndex
            });
    }
}

glm::vec3 Chunk::GetPosition()
{
    return mPosition;
}

void Chunk::SetPosition(glm::vec3 newPos)
{
    mPosition.x = newPos.x * 16;
    mPosition.y = newPos.y * 16;
    mPosition.z = 0;
}

void Chunk::DrawChunkMesh()
{ 
    if (mChunkVertices.size() > 0) {
        // Use shader and send all the matrices
        mBlockShader->Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mTexture);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), mPosition); 
        glm::mat4 view = mCamera->GetViewMatrix(); 
        glm::mat4 projection = mCamera->GetProjectionMatrix(); 
        glm::mat4 mvp = projection * view * model; 

        mBlockShader->SetMat4("u_MVP", mvp);
        // Draw the chunk
        vao.Bind();
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mChunkVertices.size()));
        vao.Unbind();
    }
}

void Chunk::DrawFolliageMesh()
{
    if (mFolliageVertices.size() > 0) {

        mFolliageShader->Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mTexture);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), mPosition);
        glm::mat4 view = mCamera->GetViewMatrix();
        glm::mat4 projection = mCamera->GetProjectionMatrix();
        glm::mat4 mvp = projection * view * model;

        mFolliageShader->SetMat4("u_MVP", mvp);
        transparentVao.Bind();
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mFolliageVertices.size()));
        transparentVao.Unbind();
    }
}
