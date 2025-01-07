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
			std::cout << "{ " << i << "," << j << " }" << std::endl;
			std::cout << mActiveChunks.size() << std::endl;
		}
	}
}

void ChunkHandler::UpdateChunks()
{
	//std::cout << "x : " << mCamera->GetPosition().x << "\ny : " << mCamera->GetPosition().y << "\nz : " << mCamera->GetPosition().z << std::endl << std::endl;

	glm::vec3 newChunkPosition[6];
	glm::vec3 chunkToCreate[6];
	float distance = 0.0;

	if (mCamera->GetPosition().x < 0) distance = -mRenderDistance;
	else distance = mRenderDistance;
	newChunkPosition[0] = {round(mCamera->GetPosition().x / 16 + (distance * mCamera->GetForwardVector().x)), round(mCamera->GetPosition().y / 16 + (distance * mCamera->GetForwardVector().y)), 0};
	newChunkPosition[1] = { newChunkPosition[0].x - 1, newChunkPosition[0].y, 0 };
	newChunkPosition[2] = { newChunkPosition[0].x + 1, newChunkPosition[0].y, 0 };
	newChunkPosition[3] = { newChunkPosition[0].x, newChunkPosition[0].y - 1, 0 };
	newChunkPosition[4] = { newChunkPosition[0].x - 1, newChunkPosition[0].y - 1, 0 };
	newChunkPosition[5] = { newChunkPosition[0].x + 1, newChunkPosition[0].y - 1, 0 };

	for (int j = 0; j < sizeof(chunkToCreate) / sizeof(chunkToCreate[0]); j++) {
		chunkToCreate[j] = newChunkPosition[j];
	}

	for (int i = 0; i < mActiveChunks.size(); i++) {
		for (int j = 0; j < 6; j++) {
			if (mActiveChunks[i]->GetPosition().x / 16 == newChunkPosition[j].x && mActiveChunks[i]->GetPosition().y / 16 == newChunkPosition[j].y) {
				chunkToCreate[j] = glm::vec3(0, 0, 0);
				return;
			}
		}
	}
	for (int j = 0; j < sizeof(chunkToCreate) / sizeof(chunkToCreate[0]); j++) {
		if (chunkToCreate[j] != glm::vec3(0, 0, 0)) {
			mActiveChunks.push_back(new Chunk(mCamera, glm::vec3(chunkToCreate[j].x, chunkToCreate[j].y, 0)));
		}
	}
}

void ChunkHandler::DrawChunks()
{
	for (int i = 0; i < mActiveChunks.size(); i++) {
		mActiveChunks[i]->Draw();
	}
}
