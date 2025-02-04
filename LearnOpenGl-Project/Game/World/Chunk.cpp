#include "Chunk.h"

Chunk::Chunk(Camera* cam, glm::vec3 pos, int seed, GLuint& texture, float& texWidth, float& texHeight) : vbo(GL_ARRAY_BUFFER), transparentVbo(GL_ARRAY_BUFFER)
{
    mCamera = cam;
    mPosition.x = pos.x * CHUNK_SIZE_X;
    mPosition.y = pos.y * CHUNK_SIZE_Y;
    mPosition.z = 0;
    mTexture = texture;
    mBlockSize = texHeight;
    mTextureWidth = texWidth;

    mShader = new Shader("Core/Shaders/shader.vs", "Core/Shaders/shader.fs");

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
    CheckWithNeighborsChunk();

    // Calculate visible faces
    for (int x = 0; x < CHUNK_SIZE_X; x++) {
        for (int y = 0; y < CHUNK_SIZE_Y; y++) {
            for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                CheckForNeighbors(x, y, z);
                // Add folliage 1% probability
                float folliageProbability = 0.01f;

                // random number between 0 and 1
                float folliageRandomValue = static_cast<float>(rand()) / RAND_MAX;

                if (folliageRandomValue < folliageProbability) {
                    AddFolliage(x, y, z); 
                }
            }
        }
    }

    // bind the VAOs and send all the vertex infos to the shaders
    vao.Bind(); 
    vbo.BufferData(mAllVertices.size() * sizeof(Vertex), mAllVertices.data(), GL_STATIC_DRAW); 
    vbo.VertexAttribPointer(0, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)); 
    vbo.VertexAttribPointer(1, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coords)); 
    vao.Unbind();

    transparentVao.Bind();
    transparentVbo.BufferData(mTransparentVertices.size() * sizeof(Vertex), mTransparentVertices.data(), GL_STATIC_DRAW);
    transparentVbo.VertexAttribPointer(0, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    transparentVbo.VertexAttribPointer(1, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coords));
    transparentVao.Unbind();
}

Chunk::~Chunk()
{

}

void Chunk::CheckForNeighbors(int x, int y, int z)
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

void Chunk::CheckWithNeighborsChunk()
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
        
        int oldX = mPosition.x;
        int oldY = mPosition.y;
        int newX = mPosition.x;
        int newY = mPosition.y;
        int x = 0;
        int y = 0;

        for (int j = 0; j < 16; j++) { // Check only the border of each chunks
            switch (directionIndex)
            {
            case 0:
                oldX = mPosition.x + 15;
                oldY = mPosition.y + j;
                newX = mPosition.x + 16;
                newY = mPosition.y + j;
                x = 15;
                y = j;
                break;
            case 1:
                oldX = mPosition.x;
                oldY = mPosition.y + j;
                newX = mPosition.x -1;
                newY = mPosition.y + j;
                x = 0;
                y = j;
                break;
            case 2:
                oldX = mPosition.x + j;
                oldY = mPosition.y + 15;
                newX = mPosition.x + j;
                newY = mPosition.y + 16;
                x = j;
                y = 15;
                break;
            case 3:
                oldX = mPosition.x + j;
                oldY = mPosition.y;
                newX = mPosition.x + j;
                newY = mPosition.y - 1;
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

void Chunk::AddFolliage(int x, int y, int z)
{
    // a folliage is made of two planes crossing in the center. 
    static const glm::vec3 vertexOffsets[2][4] = {
         {{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}, // plane 1
         {{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}}, // plane 2
    };

    if (mChunk[x][y][z] != -1) { // add only if the current block is air
        if ((mChunk[x][y][z] == 4 || mChunk[x][y][z] == 1) && z < CHUNK_SIZE_Z - 1 && mChunk[x][y][z + 1] == -1) {
            int blockIndex = static_cast<int>(Block::DeadBush);
            float TypeProbability = 0.5f;
            float TypeRandomValue = static_cast<float>(rand()) / RAND_MAX; // random between Dandelion or Tulip
            switch (mChunk[x][y][z])
            {
            case 1 :
                if (TypeRandomValue <= TypeProbability) {
                    blockIndex = static_cast<int>(Block::Dandelion);
                }
                else {
                    blockIndex = static_cast<int>(Block::Tulip);
                }
                break;
            case 4:
                blockIndex = static_cast<int>(Block::DeadBush);
                break;
            }
            
            float uMin = (blockIndex * mBlockSize) / mTextureWidth;
            float uMax = ((blockIndex + 1) * mBlockSize) / mTextureWidth;

            glm::vec2 uvCoords[4] = {
                {uMin, 0.0f},
                {uMin, 1.0f},
                {uMax, 1.0f},
                {uMax, 0.0f},
            };
            // add all the vertices for one face
            for (int i = 0; i < 2; i++){
            const glm::vec3* offsets = vertexOffsets[i];
            mTransparentVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x + offsets[0].x),
                                                 static_cast<uint8_t>(y + offsets[0].y),
                                                 static_cast<uint8_t>(z + 1 + offsets[0].z)},
                                           i16Vec2{static_cast<uint16_t>(uvCoords[0].x * 65535),
                                                   static_cast<uint16_t>(uvCoords[0].y * 65535)} });
            mTransparentVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x + offsets[1].x),
                                                 static_cast<uint8_t>(y + offsets[1].y),
                                                 static_cast<uint8_t>(z + 1 + offsets[1].z)},
                                           i16Vec2{static_cast<uint16_t>(uvCoords[1].x * 65535),
                                                   static_cast<uint16_t>(uvCoords[1].y * 65535)} });
            mTransparentVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x + offsets[2].x),
                                                 static_cast<uint8_t>(y + offsets[2].y),
                                                 static_cast<uint8_t>(z + 1 + offsets[2].z)},
                                           i16Vec2{static_cast<uint16_t>(uvCoords[2].x * 65535),
                                                   static_cast<uint16_t>(uvCoords[2].y * 65535)} });
            mTransparentVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x + offsets[0].x),
                                                 static_cast<uint8_t>(y + offsets[0].y),
                                                 static_cast<uint8_t>(z + 1 + offsets[0].z)},
                                           i16Vec2{static_cast<uint16_t>(uvCoords[0].x * 65535),
                                                   static_cast<uint16_t>(uvCoords[0].y * 65535)} });
            mTransparentVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x + offsets[2].x),
                                                 static_cast<uint8_t>(y + offsets[2].y),
                                                 static_cast<uint8_t>(z + 1 + offsets[2].z)},
                                           i16Vec2{static_cast<uint16_t>(uvCoords[2].x * 65535),
                                                   static_cast<uint16_t>(uvCoords[2].y * 65535)} });
            mTransparentVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x + offsets[3].x),
                                                 static_cast<uint8_t>(y + offsets[3].y),
                                                 static_cast<uint8_t>(z + 1 + offsets[3].z)},
                                           i16Vec2{static_cast<uint16_t>(uvCoords[3].x * 65535),
                                                   static_cast<uint16_t>(uvCoords[3].y * 65535)} });
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
        {uMax, 1.0f},
        {uMin, 1.0f},
        {uMin, 0.0f},
        {uMax, 0.0f},
    };

    if (faceIndex >= 0) {
        const glm::vec3* offsets = vertexOffsets[faceIndex];
  
        // Add the 6 vertices of a face into the vertex vector
        mAllVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x + offsets[0].x),
                                             static_cast<uint8_t>(y + offsets[0].y),
                                             static_cast<uint8_t>(z + offsets[0].z)},
                                       i16Vec2{static_cast<uint16_t>(uvCoords[0].x * 65535),
                                               static_cast<uint16_t>(uvCoords[0].y * 65535)} });
        mAllVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x + offsets[1].x),
                                             static_cast<uint8_t>(y + offsets[1].y),
                                             static_cast<uint8_t>(z + offsets[1].z)},
                                       i16Vec2{static_cast<uint16_t>(uvCoords[1].x * 65535),
                                               static_cast<uint16_t>(uvCoords[1].y * 65535)} });
        mAllVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x + offsets[2].x),
                                             static_cast<uint8_t>(y + offsets[2].y),
                                             static_cast<uint8_t>(z + offsets[2].z)},
                                       i16Vec2{static_cast<uint16_t>(uvCoords[2].x * 65535),
                                               static_cast<uint16_t>(uvCoords[2].y * 65535)} });
        mAllVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x + offsets[0].x),
                                             static_cast<uint8_t>(y + offsets[0].y),
                                             static_cast<uint8_t>(z + offsets[0].z)},
                                       i16Vec2{static_cast<uint16_t>(uvCoords[0].x * 65535),
                                               static_cast<uint16_t>(uvCoords[0].y * 65535)} });
        mAllVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x + offsets[2].x),
                                             static_cast<uint8_t>(y + offsets[2].y),
                                             static_cast<uint8_t>(z + offsets[2].z)},
                                       i16Vec2{static_cast<uint16_t>(uvCoords[2].x * 65535),
                                               static_cast<uint16_t>(uvCoords[2].y * 65535)} });
        mAllVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x + offsets[3].x),
                                             static_cast<uint8_t>(y + offsets[3].y),
                                             static_cast<uint8_t>(z + offsets[3].z)},
                                       i16Vec2{static_cast<uint16_t>(uvCoords[3].x * 65535),
                                               static_cast<uint16_t>(uvCoords[3].y * 65535)} });
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

void Chunk::Draw()
{ 
    if (mAllVertices.size() > 0) {
        // Use shader and send all the matrices
        mShader->Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mTexture);
        mShader->SetInt("u_Texture", 0); // send the texture

        glm::mat4 model = glm::translate(glm::mat4(1.0f), mPosition); 
        glm::mat4 view = mCamera->GetViewMatrix(); 
        glm::mat4 projection = mCamera->GetProjectionMatrix(); 
        glm::mat4 mvp = projection * view * model; 

        mShader->SetMat4("u_MVP", mvp);
        // Draw the chunk
        vao.Bind();
        glDrawArrays(GL_TRIANGLES, 0, mAllVertices.size());
        vao.Unbind();
    }
}

void Chunk::DrawTransparent()
{
    if (mTransparentVertices.size() > 0) {
        glEnable(GL_BLEND); // use the transparence
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        mShader->Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mTexture);
        mShader->SetInt("u_Texture", 0);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), mPosition);
        glm::mat4 view = mCamera->GetViewMatrix();
        glm::mat4 projection = mCamera->GetProjectionMatrix();
        glm::mat4 mvp = projection * view * model;

        mShader->SetMat4("u_MVP", mvp);
        transparentVao.Bind();
        glDrawArrays(GL_TRIANGLES, 0, mTransparentVertices.size());
        transparentVao.Unbind();
        glDisable(GL_BLEND);
    }
}
