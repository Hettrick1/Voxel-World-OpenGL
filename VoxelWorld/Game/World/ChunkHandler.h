#pragma once

#include <glad/glad.h>

#include "OpenGL/imageLoader/stb_image.h"

#include "OpenGL/Shader.h"
#include "OpenGL/Camera.hpp"
#include "Chunk.h"
#include "ChunkMesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <mutex>

struct ChunkHash {
	template <class T1, class T2>
	std::size_t operator()(const std::pair<T1, T2>& p) const {
		auto hash1 = std::hash<T1>{}(p.first);
		auto hash2 = std::hash<T2>{}(p.second);
		return hash1 ^ (hash2 << 1);
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

	void LoadChunkOnThread(); // load chunk on another thread
private:
	std::queue<std::pair<int, int>> mChunksToLoad;
	std::unordered_set<std::pair<int, int>, ChunkHash, ChunkEqual> mChunksToLoadSet;
	std::queue<ChunkInfos*> mChunksReady;
	std::mutex mMutex;
	std::mutex mReadyMutex;
	std::thread mLoadingThread;
	std::atomic<bool> mIsThreadRunning;

	int mCurrentCamChunkX;
	int mCurrentCamChunkY;
	int mRenderDistance;
	glm::vec3 mPreviousCameraPosition;
	int mPreloadChunkFactor;
	int mSeed;
	GLuint mTextureArray;
	const int NUM_TEXTURES = 23;
	Camera* mCamera;
	std::unordered_map<std::pair<int, int>, ChunkMesh*, ChunkHash, ChunkEqual> mActiveChunks;
	std::unordered_map<std::pair<int, int>, ChunkMesh*, ChunkHash, ChunkEqual> mUnactiveChunks;
};

