#include "Sky.h"

Sky::Sky(Camera* cam, glm::vec3 pos) : vbo(GL_ARRAY_BUFFER)
{
    mCamera = cam;
    mPosition = pos;

    m_CubeMap.CreateCubeTextureMap({
        "Game/Resources/DAY_CLOUDS_E.png",
        "Game/Resources/DAY_CLOUDS_W.png",
        "Game/Resources/DAY_CLOUDS_T.png",
        "Game/Resources/DAY_CLOUDS_B.png",
        "Game/Resources/DAY_CLOUDS_N.png",
        "Game/Resources/DAY_CLOUDS_S.png"
        });

    mShader = new Shader("Core/Shaders/skyShader.vs", "Core/Shaders/skyShader.fs");
    mShader->Use();

    mAllVertices = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };

    // Multiplication par un facteur pour éloigner la skybox
    for (size_t i = 0; i < mAllVertices.size(); ++i)
    {
        mAllVertices[i] *= 100.0f; // Augmenter la taille du cube
    }

    vao.Bind();
    vbo.BufferData(mAllVertices.size() * sizeof(float), mAllVertices.data(), GL_STATIC_DRAW);
    vbo.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    vao.Unbind();
}

Sky::~Sky()
{
}

void Sky::Draw()
{
    mShader->Use();

    glm::mat4 view = mCamera->GetViewMatrix();
    glm::mat4 projection = mCamera->GetProjectionMatrix();
    glm::mat4 mvp = projection * view;

    mShader->SetMat4("u_MVP", mvp);
    mShader->SetInt("u_Skybox", 0);

    vao.Bind();
    glActiveTexture(GL_TEXTURE0); 
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMap.GetID()); 
    glDrawArrays(GL_TRIANGLES, 0, mAllVertices.size());
    vao.Unbind();

}