#include "Chunk.h"

Chunk::Chunk(Camera* cam, glm::vec3 pos)
{
	mCamera = cam;
	mPosition.x = pos.x * SIZE_X;
	mPosition.y = 0;
	mPosition.z = pos.z * SIZE_Y;

	int count = 0;
	for (int x = 0; x < SIZE_X; x++) {
		for (int y = 0; y < SIZE_Y; y++) {
			for (int z = 0; z < 12; z++) {
				mChunk[x][y][z] = new GLuint(1);
				count += 1;
				std::cout << count << " : " << *mChunk[x][y][z] << std::endl;
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
	if (x == mPosition.x / SIZE_X && y == 0 && z == mPosition.z / SIZE_Y) {
		return this;
	}
	return nullptr;
}
