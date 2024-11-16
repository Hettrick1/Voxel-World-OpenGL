#pragma once

#include <glad/glad.h>

#include "OpenGL/Shader.h"
#include "OpenGL/Camera.hpp"

#include "OpenGL/VertexBuffer.h"
#include "OpenGL/IndexBuffer.h"
#include "OpenGL/VertexArray.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "OpenGL/imageLoader/stb_image.h"

#include "Utils/Vertex.h"

#include <iostream>
#include <vector>

class Chunk
{
private:
	GLuint* mChunk[16][16][256] = { nullptr };
	Camera* mCamera;
	Shader* shader;
	GLuint texture1, texture2;
	glm::vec3 mPosition;
	std::vector<Vertex> mVertices;

	VertexArray mVAO;
	VertexBuffer mVBO;

	std::uint32_t mVerticesCount;

	glm::vec4 m_TopFace[4];
	glm::vec4 m_BottomFace[4];
	glm::vec4 m_ForwardFace[4];
	glm::vec4 m_BackFace[4];
	glm::vec4 m_LeftFace[4];
	glm::vec4 m_RightFace[4];

public:
	Chunk(Camera* cam, glm::vec3 pos);
	~Chunk();
	void CheckForNeighbors(int x, int y, int z);
	Chunk* GetChunkWithPosition(int x, int y, int z);
	VertexArray GetVAO();
	void ConstructMesh();
	void DrawChunk();
};

