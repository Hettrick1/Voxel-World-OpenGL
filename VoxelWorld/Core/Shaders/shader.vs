#version 420 core

// basic vertex shader

layout (location = 0) in vec3 aPos; // bloc position
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in float aTexIndex;
layout(location = 3) in vec3 aNormal;

uniform mat4 u_MVP;

out vec2 TexCoord;
out vec3 vWorldPos;
out vec3 Normal;
flat out int TexIndex;

void main()
{
    gl_Position = u_MVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    vWorldPos = gl_Position.xyz;
    Normal = aNormal;
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
    TexIndex = int(aTexIndex);
}