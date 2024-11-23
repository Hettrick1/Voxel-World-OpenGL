#include "Chunk.h"

Chunk::Chunk(Camera* cam, glm::vec3 pos) : vbo(GL_ARRAY_BUFFER)
{
    mCamera = cam;
    mPosition.x = pos.x * CHUNK_SIZE_X;
    mPosition.y = 0;
    mPosition.z = pos.z * CHUNK_SIZE_Z;

    mShader = new Shader("Core/Shaders/shader.vs", "Core/Shaders/shader.fs");

    // Initialisation de mChunk avec des nullptr ou autres valeurs par défaut
    int count = 0;
    for (int x = 0; x < CHUNK_SIZE_X; x++) {
        for (int z = 0; z < CHUNK_SIZE_Z; z++) {
            for (int y = 0; y < 12; y++) {
                mChunk[x][z][y] = new GLuint(1); // Par exemple, initialisation à nullptr
                count++;
            }
        }
    }

    // Calculer les faces visibles et les stocker dans mAllVertices
    for (int x = 0; x < CHUNK_SIZE_X; x++) {
        for (int z = 0; z < CHUNK_SIZE_Z; z++) {
            for (int y = 0; y < CHUNK_SIZE_Y; y++) {
                CheckForNeighbors(x, z, y);
            }
        }
    }
}

Chunk::~Chunk()
{
}

void Chunk::CheckForNeighbors(int x, int y, int z)
{
    if (mChunk[x][y][z] == nullptr) return;

    glm::ivec3 directions[6] = {
        {1, 0, 0}, {-1, 0, 0},  // Droite, Gauche
        {0, 1, 0}, {0, -1, 0}, // Haut, Bas
        {0, 0, 1}, {0, 0, -1}  // Avant, Arrière
    };

    for (int i = 0; i < 6; i++) {
        glm::ivec3 neighborPos = glm::ivec3(x, y, z) + directions[i];
        int nx = neighborPos.x, ny = neighborPos.y, nz = neighborPos.z;

        // Vérifier les limites du chunk
        if (nx >= 0 && nx < CHUNK_SIZE_X && ny >= 0 && ny < CHUNK_SIZE_Y && nz >= 0 && nz < CHUNK_SIZE_Z) {
            if (mChunk[nx][nz][ny] == nullptr) {
                // Ajouter les 6 sommets nécessaires pour dessiner une face
                AddFace(x, y, z, directions[i]);
            }
        }
    }
}

void Chunk::AddFace(int x, int y, int z, glm::ivec3 direction)
{
    static const glm::vec3 vertexOffsets[6][4] = {
        // Face droite (+X)
        {{1, 0, 0}, {1, 1, 0}, {1, 1, 1}, {1, 0, 1}},
        // Face gauche (-X)
        {{0, 0, 0}, {0, 0, 1}, {0, 1, 1}, {0, 1, 0}},
        // Face haut (+Y)
        {{0, 1, 0}, {0, 1, 1}, {1, 1, 1}, {1, 1, 0}},
        // Face bas (-Y)
        {{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1}},
        // Face avant (+Z)
        {{0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}},
        // Face arrière (-Z)
        {{0, 0, 0}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0}}
    };

    int faceIndex = -1;
    if (direction == glm::ivec3(1, 0, 0)) faceIndex = 0;   // Droite
    if (direction == glm::ivec3(-1, 0, 0)) faceIndex = 1;  // Gauche
    if (direction == glm::ivec3(0, 1, 0)) faceIndex = 2;   // Haut
    if (direction == glm::ivec3(0, -1, 0)) faceIndex = 3;  // Bas
    if (direction == glm::ivec3(0, 0, 1)) faceIndex = 4;   // Avant
    if (direction == glm::ivec3(0, 0, -1)) faceIndex = 5;  // Arrière

    if (faceIndex >= 0) {
        const glm::vec3* offsets = vertexOffsets[faceIndex];

        // Ajouter les 6 sommets
        mAllVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x + offsets[0].x),
                                             static_cast<uint8_t>(y + offsets[0].y),
                                             static_cast<uint8_t>(z + offsets[0].z)} });
        mAllVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x + offsets[1].x),
                                             static_cast<uint8_t>(y + offsets[1].y),
                                             static_cast<uint8_t>(z + offsets[1].z)} });
        mAllVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x + offsets[2].x),
                                             static_cast<uint8_t>(y + offsets[2].y),
                                             static_cast<uint8_t>(z + offsets[2].z)} });
        mAllVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x + offsets[0].x),
                                             static_cast<uint8_t>(y + offsets[0].y),
                                             static_cast<uint8_t>(z + offsets[0].z)} });
        mAllVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x + offsets[2].x),
                                             static_cast<uint8_t>(y + offsets[2].y),
                                             static_cast<uint8_t>(z + offsets[2].z)} });
        mAllVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x + offsets[3].x),
                                             static_cast<uint8_t>(y + offsets[3].y),
                                             static_cast<uint8_t>(z + offsets[3].z)} });
    }
    // Configurer les attributs de sommet pour `Vertex`
    vao.Bind();
    // Remplir le buffer avec les données des sommets 
    vbo.BufferData(mAllVertices.size() * sizeof(Vertex), mAllVertices.data(), GL_STATIC_DRAW);
    vbo.VertexAttribPointer(0, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    vao.Unbind();

    std::cout << "Nombre de sommets : " << mAllVertices.size() << std::endl;
    std::cout << "Taille des données : " << mAllVertices.size() * sizeof(Vertex) << " octets" << std::endl;
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
    std::cout << mAllVertices[0].position.x;
    if (mAllVertices.size() > 0) {
        // Charger le shader et transmettre les matrices
        mShader->Use();
        glm::mat4 model = glm::translate(glm::mat4(1.0f), mPosition);
        glm::mat4 view = mCamera->GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
        glm::mat4 mvp = projection * view * model;

        mShader->SetMat4("u_MVP", mvp);

        // Dessiner
        vao.Bind();
        glDrawArrays(GL_TRIANGLES, 0, mAllVertices.size());
        vao.Unbind();
    }
}
