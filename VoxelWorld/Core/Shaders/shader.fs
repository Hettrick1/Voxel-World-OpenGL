#version 420 core

//basic fragment shader

out vec4 FragColor;

in vec3 vWorldPos;
in vec2 TexCoord;
in vec3 Normal;
flat in int TexIndex;

uniform sampler2DArray textureArray;

layout(std140, binding = 0) uniform FrameData
{
   vec4 uCameraPos;

   vec4 uTime;
   vec4 uScreenWidth;
   vec4 uScreenHeight;
   vec4 uFogStart;
   vec4 uFogEnd;

   vec4 uFogColor;

   vec4 uSkyLightColor;
   vec4 uSkyLightIntensity;
   vec4 uSkyLightDirection;
};


void main()
{
    vec4 baseColor = texture(textureArray, vec3(TexCoord, TexIndex)); // set texture
    if(baseColor.a < 0.2) discard;

    vec3 newBaseColor = baseColor.rgb;

    // distance caméra -> fragment
    float dist = length(vWorldPos - uCameraPos.xyz);

    // fog factor linéaire : 0 = avant fogStart, 1 = après fogEnd
    float fogFactor = clamp((dist - uFogStart.x) / (uFogEnd.x - uFogStart.x), 0.0, 1.0);
    //float fogFactor = 1.0;
    // mix texture avec fogColor

    vec3 N = normalize(Normal);
    vec3 L = normalize(uSkyLightDirection.xyz);

    vec3 ambient = newBaseColor;
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = newBaseColor * uSkyLightColor.xyz * diff * uSkyLightIntensity.x;

    // total lighting
    vec3 litColor = ambient + diffuse;

    // fog
    vec3 finalColor = mix(litColor, uFogColor.xyz, fogFactor);

    FragColor = vec4(finalColor, 1.0);
}