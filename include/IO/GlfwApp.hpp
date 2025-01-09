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

using keycode_t = uint16_t;
struct keyboardMap_t
{
    keycode_t a;
    keycode_t b;
    keycode_t start;
    keycode_t select;
    keycode_t up;
    keycode_t down;
    keycode_t left;
    keycode_t right;
};

class GlfwApp
{
public:
    GlfwApp(Controller& controller1Ref, Controller& controller2Ref);
    ~GlfwApp();

    void draw(const picture_t& pictureBuffer);
    bool drawError();

    void openFile();
    void resetRom();
    void prepareControllersState(bool isPlayer1, ControllerInput input, bool isPressed);
    
    inline bool shouldWindowClose() const { return glfwWindowShouldClose(mWindow); }

    inline bool isRomOpened() const { return mIsRomOpened; }
    inline void clearIsRomOpened() { mIsRomOpened = false; }
    inline const std::string& getRomName() const { return mPathToRom; }

    inline float getMasterVolume() const { return mMasterVolume; }
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

    inline keycode_t* getKeyToChange() { return mKeyToChange; }
    inline void clearKeyToChange() { mKeyToChange = nullptr; }

    inline void setHeaderInfo(const std::string& headerInfo) { mHeaderInfo = headerInfo; }
    inline void setErrorMessage(const std::string& errorMessage) { mErrorMessage = errorMessage; }

private:
    // Initialisation
    void initAudio();
    void initVideo();
    void initInputs();

    void initVao();
    void initShader();
    void initTexture(GLuint& textureObject);
    void initFbo();

    // Main menu bar
    void drawMainMenuBar();
    void drawMenuFile();
    void drawMenuWindows();
    void drawMenuSettings();
    void drawMenuDebug();
    void drawMenuHelp();

    // Windows
    void drawEmulatorWindow();
    void drawFrameTimeWindow();
    void drawSoundChannelsWindow();
    void drawSpectrumWindow();
    void drawAudioSettingsWindow();
    void drawVideoSettingsWindow();
    void drawInputSettingsWindow();
    void drawKeyMapping(const char* buttonStr, keycode_t* key);
    void drawHeaderInfoWindow();
    void drawAboutWindow();
    bool drawErrorWindow();

    void updateFiltering(uint8_t filteringIdx);
    bool updateShader(uint8_t shaderIdx);

    void pollKeyboard();
    void pollGamepads();
    void updateControllersState();

    constexpr timeArray_t calculateTimeArray();
    constexpr soundBufferF32_t calculateFrequencyArray();

    bool mIsRomOpened;
    std::string mPathToRom;
    std::string mErrorMessage;

    static constexpr u16 FRAMETIME_HISTORY_MAXSIZE = 256;
    bool mIsFrameTimeWindowOpen;
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

    bool mIsAudioSettingsWindowOpen;
    float mMasterVolume;

    bool mIsVideoSettingsWindowOpen;
    static constexpr const char* FILTERING_ITEMS[] = 
    {
        "Nearest",
        "Linear"
    };
    const char* mCurrentFiltering;
    static constexpr const char* SHADER_ITEMS[] = 
    {
        "Default",
        "Custom",
        "Negative",
        "Grayscale",
        "Sharpen",
        "Edges"
    };
    const char* mCurrentShaderStr;
    std::string mShaderErrorMessage;
    
    bool mIsInputSettingsWindowOpen;
    bool mIsKeyboardEnabled;
    bool mIsKeyboardPlayer1Selected;
    keyboardMap_t mKeyboardMapping;
    keycode_t* mKeyToChange;

    bool mAreGamepadsEnabled[16];
    bool mAreGamepadsPlayer1Selected[16];
    bool mAreGamepadsLayoutAlternative[16];
    float mGamepadsDeadZone[16];

    bool mIsHeaderInfoWindowOpen;
    std::string mHeaderInfo;

    bool mIsAboutWindowOpen;

    GLFWwindow* mWindow;

    static constexpr float SCREEN_VERTICES[] = 
    {
        // Position     // TexCoords
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f,
        -1.0f,  1.0f,   0.0f, 1.0f
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
    GLuint mScreenFbo;
    GLuint mPixelTexture;
    GLuint mScreenTexture;
    std::unique_ptr<Shader> mScreenShader;

    Controller& mController1Ref;
    Controller& mController2Ref;
    uint8_t mController1State;
    uint8_t mController2State;

    bool mIsPaused;
};
