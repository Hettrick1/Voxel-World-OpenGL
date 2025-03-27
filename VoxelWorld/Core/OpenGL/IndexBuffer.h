#pragma once

#include <iostream>
#include <glad/glad.h>
#include <string>

class IndexBuffer
{
public:

	IndexBuffer();
	~IndexBuffer();

	void BufferData(GLsizeiptr size, void* data, GLenum usage);
	void Bind();
	void Unbind();


private:

	GLuint buffer_id;
	GLenum type;
};

