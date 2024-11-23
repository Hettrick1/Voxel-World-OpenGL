#include "ChunkHandler.h"

ChunkHandler::ChunkHandler(int renderDistance, Camera* cam)
{
	mRenderDistance = renderDistance;
	mCamera = cam;
	mActiveChunks.reserve((renderDistance * 2) * (renderDistance * 2));
	GenerateAllChunks();
}

ChunkHandler::~ChunkHandler()
{
}

void ChunkHandler::GenerateAllChunks()
{
	for (int i = -mRenderDistance; i < mRenderDistance; i++) {
		for (int j = -mRenderDistance; j < mRenderDistance; j++) {
			mActiveChunks.push_back(new Chunk(mCamera, glm::vec3(i, j, 0)));
			std::cout << mActiveChunks.size() << std::endl;
		}
	}
}

void ChunkHandler::UpdateChunks()
{
}

void ChunkHandler::DrawChunks()
{
	for (int i = 0; i < mActiveChunks.size(); i++) {
		mActiveChunks[i]->Draw();
	}
}
