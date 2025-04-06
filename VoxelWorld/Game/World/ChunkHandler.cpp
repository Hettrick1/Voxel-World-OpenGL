#include "ChunkHandler.h"

#include <unordered_set>
#include <algorithm>

ChunkHandler::ChunkHandler(int renderDistance, Camera* cam, int seed)
{
	mRenderDistance = renderDistance;
	mCamera = cam;
	mActiveChunks.reserve((renderDistance * 2) * (renderDistance * 2));
    mPreloadChunkFactor = 1;
    mSeed = seed;

    std::vector<std::string> textureFiles{
        "Game/Resources/Blocks/GrassTop.png",
        "Game/Resources/Blocks/GrassSide.png",
        "Game/Resources/Blocks/GrassSideShadow.png",
        "Game/Resources/Blocks/Dirt.png",
        "Game/Resources/Blocks/DirtShadow.png",
        "Game/Resources/Blocks/Stone.png",
        "Game/Resources/Blocks/StoneShadow.png",
        "Game/Resources/Blocks/Cobblestone.png",
        "Game/Resources/Blocks/CobblestoneShadow.png",
        "Game/Resources/Blocks/Sand.png",
        "Game/Resources/Blocks/SandShadow.png",
        "Game/Resources/Blocks/Grass.png",
        "Game/Resources/Blocks/Tulip.png",
        "Game/Resources/Blocks/Dandelion.png",
        "Game/Resources/Blocks/Cactus.png",
        "Game/Resources/Blocks/CactusShadow.png",
        "Game/Resources/Blocks/CactusTop.png",
        "Game/Resources/Blocks/OakLeaves.png",
        "Game/Resources/Blocks/OakLeavesShadow.png",
        "Game/Resources/Blocks/OakLogSide.png",
        "Game/Resources/Blocks/OakLogSideShadow.png",
        "Game/Resources/Blocks/OakLogTop.png",
        "Game/Resources/Blocks/DeadBush.png",
    };

    glGenTextures(1, &mTextureArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureArray);

    // Texture parameter
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -4.0f); 

    int width, height, nrChannels; 
    unsigned char* firstImage = stbi_load(textureFiles[0].c_str(), &width, &height, &nrChannels, 4); 
    if (!firstImage) {
        std::cerr << "Erreur chargement texture: " << textureFiles[0] << std::endl;
        return;
    }

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, width, height, NUM_TEXTURES, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr); 

    for (int i = 0; i < textureFiles.size(); ++i) { 
        unsigned char* data = stbi_load(textureFiles[i].c_str(), &width, &height, &nrChannels, 4);
        if (data) {
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cerr << "Erreur chargement texture: " << textureFiles[i] << std::endl;
        }
    }

    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    mIsThreadRunning = true;
    mLoadingThread = std::thread(&ChunkHandler::LoadChunkOnThread, this);
}

ChunkHandler::~ChunkHandler()
{
    mIsThreadRunning = false;
    if (mLoadingThread.joinable())
    {
        mLoadingThread.join();
    }

    for (auto& pair : mActiveChunks) delete pair.second;
    for (auto& pair : mUnactiveChunks) delete pair.second;
    mActiveChunks.clear();
    mUnactiveChunks.clear();
}

void ChunkHandler::UpdateChunks()
{
    glm::vec3 cameraPos = mCamera->GetPosition(); 
    glm::vec3 cameraRot = mCamera->GetForwardVector(); 
    glm::vec2 viewportSize = mCamera->GetCameraSize(); 

    {
        glm::vec3 cameraPos = mCamera->GetPosition();
        int cameraChunkX = static_cast<int>(std::floor(cameraPos.x * 0.0625f));
        int cameraChunkY = static_cast<int>(std::floor(cameraPos.y * 0.0625f));

        {
            std::lock_guard<std::mutex> lock(mMutex);
            mCurrentCamChunkX = cameraChunkX;
            mCurrentCamChunkY = cameraChunkY;
        }
    }
    //std::cout << "x : " << mCurrentCamChunkX << " y : " << mCurrentCamChunkY << std::endl;
    // Chunk generation regarding the position
    for (int i = -mRenderDistance; i <= mRenderDistance; ++i) {
        for (int j = -mRenderDistance; j <= mRenderDistance; ++j) {
            int chunkX = mCurrentCamChunkX + i;
            int chunkY = mCurrentCamChunkY + j;
            GenerateNewChunk(chunkX, chunkY);
        }
    }
    // Delete old chunks
    RemoveOldChunk(mCurrentCamChunkX, mCurrentCamChunkY);

    {
        std::lock_guard<std::mutex> lock(mReadyMutex);
        while (!mChunksReady.empty()) {
            ChunkInfos* chunkInfo = mChunksReady.front();
            if (!chunkInfo->GetChunkVertices().empty()) {
                std::pair<int, int> chunkPosition = { std::floor(chunkInfo->GetPosition().x), std::floor(chunkInfo->GetPosition().y) };
                if (mActiveChunks.find(chunkPosition) == mActiveChunks.end())
                {
                    if (chunkPosition == std::pair<int, int>(32*16, 0))
                    {
                        std::cout << "32/0 Created !!!!" << std::endl;
                    }
                    ChunkMesh* chunk = new ChunkMesh(chunkInfo);
                    mActiveChunks[chunkPosition] = chunk;
                }
            }
            mChunksReady.pop();
        }
    }
    std::cout << " size : " << mChunksToLoad.size() << std::endl;
}

void ChunkHandler::DrawChunks()
{
    std::lock_guard<std::mutex> lock(mMutex);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (auto& pair : mActiveChunks) {
        if (IsChunkInFrustum(pair.second->GetPosition())) {
            pair.second->DrawChunkMesh();
        }
        // Get the chunk position
        glm::vec3 chunkPos = pair.second->GetPosition();

        // Distance between the chunk and the camera
        float distance = glm::length(chunkPos - glm::vec3(mCamera->GetPosition().x, mCamera->GetPosition().y, 0));

        distance *= 0.0625f; // divide by 16

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
        Frustum frustumPlanes = mCamera->GetFrustum();
        glm::vec3 chunkSize = glm::vec3(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z);
        glm::vec3 minCorner = chunkPosition;
        glm::vec3 maxCorner = chunkPosition + chunkSize;

        for (const FrustumPlane& plane : frustumPlanes.planes) {
            bool isOutside = true;
            for (int x = 0; x <= 1; x++) {
                for (int y = 0; y <= 1; y++) {
                    for (int z = 0; z <= 1; z++) {
                        glm::vec3 corner = minCorner + glm::vec3(x * chunkSize.x, y * chunkSize.y, z * chunkSize.z);
                        if (glm::dot(plane.normal, corner) + plane.d >= 0) {
                            isOutside = false;
                            break;
                        }
                    }
                    if (!isOutside) break;
                }
                if (!isOutside) break;
            }
            if (isOutside) return false; // Le chunk est entièrement en dehors du frustum
        }
        return true; // Le chunk est au moins partiellement dans le frustum
    }
}

void ChunkHandler::GenerateNewChunk(int chunkX, int chunkY)
{
    std::pair<int, int> newChunkPosition = { chunkX, chunkY };
    std::lock_guard<std::mutex> lock(mMutex);

    int distX = abs(mCurrentCamChunkX - chunkX);
    int distY = abs(mCurrentCamChunkY - chunkY);

    if (distX > mRenderDistance || distY > mRenderDistance)
        return;

    if (mActiveChunks.find(newChunkPosition) != mActiveChunks.end())
        return;

    if (mUnactiveChunks.find(newChunkPosition) != mUnactiveChunks.end())
    {
        mActiveChunks[newChunkPosition] = mUnactiveChunks[newChunkPosition];
        mUnactiveChunks.erase(newChunkPosition);
        return;
    }

    if (mChunksToLoadSet.find(newChunkPosition) != mChunksToLoadSet.end())
        return;

    {
        std::lock_guard<std::mutex> readyLock(mReadyMutex);

        std::queue<ChunkInfos*> tempQueue = mChunksReady;
        while (!tempQueue.empty())
        {
            ChunkInfos* chunk = tempQueue.front();
            tempQueue.pop();
            std::pair<int, int> chunkPos = { std::floor(chunk->GetPosition().x / 16), std::floor(chunk->GetPosition().y / 16) };
            if (chunkPos == newChunkPosition)
            {
                return;
            }
        }
    }
    mChunksToLoad.push(newChunkPosition);
    mChunksToLoadSet.insert(newChunkPosition);
}

void ChunkHandler::RemoveOldChunk(int cameraChunkX, int cameraChunkY)
{
    // unload the chunks if the camera is too far away
    // still need to delete them from the memory when we are really far away from them
    std::lock_guard<std::mutex> lock(mMutex);
    for (auto it = mActiveChunks.begin(); it != mActiveChunks.end();) {
        glm::vec3 pos = it->second->GetPosition();
        bool isValid = it->second->GetIsValid();
        int posChunkX = static_cast<int>(std::floor(pos.x * 0.0625));
        int posChunkY = static_cast<int>(std::floor(pos.y * 0.0625));

        int distX = std::abs(cameraChunkX - posChunkX);
        int distY = std::abs(cameraChunkY - posChunkY);

        if (distX > mRenderDistance || distY > mRenderDistance) {
            if (isValid) {
                mUnactiveChunks[it->first] = it->second;
            }
            else {
                delete it->second;
            }
            it = mActiveChunks.erase(it);
        }
        else {
            ++it;
        }
    }
}

void ChunkHandler::LoadChunkOnThread()
{
    while (mIsThreadRunning)
    {
        for (int i = 0; i < 1000 && !mChunksToLoad.empty(); ++i) {
            if (!mChunksToLoad.empty())
            {
                std::pair<int, int> chunkPos;
                int currentCamChunkX, currentCamChunkY;
                {
                    std::lock_guard<std::mutex> lock(mMutex);
                    currentCamChunkX = mCurrentCamChunkX;
                    currentCamChunkY = mCurrentCamChunkY;

                    chunkPos = mChunksToLoad.front();

                    int distX = abs(currentCamChunkX - chunkPos.first);
                    int distY = abs(currentCamChunkY - chunkPos.second);

                    if (distX > mRenderDistance || distY > mRenderDistance) {
                        mChunksToLoad.pop();
                        mChunksToLoadSet.erase(chunkPos);
                        continue;
                    }
                    if (mActiveChunks.find(chunkPos) != mActiveChunks.end()) {
                        mChunksToLoad.pop();
                        mChunksToLoadSet.erase(chunkPos);
                        continue;
                    }
                    if(mUnactiveChunks.find(chunkPos) != mUnactiveChunks.end()){
                        mChunksToLoad.pop();
                        mChunksToLoadSet.erase(chunkPos);
                        continue;
                    }
                }
                ChunkInfos* chunk = new ChunkInfos(mCamera, glm::vec3(chunkPos.first, chunkPos.second, 0), mSeed, mTextureArray);
                {
                    std::lock_guard<std::mutex> lock(mReadyMutex);
                    mChunksReady.push(chunk);
                    mChunksToLoad.pop();
                    mChunksToLoadSet.erase(chunkPos);
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}             

