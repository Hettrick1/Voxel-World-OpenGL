#version 330 core

//basic fragment shader

out vec4 FragColor;
in vec2 TexCoord;
flat in int TexIndex;

uniform sampler2DArray textureArray;

void main()
{
    FragColor = texture(textureArray, vec3(TexCoord, TexIndex)); // set texture
    if(FragColor.a < 0.2) discard;
}