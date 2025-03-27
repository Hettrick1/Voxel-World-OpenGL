#pragma once

#include <iostream>
#include <glad/glad.h>
#include <string>

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void Bind();
	void Unbind();

private:

	GLuint array_id;
	GLenum type;
};

