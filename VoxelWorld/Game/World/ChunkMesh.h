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
#include <random>

#include "Utils/FastNoiseLite.h"
#include "Enums/BlockTextureEnum.h"
#include "Chunk.h"

class ChunkMesh
{
private:
	ChunkInfos* mChunkInfos;
	Camera* mCamera;
	Shader* mBlockShader;
	Shader* mFolliageShader;
	GLuint mTexture;
	glm::vec3 mPosition;
	VertexArray mVao;
	VertexBuffer mVbo;
	VertexArray mTransparentVao;
	VertexBuffer mTransparentVbo;
	std::vector<Vertex> mChunkVertices;
	std::vector<Vertex> mFolliageVertices;
	bool mIsValid;
public:
	ChunkMesh(ChunkInfos* pChunkInfos);
	~ChunkMesh();
	void DrawChunkMesh();
	void DrawFolliageMesh();
	glm::vec3 GetPosition();
	void SetPosition(glm::vec3 newPos);
	inline bool GetIsValid() const { return mIsValid; }
};