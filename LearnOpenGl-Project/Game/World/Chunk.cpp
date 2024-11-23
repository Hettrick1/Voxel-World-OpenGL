#include "Chunk.h"

Chunk::Chunk(Camera* cam, glm::vec3 pos)
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
            for (int y = 0; y < CHUNK_SIZE_Y; y++) {
                mChunk[x][z][y] = nullptr; // Par exemple, initialisation à nullptr
                count++;
            }
        }
    }

    // Calculer les faces visibles et les stocker dans mAllVertices
    for (int x = 0; x < CHUNK_SIZE_X; x++) {
        for (int z = 0; z < CHUNK_SIZE_Z; z++) {
            for (int y = 0; y < CHUNK_SIZE_Y; y++) {
                CheckForNeighbors(x, y, z);
            }
        }
    }
}

Chunk::~Chunk()
{
}

void Chunk::CheckForNeighbors(int x, int y, int z)
{
    if (mChunk[x][z][y] == nullptr) return;

    glm::ivec3 directions[6] = {
        {1, 0, 0}, {-1, 0, 0},  // Droite, Gauche
        {0, 1, 0}, {0, -1, 0}, // Haut, Bas
        {0, 0, 1}, {0, 0, -1}  // Avant, Arrière
    };

    for (int i = 0; i < 6; i++) {
        glm::ivec3 neighborPos = glm::ivec3(x, y, z) + directions[i];
        int nx = neighborPos.x, ny = neighborPos.y, nz = neighborPos.z;

        // Vérifier les limites du chunk
        if (nx >= 0 && nx < 16 && ny >= 0 && ny < 256 && nz >= 0 && nz < 16) {
            if (mChunk[nx][nz][ny] == nullptr) {
                if (directions[i] == glm::ivec3(1, 0, 0)) {  // Face droite
                    mAllVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x + 1),
                                                          static_cast<uint8_t>(y),
                                                          static_cast<uint8_t>(z)} });
                }
                else if (directions[i] == glm::ivec3(-1, 0, 0)) {  // Face gauche
                    mAllVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x - 1),
                                                          static_cast<uint8_t>(y),
                                                          static_cast<uint8_t>(z)} });
                }
                else if (directions[i] == glm::ivec3(0, 1, 0)) {  // Face dessus
                    mAllVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x),
                                                          static_cast<uint8_t>(y + 1),
                                                          static_cast<uint8_t>(z)} });
                }
                else if (directions[i] == glm::ivec3(0, -1, 0)) {  // Face dessous
                    mAllVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x),
                                                          static_cast<uint8_t>(y - 1),
                                                          static_cast<uint8_t>(z)} });
                }
                else if (directions[i] == glm::ivec3(0, 0, 1)) {  // Face avant
                    mAllVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x),
                                                          static_cast<uint8_t>(y),
                                                          static_cast<uint8_t>(z + 1)} });
                }
                else if (directions[i] == glm::ivec3(0, 0, -1)) {  // Face arrière
                    mAllVertices.push_back(Vertex{ i8Vec3{static_cast<uint8_t>(x),
                                                          static_cast<uint8_t>(y),
                                                          static_cast<uint8_t>(z - 1)} });
                }
            }
        }
    }
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
    // Crée et configure le VAO et le VBO si nécessaire
    static VertexArray vao;
    static VertexBuffer vbo(GL_ARRAY_BUFFER);

    if (mAllVertices.size() > 0) {
        // Remplir le buffer avec les données des sommets 
        vbo.BufferData(mAllVertices.size() * sizeof(Vertex), mAllVertices.data(), GL_STATIC_DRAW);

        // Configurer les attributs de sommet pour `Vertex`
        vao.Bind();
        vbo.VertexAttribIPointer(0, 3, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        vao.Unbind();

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
