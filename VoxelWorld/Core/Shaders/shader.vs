#version 330 core

// basic vertex shader

layout (location = 0) in vec3 aPos; // bloc position
layout(location = 1) in vec2 aTexCoord;

uniform mat4 u_MVP;

out vec2 TexCoord;

void main()
{
    gl_Position = u_MVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}