#version 330 core

//basic fragment shader

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D u_Texture;

void main()
{
    FragColor = texture(u_Texture, TexCoord); // set texture
}