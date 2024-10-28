#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <memory>
#include <array>

#include "IO/Shader.hpp"
#include "NES/PPU.hpp"

class GlfwRenderer
{
public:
    GlfwRenderer();
    ~GlfwRenderer();

    void draw(const picture_t& pictureBuffer);

private:
    GLFWwindow* mWindow;

    int mWindowWidth = 1280;
    int mWindowHeight = 720;

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

    uint32_t mScreenVao;
    uint32_t mScreenVbo;
    uint32_t mScreenEbo;
    uint32_t mScreenTexture;
    std::unique_ptr<Shader> mScreenShader;
};
