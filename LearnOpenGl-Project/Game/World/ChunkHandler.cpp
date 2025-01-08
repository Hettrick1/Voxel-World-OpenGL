#include "ChunkHandler.h"

#include <unordered_set>
#include <tuple>

struct ChunkHash {
	size_t operator()(const std::pair<int, int>& pos) const {
		return std::hash<int>()(pos.first) ^ (std::hash<int>()(pos.second) << 1);
	}
};

struct ChunkEqual {
	bool operator()(const std::pair<int, int>& a, const std::pair<int, int>& b) const {
		return a.first == b.first && a.second == b.second;
	}
};

std::unordered_set<std::pair<int, int>, ChunkHash, ChunkEqual> activeChunks;

ChunkHandler::ChunkHandler(int renderDistance, Camera* cam)
{
	mRenderDistance = renderDistance;
	mCamera = cam;
	mActiveChunks.reserve((renderDistance * 2) * (renderDistance * 2));
	GenerateAllChunks();
}

ChunkHandler::~ChunkHandler()
{
	for (Chunk* chunk : mActiveChunks) {
		delete chunk;
	}
	mActiveChunks.clear();
	activeChunks.clear();
}

void ChunkHandler::GenerateAllChunks()
{
	for (int i = -mRenderDistance; i < mRenderDistance; i++) {
		for (int j = -mRenderDistance; j < mRenderDistance; j++) {
			std::pair<int, int> chunkPosition = { i, j };
			if (activeChunks.find(chunkPosition) == activeChunks.end()) {
				activeChunks.insert(chunkPosition);
				mActiveChunks.push_back(new Chunk(mCamera, glm::vec3(i, j, 0)));
				std::cout << "{ " << i << "," << j << " }" << std::endl;
				std::cout << mActiveChunks.size() << std::endl;
			}
		}
	}
}

void ChunkHandler::UpdateChunks()
{
    // where is the camera in chunk coordinates
    int cameraChunkX = static_cast<int>(std::floor(mCamera->GetPosition().x / 16));
    int cameraChunkY = static_cast<int>(std::floor(mCamera->GetPosition().y / 16));

    for (int dx = -mRenderDistance; dx < mRenderDistance; ++dx) {
        for (int dy = -mRenderDistance; dy < mRenderDistance; ++dy) {
            int chunkX = cameraChunkX + dx;
            int chunkY = cameraChunkY + dy;

            std::pair<int, int> newChunkPosition = { chunkX, chunkY };

            // does the chunk already exist
            if (activeChunks.find(newChunkPosition) == activeChunks.end()) {

                Chunk* newChunk = nullptr;

                if (!mOldChunks.empty()) {
                    newChunk = mOldChunks.back();
                    mOldChunks.pop_back();
                    newChunk->SetPosition(glm::vec3(chunkX, chunkY, 0));
                }
                else { // Create new chunk
                    newChunk = new Chunk(mCamera, glm::vec3(chunkX, chunkY, 0));
                }
                activeChunks.insert(newChunkPosition);
                mActiveChunks.push_back(newChunk);
            }
        }
    }

    // Delete old chunks
    for (auto it = mActiveChunks.begin(); it != mActiveChunks.end();) {
        glm::vec3 pos = (*it)->GetPosition();
        int posChunkX = static_cast<int>(pos.x / 16);
        int posChunkY = static_cast<int>(pos.y / 16);

        int distX = std::abs(cameraChunkX - posChunkX);
        int distY = std::abs(cameraChunkY - posChunkY);

        if (distX > mRenderDistance || distY > mRenderDistance) {
            activeChunks.erase({ posChunkX, posChunkY });
            mOldChunks.push_back(*it);
            it = mActiveChunks.erase(it);
        }
        else {
            ++it;
        }
    }
}

void ChunkHandler::DrawChunks()
{
	for (int i = 0; i < mActiveChunks.size(); i++) {
		mActiveChunks[i]->Draw();
	}
}
