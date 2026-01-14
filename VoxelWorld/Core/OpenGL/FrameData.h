#pragma once

#include <glm/glm.hpp>

struct FrameData
{
    FrameData():
    cameraPos(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)),
    time(0.0f),
    screenWidth(1920.0f),
    screenHeight(1920.0f),
    fogStart(200.0f),
    fogEnd(1000.0f),
    fogColor(glm::vec4(0.5647f, 0.8352f, 1.0f, 1.0f)),
    skyLightColor(glm::vec4(1.0f, 1.0f, 0.8f, 0.0f)),
    skyLightIntensity(glm::vec4(4.0f)),
    skyLightDirection(glm::vec4(0.0f, -0.2f, -0.8f, 1.0f))
    {
        
    }
    
    glm::vec4 cameraPos;

    glm::vec4 time;        
    glm::vec4 screenWidth;
    glm::vec4 screenHeight;
    glm::vec4 fogStart;    
    glm::vec4 fogEnd;      

    glm::vec4 fogColor;

    glm::vec4 skyLightColor;
    glm::vec4 skyLightIntensity;
    glm::vec4 skyLightDirection;
};