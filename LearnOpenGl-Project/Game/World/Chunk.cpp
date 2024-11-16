#include "Chunk.h"

Chunk::Chunk(Camera* cam, glm::vec3 pos) : mVBO(GL_ARRAY_BUFFER), mCamera(cam)
{
	static bool IndexBufferInitialized = false;

	static IndexBuffer StaticIBO;

	if (IndexBufferInitialized == false)
	{
		IndexBufferInitialized = true; 

		GLuint* IndexBuffer = nullptr; 

		int index_size = CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z * 6; //number of faces
		int index_offset = 0;

		IndexBuffer = new GLuint[index_size * 6]; //number of vertices

		for (size_t i = 0; i < index_size; i += 6)
		{
			IndexBuffer[i] = 0 + index_offset;
			IndexBuffer[i + 1] = 1 + index_offset;
			IndexBuffer[i + 2] = 2 + index_offset;
			IndexBuffer[i + 3] = 2 + index_offset;
			IndexBuffer[i + 4] = 3 + index_offset;
			IndexBuffer[i + 5] = 0 + index_offset;

			index_offset = index_offset + 4;
		}

		StaticIBO.BufferData(index_size * 6 * sizeof(GLuint), IndexBuffer, GL_STATIC_DRAW);

		delete[] IndexBuffer;
	}

	mVAO.Bind();
	mVBO.Bind();
	StaticIBO.Bind();
	mVBO.VertexAttribIPointer(0, 3, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	mVBO.VertexAttribIPointer(1, 2, GL_UNSIGNED_SHORT, sizeof(Vertex), (void*)offsetof(Vertex, texture_coords));
	mVAO.Unbind();

	mPosition.x = pos.x * CHUNK_SIZE_X;
	mPosition.y = 0;
	mPosition.z = pos.z * CHUNK_SIZE_Z;

	
	m_ForwardFace[0] = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); 
	m_ForwardFace[1] = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f); 
	m_ForwardFace[2] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); 
	m_ForwardFace[3] = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f); 

	m_BackFace[0] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); 
	m_BackFace[1] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); 
	m_BackFace[2] = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); 
	m_BackFace[3] = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); 

	m_TopFace[0] = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); 
	m_TopFace[1] = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	m_TopFace[2] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	m_TopFace[3] = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);

	m_BottomFace[0] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_BottomFace[1] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	m_BottomFace[2] = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	m_BottomFace[3] = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

	m_LeftFace[0] = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
	m_LeftFace[1] = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	m_LeftFace[2] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_LeftFace[3] = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

	m_RightFace[0] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	m_RightFace[1] = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	m_RightFace[2] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	m_RightFace[3] = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);

	texture1 = 0;
	texture2 = 0;

	shader = new Shader("E:/02.Prog/Minecraft-Clone/LearnOpenGl-Project/Core/OpenGL/Shaders/shader.vs", "E:/02.Prog/Minecraft-Clone/LearnOpenGl-Project/Core/OpenGL/Shaders/shader.fs");

	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height, nrChannels;
	unsigned char* data = stbi_load("ressources/textures/container.jpg", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_set_flip_vertically_on_load(true);
	data = stbi_load("ressources/textures/awesomeface.png", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	shader->Use();
	shader->SetInt("texture1", 0);
	shader->SetInt("texture2", 1);

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
	mVertices.clear();
}

void Chunk::CheckForNeighbors(int x, int z, int y)
{
	glm::vec4 translation = glm::vec4(x, z, y, 0.0f);
	if (mChunk[x][z][y] != nullptr) {
		if (z < CHUNK_SIZE_Z - 1 && z > 0) {
			if (mChunk[x][z-1][y] == nullptr) { //back
				Vertex v1, v2, v3, v4;
				v1.position = translation + m_BackFace[0];
				v2.position = translation + m_BackFace[1];
				v3.position = translation + m_BackFace[2];
				v4.position = translation + m_BackFace[3];
				mVertices.push_back(v1);
				mVertices.push_back(v2);
				mVertices.push_back(v3);
				mVertices.push_back(v4);
			}
			else if (mChunk[x][z + 1][y] == nullptr) {
				Vertex v1, v2, v3, v4;
				v1.position = translation + m_ForwardFace[3];
				v2.position = translation + m_ForwardFace[2];
				v3.position = translation + m_ForwardFace[1];
				v4.position = translation + m_ForwardFace[0];
				mVertices.push_back(v1);
				mVertices.push_back(v2);
				mVertices.push_back(v3);
				mVertices.push_back(v4);
			}
		}
		if (y < CHUNK_SIZE_Y - 1 && y > 0) {
			if (mChunk[x][z][y - 1] == nullptr) { //bottom
				Vertex v1, v2, v3, v4;
				v1.position = translation + m_BottomFace[3];
				v2.position = translation + m_BottomFace[2];
				v3.position = translation + m_BottomFace[1];
				v4.position = translation + m_BottomFace[0];
				mVertices.push_back(v1);
				mVertices.push_back(v2);
				mVertices.push_back(v3);
				mVertices.push_back(v4);
			}
			else if (mChunk[x][z][y + 1] == nullptr) {
				Vertex v1, v2, v3, v4;
				v1.position = translation + m_TopFace[0];
				v2.position = translation + m_TopFace[1];
				v3.position = translation + m_TopFace[2];
				v4.position = translation + m_TopFace[3];
				mVertices.push_back(v1);
				mVertices.push_back(v2);
				mVertices.push_back(v3);
				mVertices.push_back(v4);
			}
		}
		if (x < CHUNK_SIZE_X - 1 && x > 0) {
			if (mChunk[x - 1][z][y] == nullptr) { //left
				Vertex v1, v2, v3, v4;
				v1.position = translation + m_LeftFace[3];
				v2.position = translation + m_LeftFace[2];
				v3.position = translation + m_LeftFace[1];
				v4.position = translation + m_LeftFace[0];
				mVertices.push_back(v1);
				mVertices.push_back(v2);
				mVertices.push_back(v3);
				mVertices.push_back(v4);
			}
			else if (mChunk[x + 1][z][y] == nullptr) {
				Vertex v1, v2, v3, v4;
				v1.position = translation + m_RightFace[0];
				v2.position = translation + m_RightFace[1];
				v3.position = translation + m_RightFace[2];
				v4.position = translation + m_RightFace[3];
				mVertices.push_back(v1);
				mVertices.push_back(v2);
				mVertices.push_back(v3);
				mVertices.push_back(v4);
			}
		}
	}
	mVerticesCount = 0;
	if (mVertices.size() > 0)
	{
		mVBO.BufferData(this->mVertices.size() * sizeof(Vertex), &this->mVertices.front(), GL_STATIC_DRAW);
		mVerticesCount = mVertices.size();
		mVertices.clear();
	}
}

Chunk* Chunk::GetChunkWithPosition(int x, int y, int z)
{
	if (x == mPosition.x / CHUNK_SIZE_X && y == 0 && z == mPosition.z / CHUNK_SIZE_Z) {
		return this;
	}
	return nullptr;
}

VertexArray Chunk::GetVAO()
{
	return mVAO;
}

void Chunk::ConstructMesh()
{
	for (int x = 0; x < CHUNK_SIZE_X; x++) {
		for (int z = 0; z < CHUNK_SIZE_Z; z++) {
			for (int y = 0; y < CHUNK_SIZE_Y; y++) {
				CheckForNeighbors(x, z, y);
			}
		}
	}
}

void Chunk::DrawChunk()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	shader->Use();

	//renderer 
	glm::mat4 projection = glm::perspective(glm::radians(mCamera->Zoom), (float)800 / (float)600, 0.1f, 100.0f);
	shader->SetMat4("projection", projection);

	glm::mat4 view = mCamera->GetViewMatrix();
	shader->SetMat4("view", view);
	mVAO.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
