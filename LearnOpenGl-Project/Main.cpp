#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Core/OpenGL/Shader.h"
#include "Core/OpenGL/Camera.hpp"
#include "Game/World/ChunkHandler.h"
#include "World/Sky.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <iostream>

#include "Core/Utils/Defs.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);



// create the camera
Camera* camera = new Camera(glm::vec3(0.0f, 0.0f, 100.0f));
float lastX = DEFAULT_WINDOW_X / 2.0f;
float lastY = DEFAULT_WINDOW_Y / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
int frameCount = 0;
std::chrono::time_point<std::chrono::system_clock> lastTime = std::chrono::system_clock::now();


void calculateFPS(GLFWwindow* window) {
    auto currentTime = std::chrono::system_clock::now();

    frameCount++;

    std::chrono::duration<double> elapsedTime = currentTime - lastTime;

    // Print FPS every seconds
    if (elapsedTime.count() >= 1.0) {
        std::cout << "FPS: " << frameCount << std::endl;
        glfwSetWindowTitle(window, ("FPS : " + std::to_string(frameCount)).c_str());
        frameCount = 0;
        lastTime = currentTime;
    }
}


int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(DEFAULT_WINDOW_X, DEFAULT_WINDOW_Y, "Voxel World", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }  

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //Wireframe Mode
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // create the chunkHandler and the skybox -> don't need to be a pointer ?
    ChunkHandler* chunkHandler = new ChunkHandler(16, camera, 12345);
    Sky* skybox = new Sky(camera, camera->GetPosition());

    while (!glfwWindowShouldClose(window)) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        calculateFPS(window);

        //inputs
        processInput(window);

        chunkHandler->UpdateChunks();

        //renderer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw skybox first
        skybox->Draw();
        chunkHandler->DrawChunks();

        //swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    delete camera;
    delete chunkHandler;
    delete skybox;
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    float cameraSpeed = 10 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->ProcessKeyboard(FORWARD, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->ProcessKeyboard(BACKWARD, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->ProcessKeyboard(LEFT, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->ProcessKeyboard(RIGHT, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
        glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, 1920, 1080, 165);
        glViewport(0, 0, 1920, 1080);
        camera->SetCameraSize(1920, 1080);
    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera->ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera->ProcessMouseScroll(static_cast<float>(yoffset));
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glfwGetWindowSize(window, &width, &height);
    glfwSetWindowAspectRatio(window, width, height);
    camera->SetCameraSize(width, height);
    glViewport(0, 0, width, height);
}
