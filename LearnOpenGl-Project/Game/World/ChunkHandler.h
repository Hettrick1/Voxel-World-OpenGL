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
#include <unordered_map>
#include <algorithm>

struct ChunkHash { // structure to create a hash of a chunk regarding its position
	size_t operator()(const std::pair<int, int>& pos) const {
		return std::hash<int>()(pos.first) ^ (std::hash<int>()(pos.second) << 1);
	}
};

struct ChunkEqual { // now if two chunk are the same one
	bool operator()(const std::pair<int, int>& a, const std::pair<int, int>& b) const {
		return a.first == b.first && a.second == b.second;
	}
};

class ChunkHandler
{
public:
	ChunkHandler(int renderDistance, Camera* cam, int seed);
	~ChunkHandler();

	void GenerateAllChunks();
	void UpdateChunks();
	void DrawChunks();
	bool IsChunkInFrustum(const glm::vec3& chunkPosition);
	void GenerateNewChunk(int chunkX, int chunkY);
	void RemoveOldChunk(int cameraChunkX, int cameraChunkY);
private:
	int mRenderDistance;
	glm::vec3 mPreviousCameraPosition;
	int mPreloadChunkFactor;
	int mRectWidth;
	int mSeed;
	GLuint mTexture;
	float mBlockSize;
	float mTextureWidth;
	Camera* mCamera;
	std::unordered_map<std::pair<int, int>, Chunk*, ChunkHash, ChunkEqual> mActiveChunks;
	std::unordered_map<std::pair<int, int>, Chunk*, ChunkHash, ChunkEqual> mUnactiveChunks;
};

