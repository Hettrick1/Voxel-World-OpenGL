#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D u_Texture;

void main()
{
    FragColor = vec4(texture(u_Texture, TexCoord).rgb / 1, 1.0);
}