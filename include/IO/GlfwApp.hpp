#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <memory>
#include <string>
#include <array>
#include <deque>

#include "IO/Shader.hpp"
#include "IO/SoundManager.hpp"
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
    
    inline bool shouldWindowClose() const { return glfwWindowShouldClose(mWindow); }

    inline bool isRomOpened() const { return mIsRomOpened; }
    inline void clearIsRomOpened() { mIsRomOpened = false; }
    inline const std::string& getRomName() const { return mPathToRom; }

    inline bool isSoundChannelsWindowOpen() const { return mIsSoundChannelsWindowOpen; }
    inline void setSoundBufferPtr(const soundBufferF32_t* const ptr) { mSoundBufferPtr = ptr; }
    inline void setP1BufferPtr(const soundBufferF32_t* const ptr) { mP1BufferPtr = ptr; }
    inline void setP2BufferPtr(const soundBufferF32_t* const ptr) { mP2BufferPtr = ptr; }
    inline void setTriangleBufferPtr(const soundBufferF32_t* const ptr) { mTriangleBufferPtr = ptr; }
    inline void setNoiseBufferPtr(const soundBufferF32_t* const ptr) { mNoiseBufferPtr = ptr; }
    inline void setDmcBufferPtr(const soundBufferF32_t* const ptr) { mDmcBufferPtr = ptr; }
    inline bool isPaused() const { return mIsPaused; }
    inline void switchPauseState() { mIsPaused = !mIsPaused; }

private:
    // Main menu bar
    void drawMainMenuBar();
    void drawMenuFile();
    void drawMenuWindows();

    // Windows
    void drawEmulatorWindow();
    void drawFrameTimeWindow();
    void drawSoundChannelsWindow();
    void drawSpectrumWindow();

    bool mIsRomOpened;
    std::string mPathToRom;

    bool mIsFrameTimeWindowOpen;
    static constexpr u16 FRAMETIME_HISTORY_MAXSIZE = 256;
    std::deque<float> mFrameTimeHistoryDeque;
    std::array<float, FRAMETIME_HISTORY_MAXSIZE> mFrameTimeHistoryArray;

    bool mIsSoundChannelsWindowOpen;
    const soundBufferF32_t* mSoundBufferPtr;
    const soundBufferF32_t* mP1BufferPtr;
    const soundBufferF32_t* mP2BufferPtr;
    const soundBufferF32_t* mTriangleBufferPtr;
    const soundBufferF32_t* mNoiseBufferPtr;
    const soundBufferF32_t* mDmcBufferPtr;
    
    bool mIsSpectrumWindowOpen;

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
    bool mIsPaused;
};
