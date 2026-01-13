#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// set the basic constants of the game and the app

#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 16
#define CHUNK_SIZE_Z 200

#define PI 3.14159265358979323846264338327950288419716939937510582

#define DEFAULT_WINDOW_X 1280
#define DEFAULT_WINDOW_Y 800
#define GL_VERSION_MAJOR 4
#define GL_VERSION_MINOR 5

struct FrustumPlane {
    glm::vec3 normal;
    float d;
};

struct Frustum {
    FrustumPlane planes[6]; // Left, Right, Bottom, Top, Near, Far
};