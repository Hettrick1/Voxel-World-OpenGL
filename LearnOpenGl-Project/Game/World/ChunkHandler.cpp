#include "ChunkHandler.h"

#include <unordered_set>
#include <algorithm>

//std::unordered_set<std::pair<int, int>, ChunkHash, ChunkEqual> activeChunks;
//std::unordered_set<std::pair<int, int>, ChunkHash, ChunkEqual> unactiveChunks;

ChunkHandler::ChunkHandler(int renderDistance, Camera* cam, int seed)
{
	mRenderDistance = renderDistance;
	mCamera = cam;
	mActiveChunks.reserve((renderDistance * 2) * (renderDistance * 2));
    mPreloadChunkFactor = 1;
    mRectWidth = 16;
    mSeed = seed;
    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);

    // Texture parameter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // load the texture of the blocks
    int width, height, nrChannels;
    unsigned char* data = stbi_load("Game/Resources/368x16_sheet.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        mBlockSize = height;
        mTextureWidth = width;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Erreur : " << stbi_failure_reason() << std::endl;
    }
    stbi_image_free(data);
	GenerateAllChunks();
}

ChunkHandler::~ChunkHandler()
{
    for (auto& pair : mActiveChunks) delete pair.second;
    for (auto& pair : mUnactiveChunks) delete pair.second;
    mActiveChunks.clear();
    mUnactiveChunks.clear();
}

void ChunkHandler::GenerateAllChunks()
{
    // generate chunks regarding render distance, we can set a preload option to optimize the early game
	for (int i = -mRenderDistance * mPreloadChunkFactor; i < mRenderDistance * mPreloadChunkFactor; i++) {
		for (int j = -mRenderDistance * mPreloadChunkFactor; j < mRenderDistance * mPreloadChunkFactor; j++) {
			std::pair<int, int> chunkPosition = { i, j };
            // if the chunk is in the render distance
			if (mActiveChunks.find(chunkPosition) == mActiveChunks.end() && (abs(i) <= mRenderDistance && abs(j) <= mRenderDistance)) {
                mActiveChunks[chunkPosition] = new Chunk(mCamera, glm::vec3(i, j, 0), mSeed, mTexture, mTextureWidth, mBlockSize);
			}
            // Add the chunk into the unactive chunk if we preload the chunk and the chunk is outside the render distance
            else if (mUnactiveChunks.find(chunkPosition) == mUnactiveChunks.end()){
                mUnactiveChunks[chunkPosition] = new Chunk(mCamera, glm::vec3(i, j, 0), mSeed, mTexture, mTextureWidth, mBlockSize);
            }
		}
	}
}

void ChunkHandler::UpdateChunks()
{
    glm::vec3 cameraPos = mCamera->GetPosition(); 
    glm::vec3 cameraRot = mCamera->GetForwardVector(); 
    glm::vec2 viewportSize = mCamera->GetCameraSize(); 

    // where is the camera in chunk coordinates
    int cameraChunkX = static_cast<int>(std::floor(cameraPos.x / 16));
    int cameraChunkY = static_cast<int>(std::floor(cameraPos.y / 16));

    glm::vec3 currentPosition = mCamera->GetPosition();
    glm::vec3 direction = currentPosition - mPreviousCameraPosition;
    mPreviousCameraPosition = currentPosition;

    if (glm::length(direction) < 0.01f) { // stops if the player doesn't move
        return;
    }

    // direction angle calcul
    direction = glm::normalize(direction);
    float angle = atan2(direction.y, direction.x);

    // width of the rectangle
    int height = mRenderDistance*2;

    // Chunk generation regarding the position
    for (int i = -height / 2; i <= height / 2; ++i) {
        for (int j = -mRectWidth / 2; j <= mRectWidth / 2; ++j) {
            // Calcul de la position selon l'angle de la direction
            int offsetX = static_cast<int>(std::round(i * cos(angle) - j * sin(angle)));
            int offsetY = static_cast<int>(std::round(i * sin(angle) + j * cos(angle)));

            int chunkX = cameraChunkX + offsetX;
            int chunkY = cameraChunkY + offsetY;

            GenerateNewChunk(chunkX, chunkY);
        }
    }
    // Delete old chunks
    RemoveOldChunk(cameraChunkX, cameraChunkY); 
}

void ChunkHandler::DrawChunks()
{
    for (auto& pair : mActiveChunks) {
        if (IsChunkInFrustum(pair.second->GetPosition())) {
            pair.second->DrawChunkMesh();
        }
        // Get the chunk position
        glm::vec3 chunkPos = pair.second->GetPosition();

        // Distance between the chunk and the camera
        float distance = glm::length(chunkPos - glm::vec3(mCamera->GetPosition().x, mCamera->GetPosition().y, 0));

        distance /= 16;

        // This is the folliage render distance, if the distance between the chunk and the camera is smaller than 8 we draw the folliage
        if (distance < 8 && IsChunkInFrustum(chunkPos)) {
            pair.second->DrawFolliageMesh();
        }
    }
}

bool ChunkHandler::IsChunkInFrustum(const glm::vec3& chunkPosition)
{
    Frustum frustumPlanes = mCamera->GetFrustum();
    glm::vec3 chunkSize = glm::vec3(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z);

    for (const FrustumPlane& plane : frustumPlanes.planes) {
        int outsideCount = 0;
        for (int x = -1; x <= 1; x += 2) {
            for (int y = -1; y <= 1; y += 2) {
                for (int z = -1; z <= 1; z += 2) {
                    glm::vec3 corner = chunkPosition + glm::vec3(x, y, z) * chunkSize;
                    if (glm::dot(plane.normal, corner) + plane.d < 0) {
                        outsideCount++;
                    }
                    else {
                        outsideCount = -1;
                        break;
                    }
                }
                if (outsideCount == -1)
                {
                    break;
                }
            }
            if (outsideCount == -1)
            {
                break;
            }
        }
        if (outsideCount == 8) {
            return false; // chunk is outside the frustum
        }
    }
    return true; // chunk is at least a bit in the frustum
}

void ChunkHandler::GenerateNewChunk(int chunkX, int chunkY)
{
    std::pair<int, int> newChunkPosition = { chunkX, chunkY };

    // does NOT the chunk already exist
    if (mActiveChunks.find(newChunkPosition) == mActiveChunks.end() && mUnactiveChunks.find(newChunkPosition) == mUnactiveChunks.end()) {
        // Create new chunk
        mActiveChunks[newChunkPosition] = new Chunk(mCamera, glm::vec3(chunkX, chunkY, 0), mSeed, mTexture, mTextureWidth, mBlockSize);
    }
    // if the chunk already exists we just retreve is from the oldChunk vector
    else if (mUnactiveChunks.find(newChunkPosition) != mUnactiveChunks.end())
    {
        // Déplacer le chunk de la liste inactive à active
        mActiveChunks[newChunkPosition] = mUnactiveChunks[newChunkPosition];
        mUnactiveChunks.erase(newChunkPosition);
    }
}

void ChunkHandler::RemoveOldChunk(int cameraChunkX, int cameraChunkY)
{
    // unload the chunks if the camera is too far away
    // still need to delete them from the memory when we are really far away from them
    for (auto it = mActiveChunks.begin(); it != mActiveChunks.end();) {
        glm::vec3 pos = it->second->GetPosition();
        int posChunkX = static_cast<int>(pos.x / CHUNK_SIZE_X);
        int posChunkY = static_cast<int>(pos.y / CHUNK_SIZE_Y);

        int distX = std::abs(cameraChunkX - posChunkX);
        int distY = std::abs(cameraChunkY - posChunkY);

        if (distX > mRenderDistance || distY > mRenderDistance) {
            mUnactiveChunks[it->first] = it->second; // add to unactive chunk list
            it = mActiveChunks.erase(it); 
        }
        else {
            ++it;
        }
    }
}
