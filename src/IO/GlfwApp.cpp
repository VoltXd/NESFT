#include "IO/GlfwApp.hpp"

#include <iostream>

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

GlfwApp::GlfwApp(Controller& controllerRef)
    : mControllerRef(controllerRef)
{
    // Window size
    mWindowHeight;
    mWindowWidth;

    // Init GLFW + Version + Core profile
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Window
    mWindow = glfwCreateWindow(mWindowWidth, mWindowHeight, "Simple NES Emulator", nullptr, nullptr);
    if (mWindow == nullptr)
    {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(mWindow);

    // Init GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialise GLAD!" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Tells openGL the position & size of the view port
    int windowWidth4_3 = (int)(mWindowHeight * (256.0 / 240));
    int windowXOffset = (mWindowWidth - windowWidth4_3) / 2; 
    glViewport(windowXOffset, 0, windowWidth4_3, mWindowHeight);

    // Enable keyboard events
    glfwSetWindowUserPointer(mWindow, this);
    glfwSetKeyCallback(mWindow, keyCallback);

    // Screen VAO, VBO & EBO
    // VAO
    glGenVertexArrays(1, &mScreenVao);
    glBindVertexArray(mScreenVao);

    // VBO
    glGenBuffers(1, &mScreenVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mScreenVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SCREEN_VERTICES), SCREEN_VERTICES, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // EBO
    glGenBuffers(1, &mScreenEbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mScreenEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(SCREEN_INDICES), SCREEN_INDICES, GL_STATIC_DRAW);

    // Unbind buffers
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Shaders
    mScreenShader = std::make_unique<Shader>("shaders/screen.vert", "shaders/screen.frag");
    mScreenShader->use();
    mScreenShader->setInt("screenTexture", 0);

    // Screen texture (ToDo: PBO & FBO ?)
    glGenTextures(1, &mScreenTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mScreenTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BORDER_COLOR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, PPU_OUTPUT_WIDTH, PPU_OUTPUT_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);

    // OpenGL settings
    glEnable(GL_CULL_FACE);
}

GlfwApp::~GlfwApp()
{
    glDeleteVertexArrays(1, &mScreenVao);
    glDeleteBuffers(1, &mScreenVbo);
    glDeleteBuffers(1, &mScreenEbo);
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void GlfwApp::draw(const picture_t& pictureBuffer)
{
    // Clear frame buffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Bind screen vao & shader
    mScreenShader->use();
    glBindVertexArray(mScreenVao);  

    // Update screen texture
    // TODO: implement with TexSubImage2D, FBO & PBO
    // SubImage
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mScreenTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, PPU_OUTPUT_WIDTH, PPU_OUTPUT_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pictureBuffer.data()->data()->data());

    // Draw screen
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mScreenEbo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    // Render
    glfwSwapBuffers(mWindow);
    glfwPollEvents();
}

void GlfwApp::updateControllerState(ControllerInput input, bool isPressed)
{
    mControllerRef.updateControllerState(input, isPressed);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    GlfwApp* renderer = reinterpret_cast<GlfwApp*>(glfwGetWindowUserPointer(window));
    if (renderer)
    {
        // Handle keyboard press
        bool isPressed;
        ControllerInput input;

        if (mods != 0x0000)
            // No input if ALT, CTRL, Super...
            return;
        scancode; // Useless line to remove warning

        // Window
        if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, true);

        // Controller 1
        if (action == GLFW_PRESS)
            isPressed = true;
        else if (action == GLFW_RELEASE)
            isPressed = false;
        else 
            return;

        if (key == GLFW_KEY_K)
            input = ControllerInput::A;
        else if  (key == GLFW_KEY_J)
            input = ControllerInput::B;
        else if  (key == GLFW_KEY_G)
            input = ControllerInput::SELECT;
        else if  (key == GLFW_KEY_H)
            input = ControllerInput::START;
        else if  (key == GLFW_KEY_W)
            input = ControllerInput::UP;
        else if  (key == GLFW_KEY_S)
            input = ControllerInput::DOWN;
        else if  (key == GLFW_KEY_A)
            input = ControllerInput::LEFT;
        else if  (key == GLFW_KEY_D)
            input = ControllerInput::RIGHT;
        else
            return;
        
        renderer->updateControllerState(input, isPressed);
    }
}