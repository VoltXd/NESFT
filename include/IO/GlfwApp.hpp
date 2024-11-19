#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <memory>
#include <array>

#include "IO/Shader.hpp"
#include "NES/PPU.hpp"
#include "NES/Controller.hpp"

constexpr float NES_ASPECT_RATIO = 256.0f / 240;

class GlfwApp
{
public:
    GlfwApp(Controller& controllerRef);
    ~GlfwApp();

    void draw(const picture_t& pictureBuffer);

    void updateControllerState(ControllerInput input, bool isPressed);
    
    bool shouldWindowClose() { return glfwWindowShouldClose(mWindow); }
    
private:
    void drawEmulatorWindow();

    GLFWwindow* mWindow;

    static constexpr float SCREEN_VERTICES[] = 
    {
        // Position     // TexCoords
        -1.0f, -1.0f,   0.0f, 1.0f,
         1.0f, -1.0f,   1.0f, 1.0f,
         1.0f,  1.0f,   1.0f, 0.0f,
        -1.0f,  1.0f,   0.0f, 0.0f
    };

    static constexpr unsigned int SCREEN_INDICES[] = 
    {
        0, 1, 2,
        0, 2, 3
    };

    static constexpr float BORDER_COLOR[] = 
    { 
        0.0f, 0.0f, 0.0f, 1.0f 
    };

    GLuint mScreenVao;
    GLuint mScreenVbo;
    GLuint mScreenEbo;
    GLuint mScreenTexture;
    std::unique_ptr<Shader> mScreenShader;

    Controller& mControllerRef;
};
