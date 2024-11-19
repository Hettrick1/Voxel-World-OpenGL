#include "Chunk.h"

Chunk::Chunk(Camera* cam, glm::vec3 pos)
{
	mCamera = cam;
	mPosition.x = pos.x * CHUNK_SIZE_X;
	mPosition.y = 0;
	mPosition.z = pos.z * CHUNK_SIZE_Z;

	int count = 0;
	for (int x = 0; x < CHUNK_SIZE_X; x++) {
		for (int z = 0; z < CHUNK_SIZE_Z; z++) {
			for (int y = 0; y < 12; y++) {
				mChunk[x][z][y] = new GLuint(1);
				count += 1;
				std::cout << count << " : " << *mChunk[x][z][y] << std::endl;
			}
		}
	}
}

Chunk::~Chunk()
{
}

void Chunk::CheckForNeighbors(int x, int y, int z)
{
}

Chunk* Chunk::GetChunkWithPosition(int x, int y, int z)
{
	if (x == mPosition.x / CHUNK_SIZE_X && y == 0 && z == mPosition.z / CHUNK_SIZE_Z) {
		return this;
	}
	return nullptr;
}
