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
	Shader* mShader;
	GLuint mTexture;
	glm::vec3 mPosition;
	std::vector<Vertex> mAllVertices;
	std::vector<Vertex> mTransparentVertices;
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
	void CheckForNeighbors(int x, int y, int z);
	void CheckWithNeighborsChunk();
	Chunk* GetChunkWithPosition(int x, int y, int z);
	void AddFolliage(int x, int y, int z);
	void Draw();
	void DrawTransparent();
	void AddFace(int x, int y, int z, glm::ivec3 direction, int8_t blockType);
	glm::vec3 GetPosition();
	void SetPosition(glm::vec3 newPos);
};

