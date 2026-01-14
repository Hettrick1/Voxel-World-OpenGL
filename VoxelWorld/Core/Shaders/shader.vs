#version 420 core

// basic vertex shader

layout (location = 0) in vec3 aPos; // bloc position
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in float aTexIndex;
layout(location = 3) in vec3 aNormal;

uniform mat4 u_MVP;
uniform mat4 model;
uniform mat4 lightProjection;

out vec2 TexCoord;
out vec3 vWorldPos;
out vec3 Normal;

out vec4 fragPositionLight;

flat out int TexIndex;

void main()
{
    gl_Position = u_MVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    vWorldPos = (model * vec4(aPos, 1.0)).xyz;
    Normal = aNormal;
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
    TexIndex = int(aTexIndex);

    fragPositionLight = lightProjection * vec4(vWorldPos, 1.0);
}