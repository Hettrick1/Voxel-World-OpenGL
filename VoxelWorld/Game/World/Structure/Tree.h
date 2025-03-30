#pragma once
#include "World/Chunk.h"
#include "World/Enums/BlockTextureEnum.h"

// small class to get all the vertices to create a tree
class Tree
{
public:
	Tree(glm::vec3 initialPos, float blockSize, float textureWidth, float heightProbability);
	~Tree();
	void AddLogFace(int x, int y, int z, glm::ivec3 direction);
	void AddLeavesFace(int x, int y, int z, glm::ivec3 direction);
	inline std::vector<Vertex> GetTreeLogVertices() const { return mTreeLogVertices; }
	inline std::vector<Vertex> GetTreeLeavesVertices() const { return mTreeLeavesVertices; }
private:
	std::vector<Vertex> mTreeLogVertices;
	std::vector<Vertex> mTreeLeavesVertices;
	float mBlockSize;
	float mTextureWidth;
	float mTruncHeight;
	uint8_t mLeaves[5][5][6] = { 0 };
};