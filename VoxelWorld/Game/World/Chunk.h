#pragma once

#include <glad/glad.h>

#include "OpenGL/imageLoader/stb_image.h"

#include "Utils/Defs.h"
#include "Utils/Vertex.h"

#include "OpenGL/Shader.h"
#include "OpenGL/Camera.hpp"

#include "OpenGL/VertexArray.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/IndexBuffer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <random>

#include "Utils/FastNoiseLite.h"
#include "Enums/BlockTextureEnum.h"

class Chunk
{
private:
	int8_t mChunk[16][16][200] = { -1 };
	Camera* mCamera;
	Shader* mBlockShader;
	Shader* mFolliageShader;
	GLuint mTexture;
	glm::vec3 mPosition;
	std::vector<Vertex> mChunkVertices;
	std::vector<Vertex> mFolliageVertices;
	VertexArray vao;
	VertexBuffer vbo;
	VertexArray transparentVao;
	VertexBuffer transparentVbo;
	float mBlockSize;
	float mTextureWidth;
	FastNoiseLite heightMap;
	FastNoiseLite biome;
public:
	Chunk(Camera* cam, glm::vec3 pos, int seed, GLuint &texture, float &texWidth, float & texHeight);
	~Chunk();
	void CheckForNeighborBlock(int x, int y, int z);
	void CheckWithNeighborChunk();
	bool CheckForTree(int x, int y, int z);
	void AddFolliage(int x, int y, int z, float probability);
	void DrawChunkMesh();
	void DrawFolliageMesh();
	void AddFace(int x, int y, int z, glm::ivec3 direction, int8_t blockType);
	glm::vec3 GetPosition();
	void SetPosition(glm::vec3 newPos);
};

