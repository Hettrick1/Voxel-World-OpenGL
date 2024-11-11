#pragma once

#include <glad/glad.h>

#include "OpenGL/Shader.h"
#include "OpenGL/Camera.hpp"
#include "Chunk.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

class ChunkHandler
{
public:
	ChunkHandler(int renderDistance, Camera* cam);
	~ChunkHandler();

	void GenerateAllChunks();
	void UpdateChunks();
	void DrawChunks();
private:
	std::vector<Chunk*> mActiveChunks;
	std::vector<Chunk*> mOldChunks;
	int mRenderDistance;
	Camera* mCamera;
};

