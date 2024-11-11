#pragma once

#include <glad/glad.h>

#include "OpenGL/Shader.h"
#include "OpenGL/Camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

class Chunk
{
private:
	const int SIZE_X = 16;
	const int SIZE_Y = 16;
	const int SIZE_Z = 256;
	GLuint* mChunk[16][16][256] = { nullptr };
	Camera* mCamera;
	Shader* mShader;
	glm::vec3 mPosition;
	std::vector<float> mAllVertices;
public:
	Chunk(Camera* cam, glm::vec3 pos);
	~Chunk();
	void CheckForNeighbors(int x, int y, int z);
	Chunk* GetChunkWithPosition(int x, int y, int z);
};

