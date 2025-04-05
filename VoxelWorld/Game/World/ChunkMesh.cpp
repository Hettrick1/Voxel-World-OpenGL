#include "ChunkMesh.h"

ChunkMesh::ChunkMesh(ChunkInfos* pChunkInfos)
    :mVbo(GL_ARRAY_BUFFER), mTransparentVbo(GL_ARRAY_BUFFER), mChunkInfos(pChunkInfos), mIsValid(false)
{
    mCamera = pChunkInfos->GetCamera();
    mPosition.x = pChunkInfos->GetPosition().x;
    mPosition.y = pChunkInfos->GetPosition().y;
    mPosition.z = 0;
    mTexture = pChunkInfos->GetTexture();

    mBlockShader = new Shader("Core/Shaders/shader.vs", "Core/Shaders/shader.fs");
    mFolliageShader = new Shader("Core/Shaders/folliageShader.vs", "Core/Shaders/folliageShader.fs");

    mChunkVertices = pChunkInfos->GetChunkVertices();
    mFolliageVertices = pChunkInfos->GetFolliageVertices();

    mVao.Bind();
    mVbo.BufferData(mChunkVertices.size() * sizeof(Vertex), mChunkVertices.data(), GL_STATIC_DRAW);
    mVbo.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    mVbo.VertexAttribPointer(1, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coords));
    mVbo.VertexAttribPointer(2, 1, GL_INT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexIndex));
    mVao.Unbind();

    mTransparentVao.Bind();
    mTransparentVbo.BufferData(mFolliageVertices.size() * sizeof(Vertex), mFolliageVertices.data(), GL_STATIC_DRAW);
    mTransparentVbo.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    mTransparentVbo.VertexAttribPointer(1, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coords));
    mTransparentVbo.VertexAttribPointer(2, 1, GL_INT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexIndex));
    mTransparentVao.Unbind();
    mIsValid = true;
}

ChunkMesh::~ChunkMesh()
{
    delete mBlockShader;
    delete mFolliageShader;
}

void ChunkMesh::DrawChunkMesh()
{
    if (mChunkVertices.size() > 0) {
        // Use shader and send all the matrices
        mBlockShader->Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, mTexture);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), mPosition);
        glm::mat4 view = mCamera->GetViewMatrix();
        glm::mat4 projection = mCamera->GetProjectionMatrix();
        glm::mat4 mvp = projection * view * model;

        mBlockShader->SetMat4("u_MVP", mvp);
        // Draw the chunk
        mVao.Bind();
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mChunkVertices.size()));
        mVao.Unbind();
    }
}

void ChunkMesh::DrawFolliageMesh()
{
    if (mFolliageVertices.size() > 0) {

        mFolliageShader->Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mTexture);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), mPosition);
        glm::mat4 view = mCamera->GetViewMatrix();
        glm::mat4 projection = mCamera->GetProjectionMatrix();
        glm::mat4 mvp = projection * view * model;

        mFolliageShader->SetMat4("u_MVP", mvp);
        mTransparentVao.Bind();
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mFolliageVertices.size()));
        mTransparentVao.Unbind();
    }
}

glm::vec3 ChunkMesh::GetPosition()
{
    return mPosition;
}

void ChunkMesh::SetPosition(glm::vec3 newPos)
{
    mPosition.x = newPos.x * 16;
    mPosition.y = newPos.y * 16;
    mPosition.z = 0;
}
