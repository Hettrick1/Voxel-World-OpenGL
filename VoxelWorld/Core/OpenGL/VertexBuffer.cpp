#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(GLenum type) // VBO constuctor
{
	this->buffer_id = 0;
	this->type = type; // set the type
	glGenBuffers(1, &(this->buffer_id));
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &(this->buffer_id));
	this->Unbind();
}


void VertexBuffer::BufferData(GLsizeiptr size, void* data, GLenum usage)
{
	this->Bind();
	glBufferData(this->type, size, data, usage);
}

void VertexBuffer::BufferSubData(GLintptr offset, GLsizeiptr size, void* data)
{
	this->Bind();
	(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
}

void VertexBuffer::Bind()
{
	glBindBuffer(this->type, buffer_id);
}

void VertexBuffer::Unbind()
{
	glBindBuffer(this->type, 0);
}

void VertexBuffer::VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer)
{
	this->Bind();
	glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	glEnableVertexAttribArray(index);
}

void VertexBuffer::VertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
	this->Bind();
	glVertexAttribIPointer(index, size, type, stride, pointer);
	glEnableVertexAttribArray(index);
}
