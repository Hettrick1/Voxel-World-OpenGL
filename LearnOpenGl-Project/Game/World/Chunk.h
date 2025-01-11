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

#include "Utils/FastNoiseLite.h"
#include "Enums/BlockTextureEnum.h"

class Chunk
{
private:
	GLuint* mChunk[16][16][200] = { nullptr };
	Camera* mCamera;
	Shader* mShader;
	GLuint mTexture;
	glm::vec3 mPosition;
	std::vector<Vertex> mAllVertices;
	VertexArray vao;
	VertexBuffer vbo;
	float mBlockSize;
	float mTextureWidth;
	FastNoiseLite heightMap;
	FastNoiseLite biome;
public:
	Chunk(Camera* cam, glm::vec3 pos, int seed);
	~Chunk();
	void CheckForNeighbors(int x, int y, int z);
	void CheckWithNeighborsChunk();
	Chunk* GetChunkWithPosition(int x, int y, int z);
	void Draw();
	void AddFace(int x, int y, int z, glm::ivec3 direction, GLuint blockType);
	glm::vec3 GetPosition();
	void SetPosition(glm::vec3 newPos);
};

