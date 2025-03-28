#pragma once
#include "Defs.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// structure to store all the vertex informations, used to store all the chunk vertices position^s and the texture coordinates.

struct i16Vec3
{
	uint16_t x;
	uint16_t y;
	uint16_t z;

	i16Vec3 operator=(const glm::vec3& vec)
	{
		x = static_cast<int>(floor(vec.x));
		y = static_cast<int>(floor(vec.y));
		z = static_cast<int>(floor(vec.z));

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
	i16Vec3 position;
	i16Vec2 texture_coords;
};