#include "Sky.h"

Sky::Sky(Camera* cam, glm::vec3 pos) : vbo(GL_ARRAY_BUFFER)
{
    mCamera = cam;
    mPosition = pos;

    m_CubeMap.CreateCubeTextureMap({
        "Game/Resources/DAY_CLOUDS_S.jpg",
        "Game/Resources/DAY_CLOUDS_N.jpg",
        "Game/Resources/DAY_CLOUDS_T.jpg",
        "Game/Resources/DAY_CLOUDS_B.jpg",
        "Game/Resources/DAY_CLOUDS_W.jpg",
        "Game/Resources/DAY_CLOUDS_E.jpg"
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
    glDepthMask(GL_FALSE);
    mShader->Use();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.2, 0));
    glm::mat4 view = glm::mat4(glm::mat3(mCamera->GetViewMatrix()));;
    view = glm::rotate(view, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 projection = mCamera->GetProjectionMatrix();
    glm::mat4 mvp = projection * view * model;

    mShader->SetMat4("u_MVP", mvp);
    mShader->SetInt("u_Skybox", 0);

    vao.Bind();
    glActiveTexture(GL_TEXTURE0); 
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMap.GetID()); 
    glDrawArrays(GL_TRIANGLES, 0, 36);
    vao.Unbind();
    glDepthMask(GL_TRUE);
}