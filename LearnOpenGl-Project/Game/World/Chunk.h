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
	const int sizeX = 16;
	const int sizeY = 16;
	const int sizeZ = 256;
	GLuint* chunk[16][16][256] = { nullptr };
	Camera* camera;
	Shader* shader;
	glm::vec3 position;
	std::vector<float> allVertices = { nullptr };
public:
	Chunk(Camera* cam, glm::vec3 pos);
	~Chunk();
	void CheckForNeighbors(int x, int y, int z);
};

