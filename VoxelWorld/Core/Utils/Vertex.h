#pragma once
#include "Defs.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// structure to store all the vertex informations, used to store all the chunk vertices position^s and the texture coordinates.

struct i8Vec3
{
	uint8_t x;
	uint8_t y;
	uint8_t z;

	i8Vec3 operator=(const glm::vec3& vec)
	{
		assert(floor(vec.x) <= CHUNK_SIZE_X);
		assert(floor(vec.y) < 255);
		assert(floor(vec.z) <= CHUNK_SIZE_Z);

		x = floor(vec.x);
		y = floor(vec.y);
		z = floor(vec.z);

		return *this;
	}
};

struct i16Vec2
{
	uint16_t x;
	uint16_t y;
};


struct Vertex
{
	i8Vec3 position;
	i16Vec2 texture_coords;
};