#pragma once

#include <glad/glad.h>

#include "OpenGL/Shader.h"
#include "OpenGL/Camera.hpp"
#include "Chunk.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "OpenGL/FrameData.h"
#include "OpenGL/FrameUboOpenGL.h"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <queue>
#include <mutex>

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
private:
	Shader* mBlockShader;
	Shader* mFolliageShader;
	Shader* mShadowMapShader;
	int mRenderDistance;
	glm::vec3 mPreviousCameraPosition;
	int mPreloadChunkFactor;
	int mRectWidth;
	int mSeed;
	GLuint mTextureArray;
	float mBlockSize;
	float mTextureWidth;
	const int NUM_TEXTURES = 23;
	Camera* mCamera;
	std::unordered_map<std::pair<int, int>, Chunk*, ChunkHash, ChunkEqual> mActiveChunks;
	std::unordered_map<std::pair<int, int>, Chunk*, ChunkHash, ChunkEqual> mUnactiveChunks;

	FrameData frameData = FrameData();
	FrameUboOpenGL frameUBO;
	
	unsigned int mShadowMapFBO = 0;
	unsigned int mShadowMapTexture = 0;
	glm::ivec2 mShadowMapResolution = {2048, 2048};
	glm::mat4 mShadowProjectionMatrix;
	glm::mat4 mLightViewMatrix;
	glm::mat4 mLightProjMatrix;
	
public:
	ChunkHandler(int renderDistance, Camera* cam, int seed);
	~ChunkHandler();


	void InitShadowPass();
	void GenerateAllChunks();
	void UpdateChunks();
	void Draw();
	void ShadowPass();
	void LightPass();
	bool IsChunkInFrustum(const glm::vec3& chunkPosition);
	void GenerateNewChunk(int chunkX, int chunkY);
	void RemoveOldChunk(int cameraChunkX, int cameraChunkY);
};

