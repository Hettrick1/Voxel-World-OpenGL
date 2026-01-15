#version 420 core

//basic fragment shader

out vec4 FragColor;

in vec3 vWorldPos;
in vec2 TexCoord;
in vec3 Normal;

in vec4 fragPositionLight;

flat in int TexIndex;

uniform sampler2DArray textureArray;
uniform sampler2D shadowMap;

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

    float dist = length(vWorldPos - uCameraPos.xyz);
    vec3 newBaseColor = baseColor.rgb;

    float fogFactor = clamp((dist - uFogStart.x) / (uFogEnd.x - uFogStart.x), 0.0, 1.0);
    //float fogFactor = 1.0;

    vec3 N = normalize(Normal);
    vec3 L = normalize(-uSkyLightDirection.xyz);

    vec3 ambient = uSkyLightColor.xyz * 0.4;
    float diff = max(dot(L, N), 0.0);
    vec3 diffuse = uSkyLightColor.xyz * diff;

    float shadow = 0.0f;
    vec3 lightCoords = fragPositionLight.xyz / fragPositionLight.w;
    if (lightCoords.z <= 1.0f)
    {
        lightCoords = lightCoords * 0.5 +0.5;

        float closestDepth = texture(shadowMap, lightCoords.xy).r;
        float currentDepth = lightCoords.z;
        
        float bias = max(0.05 * (1.0 - dot(N, L)), 0.005);  

        if (currentDepth - bias > closestDepth)
        {
            shadow = 1.0f;
        }
    }

    // total lighting
    vec3 litColor = (ambient + (1.0 - shadow) * diffuse) * newBaseColor;

    // fog
    vec3 finalColor = mix(litColor, uFogColor.xyz, fogFactor);

    FragColor = vec4(vec3(texture(shadowMap, TexCoord).r), 1.0);
    FragColor = vec4(finalColor, 1.0);
}