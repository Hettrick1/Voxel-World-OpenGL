#pragma once

#include <glad/glad.h>

#include "Utils/Defs.h"
#include "Utils/Vertex.h"

#include "OpenGL/Camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <random>

#include "Utils/FastNoiseLite.h"
#include "Enums/BlockTextureEnum.h"

class ChunkInfos
{
private:
	int8_t mChunk[16][16][200] = { -1 };
	Camera* mCamera;
	GLuint mTexture;
	glm::vec3 mPosition;
	std::vector<Vertex> mChunkVertices;
	std::vector<Vertex> mFolliageVertices;
	float mBlockSize;
	float mTextureWidth;
	FastNoiseLite heightMap;
	FastNoiseLite biome;
	bool mIsValid;
public:
	ChunkInfos(Camera* cam, glm::vec3 pos, int seed, GLuint &texture);
	~ChunkInfos();
	void CheckForNeighborBlock(int x, int y, int z);
	void CheckWithNeighborChunk();
	bool CheckForTree(int x, int y, int z);
	void AddFolliage(int x, int y, int z, float probability);
	void AddFace(int x, int y, int z, glm::ivec3 direction, int8_t blockType);
	glm::vec3 GetPosition();
	void SetPosition(glm::vec3 newPos);
	inline Camera* GetCamera() const { return mCamera; }
	inline GLuint& GetTexture() { return mTexture; }
	inline glm::vec3 GetPosition() const { return mPosition; }
	inline std::vector<Vertex>& GetChunkVertices() { return mChunkVertices; }
	inline std::vector<Vertex>& GetFolliageVertices(){ return mFolliageVertices; }
	inline bool GetIsValid() const { return mIsValid; }
};

