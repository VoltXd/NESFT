#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include <array>
#include <deque>

#include "IO/Shader.hpp"
#include "IO/SoundManager.hpp"
#include "NES/PPU.hpp"
#include "NES/Controller.hpp"

using timeArray_t = std::array<float, BUFFER_SIZE / 2>;

constexpr float NES_ASPECT_RATIO = (float)PPU_OUTPUT_WIDTH / PPU_OUTPUT_HEIGHT;

class GlfwApp
{
public:
    GlfwApp(Controller& controllerRef);
    ~GlfwApp();

    void draw(const picture_t& pictureBuffer);

    void openFile();
    void resetRom();
    void updateControllerState(ControllerInput input, bool isPressed);
    
    inline bool shouldWindowClose() const { return glfwWindowShouldClose(mWindow); }

    inline bool isRomOpened() const { return mIsRomOpened; }
    inline void clearIsRomOpened() { mIsRomOpened = false; }
    inline const std::string& getRomName() const { return mPathToRom; }

    inline bool isSoundChannelsWindowOpen() const { return mIsSoundChannelsWindowOpen; }
    inline bool isSpectrumWindowOpen() const { return mIsSpectrumWindowOpen; }
    inline void setSoundFIFOPtr(const soundFIFO_t* const ptr) { mSoundFIFOPtr = ptr; }
    inline void setP1FIFOPtr(const soundFIFO_t* const ptr) { mP1FIFOPtr = ptr; }
    inline void setP2FIFOPtr(const soundFIFO_t* const ptr) { mP2FIFOPtr = ptr; }
    inline void setTriangleFIFOPtr(const soundFIFO_t* const ptr) { mTriangleFIFOPtr = ptr; }
    inline void setNoiseFIFOPtr(const soundFIFO_t* const ptr) { mNoiseFIFOPtr = ptr; }
    inline void setDmcFIFOPtr(const soundFIFO_t* const ptr) { mDmcFIFOPtr = ptr; }
    inline bool isPaused() const { return mIsPaused; }
    inline void switchPauseState() { mIsPaused = !mIsPaused; }

private:

    // Main menu bar
    void drawMainMenuBar();
    void drawMenuFile();
    void drawMenuWindows();
    void drawMenuDebug();

    // Windows
    void drawEmulatorWindow();
    void drawFrameTimeWindow();
    void drawSoundChannelsWindow();
    void drawSpectrumWindow();

    constexpr timeArray_t calculateTimeArray();
    constexpr soundBufferF32_t calculateFrequencyArray();

    bool mIsRomOpened;
    std::string mPathToRom;

    bool mIsFrameTimeWindowOpen;
    static constexpr u16 FRAMETIME_HISTORY_MAXSIZE = 256;
    std::deque<float> mFrameTimeHistoryDeque;
    std::array<float, FRAMETIME_HISTORY_MAXSIZE> mFrameTimeHistoryArray;

    bool mIsSoundChannelsWindowOpen;
    timeArray_t mTimeArray;
    const soundFIFO_t* mSoundFIFOPtr;
    const soundFIFO_t* mP1FIFOPtr;
    const soundFIFO_t* mP2FIFOPtr;
    const soundFIFO_t* mTriangleFIFOPtr;
    const soundFIFO_t* mNoiseFIFOPtr;
    const soundFIFO_t* mDmcFIFOPtr;
    
    bool mIsSpectrumWindowOpen;
    soundBufferF32_t mFrequencies;

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
