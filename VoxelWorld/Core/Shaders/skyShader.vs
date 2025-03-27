#version 330 core

// skybox vertex shader

layout (location = 0) in vec3 a_Position;

out vec3 v_TexCoords;

uniform mat4 u_MVP;

void main()
{
    v_TexCoords = a_Position;
    gl_Position = u_MVP * vec4(a_Position.x, a_Position.y, a_Position.z, 1.0);
}  