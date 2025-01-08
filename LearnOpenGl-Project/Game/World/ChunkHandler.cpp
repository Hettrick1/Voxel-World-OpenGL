#include "ChunkHandler.h"

#include <unordered_set>
#include <algorithm>

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
std::unordered_set<std::pair<int, int>, ChunkHash, ChunkEqual> unactiveChunks;

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
    mOldChunks.clear();
    unactiveChunks.clear();
}

void ChunkHandler::GenerateAllChunks()
{
	for (int i = -mRenderDistance * 3; i < mRenderDistance * 3; i++) {
		for (int j = -mRenderDistance * 3; j < mRenderDistance * 3; j++) {
			std::pair<int, int> chunkPosition = { i, j };
			if (activeChunks.find(chunkPosition) == activeChunks.end() && (abs(i) <= mRenderDistance && abs(j) <= mRenderDistance)) {
				activeChunks.insert(chunkPosition);
				mActiveChunks.push_back(new Chunk(mCamera, glm::vec3(i, j, 0)));
				std::cout << "{ " << i << "," << j << " }" << std::endl;
				std::cout << mActiveChunks.size() << std::endl;
			}
            else if (unactiveChunks.find(chunkPosition) == unactiveChunks.end()){
                unactiveChunks.insert(chunkPosition);
                mOldChunks.push_back(new Chunk(mCamera, glm::vec3(i, j, 0)));
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

    glm::vec3 currentPosition = mCamera->GetPosition();
    glm::vec3 direction = currentPosition - mPreviousCameraPosition;
    mPreviousCameraPosition = currentPosition;

    if (glm::length(direction) < 0.01f) {
        return;  // Pas de mouvement détecté, on arrête
    }

    // Normalisation et calcul de l'angle de direction
    direction = glm::normalize(direction);
    float angle = atan2(direction.y, direction.x); // Angle en radians

    int width = 3;  // Largeur du rectangle en chunks
    int height = mRenderDistance*2;

    // Génération de chunks orientée selon la direction
    for (int i = -height / 2; i <= height / 2; ++i) {
        for (int j = -width / 2; j <= width / 2; ++j) {
            // Calcul de la position selon l'angle de la direction
            int offsetX = static_cast<int>(std::round(i * cos(angle) - j * sin(angle)));
            int offsetY = static_cast<int>(std::round(i * sin(angle) + j * cos(angle)));

            int chunkX = cameraChunkX + offsetX;
            int chunkY = cameraChunkY + offsetY;

            GenerateNewChunk(chunkX, chunkY);
        }
    }

    // Suppression des vieux chunks
    RemoveOldChunk(cameraChunkX, cameraChunkY);
    
}

void ChunkHandler::DrawChunks()
{
	for (int i = 0; i < mActiveChunks.size(); i++) {
		mActiveChunks[i]->Draw();
	}
}

void ChunkHandler::GenerateNewChunk(int chunkX, int chunkY)
{
    std::pair<int, int> newChunkPosition = { chunkX, chunkY };

    // does the chunk already exist
    if (activeChunks.find(newChunkPosition) == activeChunks.end() && unactiveChunks.find(newChunkPosition) == unactiveChunks.end()) {
        // Create new chunk
        Chunk* newChunk = nullptr;
        newChunk = new Chunk(mCamera, glm::vec3(chunkX, chunkY, 0));
        activeChunks.insert(newChunkPosition);
        mActiveChunks.push_back(newChunk);
    }
    else if (unactiveChunks.find(newChunkPosition) != unactiveChunks.end()) {
        auto it = std::find_if(mOldChunks.begin(), mOldChunks.end(),
            [chunkX, chunkY](Chunk* chunk) {
                glm::vec3 pos = chunk->GetPosition();
                return static_cast<int>(pos.x / 16) == chunkX && static_cast<int>(pos.y / 16) == chunkY;
            });

        if (it != mOldChunks.end()) {
            activeChunks.insert(newChunkPosition);
            unactiveChunks.erase(newChunkPosition);
            mActiveChunks.push_back(*it);
            mOldChunks.erase(it);
        }
    }
}

void ChunkHandler::RemoveOldChunk(int cameraChunkX, int cameraChunkY)
{
    for (auto it = mActiveChunks.begin(); it != mActiveChunks.end();) {
        glm::vec3 pos = (*it)->GetPosition();
        int posChunkX = static_cast<int>(pos.x / 16);
        int posChunkY = static_cast<int>(pos.y / 16);

        int distX = std::abs(cameraChunkX - posChunkX);
        int distY = std::abs(cameraChunkY - posChunkY);

        if (distX > mRenderDistance || distY > mRenderDistance) {
            activeChunks.erase({ posChunkX, posChunkY });
            unactiveChunks.insert({ posChunkX, posChunkY });  // Ajouter dans la liste des chunks inactifs
            mOldChunks.push_back(*it);
            it = mActiveChunks.erase(it);
        }
        else {
            ++it;
        }
    }
}
