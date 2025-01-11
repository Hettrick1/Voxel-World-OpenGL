#include "Chunk.h"

Chunk::Chunk(Camera* cam, glm::vec3 pos, int seed) : vbo(GL_ARRAY_BUFFER)
{
    mCamera = cam;
    mPosition.x = pos.x * CHUNK_SIZE_X;
    mPosition.y = pos.y * CHUNK_SIZE_Y;
    mPosition.z = 0;
    mTexture = 0;

    mShader = new Shader("Core/Shaders/shader.vs", "Core/Shaders/shader.fs");

    heightMap.SetSeed(seed);
    heightMap.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    biome.SetSeed(seed);

    // Initialisation de mChunk avec des nullptr ou autres valeurs par défaut
    int count = 0;
    for (int x = 0; x < CHUNK_SIZE_X; x++) {
        for (int y = 0; y < CHUNK_SIZE_Y; y++) {

            float scale = 2.0f;
            float globalX = mPosition.x + x;
            float globalY = mPosition.y + y;

            float heightValue = heightMap.GetNoise(globalX * scale, globalY * scale);
            float heightBiome = biome.GetNoise(globalX * (scale / 20), globalY * (scale / 20));
            float heightMultiplier = (heightBiome + 1.0f) / 4;
            int height = static_cast<int>((heightValue + 1.0f) * heightMultiplier * CHUNK_SIZE_Z);

            for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                if (z < height - 2) {
                    mChunk[x][y][z] = new GLuint(3); // stone
                }
                else if (z >= height - 2 && z < height) {
                    mChunk[x][y][z] = (heightMultiplier < 0.1f) ? new GLuint(4) : new GLuint(2); // sand or dirt
                }
                else if (z == height) {
                    mChunk[x][y][z] = (heightMultiplier < 0.1f) ? new GLuint(4) : new GLuint(1); // sand or grass
                }
                else {
                    mChunk[x][y][z] = nullptr;
                }
            }
        }
    }

    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);

    // Paramètres de la texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("Game/Resources/176x16_sheet.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        mBlockSize = height;
        mTextureWidth = width;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        //std::cerr << "Failed to load texture" << std::endl;
        std::cerr << "Erreur : " << stbi_failure_reason() << std::endl;
    }
    stbi_image_free(data);

    // Calculate visible faces
    for (int x = 0; x < CHUNK_SIZE_X; x++) {
        for (int y = 0; y < CHUNK_SIZE_Y; y++) {
            for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                CheckForNeighbors(x, y, z);
            }
        }
    }
    // Calculate visible faces between chunks
    CheckWithNeighborsChunk();

    vao.Bind(); 
    vbo.BufferData(mAllVertices.size() * sizeof(Vertex), mAllVertices.data(), GL_STATIC_DRAW); 
    vbo.VertexAttribPointer(0, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)); 
    vbo.VertexAttribPointer(1, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coords)); 
    vao.Unbind();
}

Chunk::~Chunk()
{
    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
            for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                delete mChunk[x][y][z];  // Supprimer les blocs alloués
            }
        }
    }
}

void Chunk::CheckForNeighbors(int x, int y, int z)
{
    if (mChunk[x][y][z] == nullptr) return;

    glm::ivec3 directions[6] = {
        {1, 0, 0}, {-1, 0, 0},  // Droite, Gauche
        {0, 1, 0}, {0, -1, 0},  // Avant, Arrière
        {0, 0, 1}, {0, 0, -1}, // Haut, Bas
    };

    for (int i = 0; i < 6; i++) {
        glm::ivec3 neighborPos = glm::ivec3(x, y, z) + directions[i];
        int nx = neighborPos.x, ny = neighborPos.y, nz = neighborPos.z;

        // Vérifier les limites du chunk
        if (nx >= 0 && nx < CHUNK_SIZE_X && ny >= 0 && ny < CHUNK_SIZE_Y && nz >= 0 && nz < CHUNK_SIZE_Z) {
            if (mChunk[nx][ny][nz] == nullptr) {
                // Ajouter les 6 sommets nécessaires pour dessiner une face
                AddFace(x, y, z, directions[i], *mChunk[x][y][z]);
            }
        }
    }
}

void Chunk::CheckWithNeighborsChunk()
{
    glm::ivec3 directions[4] = {
        {1, 0, 0}, {-1, 0, 0},  // Droite, Gauche
        {0, 1, 0}, {0, -1, 0},  // Avant, Arrière
    };

    for (int i = 0; i < 4; i++) {
        int directionIndex = -1;
        if (directions[i] == glm::ivec3(1, 0, 0)) directionIndex = 0;   // Droite
        if (directions[i] == glm::ivec3(-1, 0, 0)) directionIndex = 1;  // Gauche
        if (directions[i] == glm::ivec3(0, 1, 0)) directionIndex = 2;   // Avant
        if (directions[i] == glm::ivec3(0, -1, 0)) directionIndex = 3;  // Arrière
        
        int oldX = mPosition.x;
        int oldY = mPosition.y;
        int newX = mPosition.x;
        int newY = mPosition.y;
        int x = 0;
        int y = 0;

        for (int j = 0; j < 16; j++) {
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

            float heightValue = heightMap.GetNoise(oldX * scale, oldY * scale); 
            float heightBiome = biome.GetNoise(oldX * (scale / 20), oldY * (scale / 20));
            float heightMultiplier = (heightBiome + 1.0f) / 4;
            int height = static_cast<int>((heightValue + 1.0f) * heightMultiplier * CHUNK_SIZE_Z);

            float newHeightValue = heightMap.GetNoise(newX * scale, newY * scale);
            float newHeightBiome = biome.GetNoise(newX * (scale / 20), newY * (scale / 20));
            float newHeightMultiplier = (newHeightBiome + 1.0f) / 4;
            int newHeight = static_cast<int>((newHeightValue + 1.0f) * newHeightMultiplier * CHUNK_SIZE_Z);

            for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                // Vérification finale avant l'accès
                if (height > newHeight && z <= height && z > newHeight) {
                    if (mChunk[x][y][z] != nullptr) {
                        AddFace(x, y, z, directions[i], *mChunk[x][y][z]);
                    }
                }
            }
        }
    }
}

void Chunk::AddFace(int x, int y, int z, glm::ivec3 direction, GLuint blockType)
{
    static const glm::vec3 vertexOffsets[6][4] = {
        // Face droite (+X)
        {{1, 0, 0}, {1, 1, 0}, {1, 1, 1}, {1, 0, 1}},
        // Face gauche (-X)
        {{0, 0, 0}, {0, 1, 0}, {0, 1, 1}, {0, 0, 1}},
        // Face derrière (+Y)
        {{0, 1, 0}, {1, 1, 0}, {1, 1, 1}, {0, 1, 1}},
        // Face devant (-Y)
        {{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1}},
        // Face haut (+Z)
        {{0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}},
        // Face bas (-Z)
        {{0, 0, 0}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0}}
    };

    int faceIndex = -1;
    if (direction == glm::ivec3(1, 0, 0)) faceIndex = 0;   // Droite
    if (direction == glm::ivec3(-1, 0, 0)) faceIndex = 1;  // Gauche
    if (direction == glm::ivec3(0, 1, 0)) faceIndex = 2;   // Avant
    if (direction == glm::ivec3(0, -1, 0)) faceIndex = 3;  // Arrière
    if (direction == glm::ivec3(0, 0, 1)) faceIndex = 4;   // Haut
    if (direction == glm::ivec3(0, 0, -1)) faceIndex = 5;  // Bas

    int blockIndex = 0;
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
  
        // Ajouter les 6 sommets
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

Chunk* Chunk::GetChunkWithPosition(int x, int y, int z)
{
	if (x == mPosition.x / CHUNK_SIZE_X && y == 0 && z == mPosition.z / CHUNK_SIZE_Z) {
		return this;
	}
	return nullptr;
}

void Chunk::Draw()
{
    if (mAllVertices.size() > 0) {
        // Charger le shader et transmettre les matrices
        mShader->Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mTexture);
        mShader->SetInt("u_Texture", 0);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), mPosition); 
        glm::mat4 view = mCamera->GetViewMatrix(); 
        glm::mat4 projection = mCamera->GetProjectionMatrix(); 
        glm::mat4 mvp = projection * view * model; 

        mShader->SetMat4("u_MVP", mvp);
        // Dessiner
        vao.Bind();
        glDrawArrays(GL_TRIANGLES, 0, mAllVertices.size());
        vao.Unbind();
    }
}
