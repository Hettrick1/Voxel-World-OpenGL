#pragma once

#include <glad/glad.h>

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

class Chunk
{
private:
	GLuint* mChunk[16][16][255] = { nullptr };
	Camera* mCamera;
	Shader* mShader;
	glm::vec3 mPosition;
	std::vector<Vertex> mAllVertices;
	VertexArray vao;
	VertexBuffer vbo;
public:
	Chunk(Camera* cam, glm::vec3 pos);
	~Chunk();
	void CheckForNeighbors(int x, int y, int z);
	Chunk* GetChunkWithPosition(int x, int y, int z);
	void Draw();
	void AddFace(int x, int y, int z, glm::ivec3 direction);
};

