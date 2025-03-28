#pragma once
#include "Defs.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// structure to store all the vertex informations, used to store all the chunk vertices position^s and the texture coordinates.

struct fVec3
{
	float x;
	float y;
	float z;

	fVec3 operator=(const glm::vec3& vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;

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
	fVec3 position;
	i16Vec2 texture_coords;
};