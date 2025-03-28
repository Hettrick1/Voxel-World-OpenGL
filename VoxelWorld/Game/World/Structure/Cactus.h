#pragma once
#include "World/Chunk.h"
#include "World/Enums/BlockTextureEnum.h"

// small class to get all the vertices to create a cactus
class Cactus
{
public:
	Cactus(glm::vec3 initialPos, float blockSize, float textureWidth, float heightProbability);
	~Cactus();
	void AddFace(int x, int y, int z, glm::ivec3 direction);
	inline std::vector<Vertex> GetCactusVertices() const { return mCactusVertices; }
private:
	std::vector<Vertex> mCactusVertices;
	float mBlockSize;
	float mTextureWidth;
	float mCactusHeight;
};
