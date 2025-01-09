#include "IO/GlfwApp.hpp"

#include "imgui.h"
#include "implot.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "NES/Toolbox.hpp"

#include <nfd.h>
#include <iostream>
#include <vector>
#include <algorithm>

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void resizeCallback(GLFWwindow* window, int windowWidth, int windowHeight);

GlfwApp::GlfwApp(Controller& controller1Ref, Controller& controller2Ref)
    : mController1Ref(controller1Ref), mController2Ref(controller2Ref)
{
    initAudio();
    initVideo();
    initInputs();

    // Window size
    constexpr int windowWidth = 1280;
    constexpr int windowHeight = 720;

    // Init GLFW + Version + Core profile
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Window
    mWindow = glfwCreateWindow(windowWidth, windowHeight, "NESFT", nullptr, nullptr);
    if (mWindow == nullptr)
    {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(mWindow);
    glfwSwapInterval(0); // Disable VSync

    // Init GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialise GLAD!" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Tells openGL the position & size of the view port
    resizeCallback(mWindow, windowWidth, windowHeight);

    // Enable keyboard & resize callbacks
    glfwSetWindowUserPointer(mWindow, this);
    glfwSetKeyCallback(mWindow, keyCallback);
    glfwSetWindowSizeCallback(mWindow, resizeCallback);

    // Init OpenGL objects
    initVao();
    initShader();
    initTexture(mPixelTexture);
    initTexture(mScreenTexture);
    initFbo();
    
    // OpenGL settings
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    // GUI variables
    mIsRomOpened = false;
    mIsFrameTimeWindowOpen = false;
    mFrameTimeHistoryDeque.resize(FRAMETIME_HISTORY_MAXSIZE);

    mIsSoundChannelsWindowOpen = false;
    mTimeArray = calculateTimeArray();
    mSoundFIFOPtr = nullptr;
    mP1FIFOPtr = nullptr;
    mP2FIFOPtr = nullptr;
    mTriangleFIFOPtr = nullptr;
    mNoiseFIFOPtr = nullptr;
    mDmcFIFOPtr = nullptr;

    mIsSpectrumWindowOpen = false;
    mFrequencies = calculateFrequencyArray();
    mIsLogScale = false;

    mIsHeaderInfoWindowOpen = false;

    mIsAboutWindowOpen = false;

    mIsAudioSettingsWindowOpen = false;
    mIsVideoSettingsWindowOpen = false;
    mIsInputSettingsWindowOpen = false;

    mIsPaused = false; 

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}

GlfwApp::~GlfwApp()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glDeleteFramebuffers(1, &mScreenFbo);
    glDeleteVertexArrays(1, &mScreenVao);
    glDeleteBuffers(1, &mScreenVbo);
    glDeleteBuffers(1, &mScreenEbo);
    glDeleteTextures(1, &mScreenTexture);
    glDeleteTextures(1, &mPixelTexture);
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void GlfwApp::initAudio()
{
    mMasterVolume = 1.0f;
}

void GlfwApp::initVideo()
{
    mCurrentFiltering = FILTERING_ITEMS[1];
    mCurrentShaderStr = SHADER_ITEMS[0];
}

void GlfwApp::initInputs()
{
    // Keyboard
    mIsKeyboardEnabled = true;
    mIsKeyboardPlayer1Selected = true;
    mKeyToChange = nullptr;
    mKeyboardMapping.a = GLFW_KEY_K;
    mKeyboardMapping.b = GLFW_KEY_J;
    mKeyboardMapping.start = GLFW_KEY_H;
    mKeyboardMapping.select = GLFW_KEY_G;
    mKeyboardMapping.up = GLFW_KEY_W;
    mKeyboardMapping.down = GLFW_KEY_S;
    mKeyboardMapping.left = GLFW_KEY_A;
    mKeyboardMapping.right = GLFW_KEY_D;

    // Gamepads
    for (int i = 0; i < 16; i++)
    {
        mGamepadsDeadZone[i] = 0.2f;
        mAreGamepadsEnabled[i] = true;
        mAreGamepadsPlayer1Selected[i] = true;
        mAreGamepadsLayoutAlternative[i] = false;
    }
}
void GlfwApp::initVao()

{
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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GlfwApp::initShader()
{
    mScreenShader = std::make_unique<Shader>(VERT_SHADER_DEFAULT, FRAG_SHADER_DEFAULT);
    mScreenShader->use();
    mScreenShader->setInt("screenTexture", 0);
}

void GlfwApp::initTexture(GLuint& textureObject)
{
    glGenTextures(1, &textureObject);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureObject);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BORDER_COLOR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, PPU_OUTPUT_WIDTH, PPU_OUTPUT_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void GlfwApp::initFbo()
{
    glGenFramebuffers(1, &mScreenFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, mScreenFbo);

    // Color
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mScreenTexture, 0);

    // Final check
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "FBO INCOMPLETE!" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GlfwApp::draw(const picture_t &pictureBuffer)
{
    // Reset controllers;
    mController1State = 0;
    mController2State = 0;

    // Poll events
    glfwPollEvents();
    pollGamepads();
    pollKeyboard();

    // Update controllers
    updateControllersState();

    // Clear frame buffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Apply shader
    glBindFramebuffer(GL_FRAMEBUFFER, mScreenFbo);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Update screen texture
    // SubImage
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mPixelTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, PPU_OUTPUT_WIDTH, PPU_OUTPUT_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pictureBuffer.data()->data()->data());

    // Bind screen vao & shader
    mScreenShader->use();
    mScreenShader->setInt("screenTexture", 0);
    mScreenShader->setFloat("time", (float)glfwGetTime());
    glBindVertexArray(mScreenVao);       
    glViewport(0, 0, PPU_OUTPUT_WIDTH, PPU_OUTPUT_HEIGHT);   
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ImGuiID dockSpaceID = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

    // Draw main menu bar
    drawMainMenuBar();

    // Draw screen texture to ImGUI window
    ImGui::SetNextWindowDockID(dockSpaceID, ImGuiCond_Once);
    drawEmulatorWindow();

    // Draw frame timing window if opened
    if (mIsFrameTimeWindowOpen)
        drawFrameTimeWindow();

    // Draw sound channel window if opened
    if (mIsSoundChannelsWindowOpen)
        drawSoundChannelsWindow();

    // Draw spectrum window if opened
    if (mIsSpectrumWindowOpen)
        drawSpectrumWindow();

    if (mIsAudioSettingsWindowOpen)
        drawAudioSettingsWindow();

    if (mIsVideoSettingsWindowOpen)
        drawVideoSettingsWindow();

    if (mIsInputSettingsWindowOpen)
        drawInputSettingsWindow();

    // Draw header info window if opened
    if (mIsHeaderInfoWindowOpen)
        drawHeaderInfoWindow();

    if (mIsAboutWindowOpen)
        drawAboutWindow();

    // Render
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(mWindow);
}

bool GlfwApp::drawError()
{
    bool isErrorWindowClosed = false;

    // Poll events
    glfwPollEvents();

    // Clear frame buffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiID dockSpaceID = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
    
    ImGui::SetNextWindowDockID(dockSpaceID, ImGuiCond_Once);
    isErrorWindowClosed = drawErrorWindow();

    // Render
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(mWindow);

    return isErrorWindowClosed;
}

void GlfwApp::prepareControllersState(bool isPlayer1, ControllerInput input, bool isPressed)
{
    if (isPlayer1)
        mController1State |= isPressed ? input : 0;
    else
        mController2State |= isPressed ? input : 0;
}

void GlfwApp::drawMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        drawMenuFile();
        drawMenuWindows();
        drawMenuSettings();
        drawMenuDebug();
        drawMenuHelp();

        ImGui::EndMainMenuBar();
    }
}

void GlfwApp::drawMenuFile()
{
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("Open", "Ctrl+O"))
        {
            // Open NES ROM
            openFile();
        }

        if (ImGui::MenuItem("Reset", "Ctrl+R"))
        {
            // Open NES ROM
            resetRom();
        }

        if (ImGui::MenuItem("Exit", "Alt+F4"))
        {
            // Exit program
            glfwSetWindowShouldClose(mWindow, true);
        }

        ImGui::EndMenu();
    }
}

void GlfwApp::drawMenuWindows()
{
    if (ImGui::BeginMenu("Windows"))
    {
        ImGui::MenuItem("Frame timing", nullptr, &mIsFrameTimeWindowOpen);
        ImGui::MenuItem("Sound channels", nullptr, &mIsSoundChannelsWindowOpen);
        ImGui::MenuItem("Spectrum", nullptr, &mIsSpectrumWindowOpen);

        ImGui::EndMenu();
    }
}

void GlfwApp::drawMenuSettings()
{
    if (ImGui::BeginMenu("Settings"))
    {
        ImGui::MenuItem("Audio", nullptr, &mIsAudioSettingsWindowOpen);
        ImGui::MenuItem("Video", nullptr, &mIsVideoSettingsWindowOpen);
        ImGui::MenuItem("Input", nullptr, &mIsInputSettingsWindowOpen);
        
        ImGui::EndMenu();
    }
}

void GlfwApp::drawMenuDebug()
{
    if (ImGui::BeginMenu("Debug"))
    {
        ImGui::MenuItem("CPU trace log", nullptr, &gIsTraceLogCpuEnabled);
        ImGui::MenuItem("PPU trace log", nullptr, &gIsTraceLogPpuEnabled);
        ImGui::MenuItem("MMC3 IRQ trace log", nullptr, &gIsTraceLogMMC3IrqEnabled);
        
        ImGui::EndMenu();
    }
}

void GlfwApp::drawMenuHelp()
{
    if (ImGui::BeginMenu("Help"))
    {
        ImGui::MenuItem("ROM Info", nullptr, &mIsHeaderInfoWindowOpen);
        ImGui::MenuItem("About", nullptr, &mIsAboutWindowOpen);
        
        ImGui::EndMenu();
    }
}

void GlfwApp::drawEmulatorWindow()
{
    if (ImGui::Begin("Emulator"))
    {
        // Get top left position window position
        ImVec2 windowTopLeft = ImGui::GetCursorScreenPos();
        ImVec2 windowBotRight;

        // Get current window size
        const float windowWidth = ImGui::GetContentRegionAvail().x;
        const float windowHeight = ImGui::GetContentRegionAvail().y;

        // Calculate NES window positions 
        if (windowWidth / windowHeight > NES_ASPECT_RATIO)
        {
            // Width is too big
            float windowWidthRatioed = windowHeight * NES_ASPECT_RATIO;
            float windowXOffset = (windowWidth - windowWidthRatioed) / 2; 

            // Top left
            windowTopLeft.x += windowXOffset;
            
            // Bottom right
            windowBotRight.x = windowTopLeft.x + windowWidthRatioed;
            windowBotRight.y = windowTopLeft.y + windowHeight;
        }
        else
        {
            // Height is too big
            float windowHeightRatioed = windowWidth * (1.0f / NES_ASPECT_RATIO);
            float windowYOffset = (windowHeight - windowHeightRatioed) / 2; 

            // Top left
            windowTopLeft.y += windowYOffset;
            
            // Bottom right
            windowBotRight.x = windowTopLeft.x + windowWidth;
            windowBotRight.y = windowTopLeft.y + windowHeightRatioed;
        }

        // Send NES image texture to window
        ImGui::GetWindowDrawList()->AddImage(mScreenTexture, 
                                            windowTopLeft, 
                                            windowBotRight,
                                            ImVec2(0, 0),
                                            ImVec2(1, 1));

    }
    ImGui::End();
}

void GlfwApp::drawFrameTimeWindow()
{
    if (ImGui::Begin("Frame timing"))
    {
        float deltaTimeMs = ImGui::GetIO().DeltaTime * 1000;
        float currentFreq = 1.0f / ImGui::GetIO().DeltaTime;
        // History vector full -> pop first data
        mFrameTimeHistoryDeque.pop_front();
        mFrameTimeHistoryDeque.push_back(deltaTimeMs);

        // Copy to vector
        std::copy(mFrameTimeHistoryDeque.begin(), 
                  mFrameTimeHistoryDeque.end(), 
                  mFrameTimeHistoryArray.begin());

        ImGui::Text("Time between frames: %.1f ms (%.1f Hz)", deltaTimeMs, currentFreq);
        // ImGui::PlotLines("Frame timing", mFrameTimeHistoryArray.data(), (int)mFrameTimeHistoryArray.size(), 0, nullptr, 0, 30, {0.0f, 0.0f});
        if (ImPlot::BeginPlot("Frame timing", { -1, -1 }))
        {
            ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoTickLabels);
            ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_Lock);
            ImPlot::SetupAxesLimits(0, (double)mFrameTimeHistoryArray.size(), 0.0f, 30.0f);
            ImPlot::PlotLine("Frame time", mFrameTimeHistoryArray.data(), (int)mFrameTimeHistoryArray.size());

            ImPlot::EndPlot();
        }
    }
    ImGui::End();   
}

void GlfwApp::drawSoundChannelsWindow()
{
    if (ImGui::Begin("Sound channels"))
    {
        // TODO: Plot the 5 sound channels
        // TODO: Add a button to plot mixed channels ?
        soundBufferF32_t buffer;
        s32 offset;
        if (ImPlot::BeginSubplots("##NoTitle", 6, 1, { -1, -1}))
        {
            if (ImPlot::BeginPlot("Sound output", { -1, -1 }))
            {
                ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoTickLabels);
                ImPlot::SetupAxesLimits(mTimeArray.front(), mTimeArray.back(), -0.1f, 1);
                if (mSoundFIFOPtr != nullptr)
                {
                    std::copy(mSoundFIFOPtr->begin(), mSoundFIFOPtr->end(), buffer.begin());
                    offset = getScopeTriggerOffset(buffer);
                    ImPlot::PlotLine("Sound output", mTimeArray.data(), buffer.data() + offset, (int)mTimeArray.size());
                }
                ImPlot::EndPlot();
            }

            if (ImPlot::BeginPlot("Pulse 1", { -1, -1 }))
            {
                ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoTickLabels);
                ImPlot::SetupAxesLimits(mTimeArray.front(), mTimeArray.back(), -0.1f, 1);
                if (mP1FIFOPtr != nullptr)
                {
                    std::copy(mP1FIFOPtr->begin(), mP1FIFOPtr->end(), buffer.begin());
                    offset = getScopeTriggerOffset(buffer);
                    ImPlot::PlotLine("Pulse 1", mTimeArray.data(), buffer.data() + offset, (int)mTimeArray.size());
                }
                ImPlot::EndPlot();
            }
                    
            if (ImPlot::BeginPlot("Pulse 2", { -1, -1 }))
            {
                ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoTickLabels);
                ImPlot::SetupAxesLimits(mTimeArray.front(), mTimeArray.back(), -0.1f, 1);
                if (mP2FIFOPtr != nullptr)
                {
                    std::copy(mP2FIFOPtr->begin(), mP2FIFOPtr->end(), buffer.begin());
                    offset = getScopeTriggerOffset(buffer);
                    ImPlot::PlotLine("Pulse 2", mTimeArray.data(), buffer.data() + offset, (int)mTimeArray.size());
                }
                ImPlot::EndPlot();
            }
                    
            if (ImPlot::BeginPlot("Triangle", { -1, -1 }))
            {
                ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoTickLabels);
                ImPlot::SetupAxesLimits(mTimeArray.front(), mTimeArray.back(), -0.1f, 1);
                if (mTriangleFIFOPtr != nullptr)
                {
                    std::copy(mTriangleFIFOPtr->begin(), mTriangleFIFOPtr->end(), buffer.begin());
                    offset = getScopeTriggerOffset(buffer);
                    ImPlot::PlotLine("Triangle", mTimeArray.data(), buffer.data() + offset, (int)mTimeArray.size());
                }
                ImPlot::EndPlot();
            }
            
            if (ImPlot::BeginPlot("Noise", { -1, -1 }))
            {
                ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoTickLabels);
                ImPlot::SetupAxesLimits(mTimeArray.front(), mTimeArray.back(), -0.1f, 1);
                if (mNoiseFIFOPtr != nullptr)
                {
                    std::copy(mNoiseFIFOPtr->begin(), mNoiseFIFOPtr->end(), buffer.begin());
                    offset = getScopeTriggerOffset(buffer);
                    ImPlot::PlotLine("Noise", mTimeArray.data(), buffer.data() + offset, (int)mTimeArray.size());
                }
                ImPlot::EndPlot();
            }
                    
            if (ImPlot::BeginPlot("DMC", { -1, -1 }))
            {
                ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_Lock);
                ImPlot::SetupAxesLimits(mTimeArray.front(), mTimeArray.back(), -0.1f, 1);
                if (mDmcFIFOPtr != nullptr)
                {
                    std::copy(mDmcFIFOPtr->begin(), mDmcFIFOPtr->end(), buffer.begin());
                    offset = getScopeTriggerOffset(buffer);
                    ImPlot::PlotLine("DMC", mTimeArray.data(), buffer.data() + offset, (int)mTimeArray.size());
                }
                ImPlot::EndPlot();
            }

            ImPlot::EndSubplots();
        }
    }
    ImGui::End();    
}

void GlfwApp::drawSpectrumWindow()
{
    if (ImGui::Begin("Spectrum"))
    {
        if (mSoundFIFOPtr != nullptr)
        {
            // Compute FFT
            soundBufferF32_t signal;
            std::copy(mSoundFIFOPtr->begin(), mSoundFIFOPtr->end(), signal.begin());
            
            soundBufferF32_t spectrum;
            fftMagnitude<BUFFER_SIZE>(signal, spectrum);

            ImGui::TextUnformatted("Scale: ");
            ImGui::SameLine();
            if (ImGui::RadioButton("Normal", !mIsLogScale))
                mIsLogScale = false;
            ImGui::SameLine();
            if (ImGui::RadioButton("Semi-log", mIsLogScale))
                mIsLogScale = true;

            if (ImPlot::BeginPlot("Sound Fourier transform", { -1, -1 }))
            {
                ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_Lock);
                if (mIsLogScale)
                    ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
                ImPlot::SetupAxesLimits((float)BUFFER_SAMPLE_RATE / BUFFER_SIZE, BUFFER_SAMPLE_RATE / 2, 0, 0.1f);
                ImPlot::SetNextMarkerStyle(ImPlotMarker_Up);
                ImPlot::PlotStems("FFT", mFrequencies.data(), spectrum.data(), (int)spectrum.size() / 2);
                ImPlot::EndPlot();
            }
        }
        // TODO: Plot the spectrum of the APU output
        // TODO: Enable to see the individual channels spectrum ?
    }
    ImGui::End();    
}

void GlfwApp::drawAudioSettingsWindow()
{
    if (ImGui::Begin("Audio settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        // Master volume
        ImGui::SliderFloat("Master volume", &mMasterVolume, 0.0f, 6.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
        ImGui::SameLine(0, 0);
        ImGui::TextDisabled("(?)");
        if (ImGui::BeginItemTooltip())
        {
            ImGui::TextUnformatted("Ctrl+click on the slider to write the value.");
            ImGui::EndTooltip();
        }

        if (ImGui::Button("Close"))
            mIsAudioSettingsWindowOpen = false;
    }
    ImGui::End();
}

void GlfwApp::drawVideoSettingsWindow()
{
    if (ImGui::Begin("Video settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        // Filtering
        if (ImGui::BeginCombo("Filtering", mCurrentFiltering))
        {
            for (uint8_t i = 0; i < IM_ARRAYSIZE(FILTERING_ITEMS); i++)
            {
                bool isSelected = (mCurrentFiltering == FILTERING_ITEMS[i]);
                if (ImGui::Selectable(FILTERING_ITEMS[i], isSelected))
                {
                    // Clicked on an item -> change filtering
                    mCurrentFiltering = FILTERING_ITEMS[i];
                    updateFiltering(i);
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            
            ImGui::EndCombo();
        }

        // Shader
        bool isUpdateOk = true;
        if (ImGui::BeginCombo("Shader", mCurrentShaderStr))
        {
            for (uint8_t i = 0; i < IM_ARRAYSIZE(SHADER_ITEMS); i++)
            {
                bool isSelected = (mCurrentShaderStr == SHADER_ITEMS[i]);
                if (ImGui::Selectable(SHADER_ITEMS[i], isSelected))
                {
                    // Clicked on an item -> change filtering
                    mCurrentShaderStr = SHADER_ITEMS[i];
                    if(!updateShader(i))
                        isUpdateOk = false;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            
            ImGui::EndCombo();
        }

        // Recompile
        if (mCurrentShaderStr == SHADER_ITEMS[1])
        {
            if (ImGui::Button("Recompile"))
                if(!updateShader(1))
                    isUpdateOk = false;
        }

        // Error pop-up
        if (!isUpdateOk)
            ImGui::OpenPopup("SHADER ERROR");
        if (ImGui::BeginPopupModal("SHADER ERROR", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::TextUnformatted(mShaderErrorMessage.c_str());

            if (ImGui::Button("Close"))
                ImGui::CloseCurrentPopup();

            ImGui::EndPopup();
        }

        if (ImGui::Button("Close"))
            mIsVideoSettingsWindowOpen = false;
    }
    ImGui::End();
}

void GlfwApp::drawInputSettingsWindow()
{
    if (ImGui::Begin("Input settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (ImGui::BeginTabBar("Devices"))
        {
            if (ImGui::BeginTabItem("Keyboard"))
            {
                ImGui::Checkbox("Enabled", &mIsKeyboardEnabled);
                ImGui::TextUnformatted("Player: ");
                ImGui::SameLine();
                if (ImGui::RadioButton("1", mIsKeyboardPlayer1Selected))
                    mIsKeyboardPlayer1Selected = true;
                ImGui::SameLine();
                if (ImGui::RadioButton("2", !mIsKeyboardPlayer1Selected))
                    mIsKeyboardPlayer1Selected = false;

                ImGui::SeparatorText("Key mapping:");
                drawKeyMapping("A: ",      &mKeyboardMapping.a);
                drawKeyMapping("B: ",      &mKeyboardMapping.b);
                drawKeyMapping("Start: ",  &mKeyboardMapping.start);
                drawKeyMapping("Select: ", &mKeyboardMapping.select);
                drawKeyMapping("Up: ",     &mKeyboardMapping.up);
                drawKeyMapping("Down: ",   &mKeyboardMapping.down);
                drawKeyMapping("Left: ",   &mKeyboardMapping.left);
                drawKeyMapping("Right: ",  &mKeyboardMapping.right);

                ImGui::EndTabItem();
            }
            
            for (int i = 0; i < 16; i++)
            {
                if (glfwJoystickIsGamepad(i))
                {
                    // Get controller info
                    const char* gamepadName = glfwGetGamepadName(i);
                    GLFWgamepadstate state;
                    glfwGetGamepadState(i, &state);

                    // Render tab
                    if (ImGui::BeginTabItem(gamepadName))
                    {
                        ImGui::Checkbox("Enabled", &mAreGamepadsEnabled[i]);
                        ImGui::TextUnformatted("Player: ");
                        ImGui::SameLine();
                        if (ImGui::RadioButton("1", mAreGamepadsPlayer1Selected[i]))
                            mAreGamepadsPlayer1Selected[i] = true;
                        ImGui::SameLine();
                        if (ImGui::RadioButton("2", !mAreGamepadsPlayer1Selected[i]))
                            mAreGamepadsPlayer1Selected[i] = false;

                        ImGui::TextUnformatted("Layout: ");
                        ImGui::SameLine();
                        if (ImGui::RadioButton("B: XY, A: AB", !mAreGamepadsLayoutAlternative[i]))
                            mAreGamepadsLayoutAlternative[i] = false;
                        ImGui::SameLine();
                        if (ImGui::RadioButton("B: AX, A: BY", mAreGamepadsLayoutAlternative[i]))
                            mAreGamepadsLayoutAlternative[i] = true;

                        ImGui::SeparatorText("Controller state");
                        ImGui::SliderFloat("Dead zone", &mGamepadsDeadZone[i], 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                        if (ImPlot::BeginPlot("L-Joystick", { -1, 0 }, ImPlotFlags_Equal))
                        {
                            float x = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
                            float y = -state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
                            float deadZone = mGamepadsDeadZone[i];
                            float deadZoneSquareX[] = { deadZone, -deadZone, -deadZone, deadZone, deadZone };
                            float deadZoneSquarey[] = { deadZone, deadZone, -deadZone, -deadZone, deadZone };
                            ImPlot::SetupAxesLimits(-1.0f, 1.0f, -1.0f, 1.0f);
                            ImPlot::PlotScatter("Position", &x, &y, 1);
                            ImPlot::PlotLine("Dead zone", deadZoneSquareX, deadZoneSquarey, 5);
                            
                            ImPlot::EndPlot();
                        }
                        ImGui::TextUnformatted("Buttons pressed: ");
                        if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS)
                        {
                            ImGui::SameLine();
                            ImGui::TextUnformatted("A");
                        }
                        if (state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS)
                        {
                            ImGui::SameLine();
                            ImGui::TextUnformatted("B");
                        }
                        if (state.buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS)
                        {
                            ImGui::SameLine();
                            ImGui::TextUnformatted("X");
                        }
                        if (state.buttons[GLFW_GAMEPAD_BUTTON_Y] == GLFW_PRESS)
                        {
                            ImGui::SameLine();
                            ImGui::TextUnformatted("Y");
                        }
                        if (state.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS)
                        {
                            ImGui::SameLine();
                            ImGui::TextUnformatted("START");
                        }
                        if (state.buttons[GLFW_GAMEPAD_BUTTON_BACK] == GLFW_PRESS)
                        {
                            ImGui::SameLine();
                            ImGui::TextUnformatted("BACK");
                        }
                
                        ImGui::EndTabItem();
                    }                    
                }
            }

            ImGui::EndTabBar();
        }

        if (ImGui::Button("Close"))
            mIsInputSettingsWindowOpen = false;
    }
    ImGui::End();
}

void GlfwApp::drawKeyMapping(const char* buttonStr, keycode_t* key)
{
    constexpr const char* UNKNOWN_KEY = "unknown";
    const char* keyString = nullptr;

    ImGui::TextUnformatted(buttonStr);
    ImGui::SameLine(0, 0);

    int scancode = glfwGetKeyScancode(*key);
    if ((keyString = glfwGetKeyName(*key, scancode)) == nullptr)
        keyString = UNKNOWN_KEY;
        
    if (ImGui::Selectable(keyString, mKeyToChange == key))
        mKeyToChange = key;
}

void GlfwApp::drawHeaderInfoWindow()
{
    if (ImGui::Begin("ROM Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted(mPathToRom.c_str());
        ImGui::TextUnformatted(mHeaderInfo.c_str());
        if (ImGui::Button("Close"))
            mIsHeaderInfoWindowOpen = false;
    }
    ImGui::End();
}

void GlfwApp::drawAboutWindow()
{
    if (ImGui::Begin("About - NESFT", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("NESFT\n" 
                               "Version: 1.0\n"
                               "Build date: " __DATE__ ", " __TIME__ "\n");
        ImGui::TextUnformatted("More info at: ");
        ImGui::SameLine(0, 0);
        ImGui::TextLinkOpenURL("github.com/VoltXd/NESFT", "https://github.com/VoltXd/NESFT");
        if (ImGui::Button("Close"))
            mIsAboutWindowOpen = false;
    }
    ImGui::End();
}

bool GlfwApp::drawErrorWindow()
{
    bool isClosed = false;

    if (ImGui::Begin("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted(mErrorMessage.c_str());
        isClosed = ImGui::Button("Close");
    }
    ImGui::End();
    
    return isClosed;
}

void GlfwApp::updateFiltering(uint8_t filteringIdx)
{
    constexpr uint8_t NEAREST_IDX = 0;
    constexpr uint8_t LINEAR_IDX = 1;

    GLint filtering;
    switch (filteringIdx)
    {
        case NEAREST_IDX:
            filtering = GL_NEAREST;
            break;

        case LINEAR_IDX:
            filtering = GL_LINEAR;
            break;
        
        default:
            return;
    }

    // Set filtering settings
    glBindTexture(GL_TEXTURE_2D, mScreenTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
}

bool GlfwApp::updateShader(uint8_t shaderIdx)
{
    constexpr uint8_t DEFAULT_IDX = 0;
    constexpr uint8_t CUSTOM_IDX = 1;
    constexpr uint8_t NEGATIVE_IDX = 2;
    constexpr uint8_t GRAYSCALE_IDX = 3;
    constexpr uint8_t SHARPEN_IDX = 4;
    constexpr uint8_t EDGES_IDX = 5;

    bool isUpdateOk = true;
    switch (shaderIdx)
    {
        case DEFAULT_IDX:
            mScreenShader.reset(new Shader(VERT_SHADER_DEFAULT, FRAG_SHADER_DEFAULT));
            break;

        case CUSTOM_IDX:
            mScreenShader.reset(new Shader("shaders/custom.vert", "shaders/custom.frag", nullptr));
            isUpdateOk = mScreenShader->isCompiled();

            if (!isUpdateOk)
            {
                // Get error message
                mShaderErrorMessage = mScreenShader->getErrorMessage();

                // Reset to default shader
                mCurrentShaderStr = SHADER_ITEMS[0];
                mScreenShader.reset(new Shader(VERT_SHADER_DEFAULT, FRAG_SHADER_DEFAULT));
            }
            break;

        case NEGATIVE_IDX:
            mScreenShader.reset(new Shader(VERT_SHADER_DEFAULT, FRAG_SHADER_NEGATIVE));
            break;

        case GRAYSCALE_IDX:
            mScreenShader.reset(new Shader(VERT_SHADER_DEFAULT, FRAG_SHADER_GRAYSCALE));
            break;

        case SHARPEN_IDX:
            mScreenShader.reset(new Shader(VERT_SHADER_DEFAULT, FRAG_SHADER_SHARPEN));
            break;

        case EDGES_IDX:
            mScreenShader.reset(new Shader(VERT_SHADER_DEFAULT, FRAG_SHADER_EDGES));
            break;
        
        default:
            break;
    }

    return isUpdateOk;
}

void GlfwApp::pollKeyboard()
{
    // Controller
    if (!mIsKeyboardEnabled)
        return;

    prepareControllersState(mIsKeyboardPlayer1Selected, ControllerInput::A,      glfwGetKey(mWindow, mKeyboardMapping.a)      == GLFW_PRESS);
    prepareControllersState(mIsKeyboardPlayer1Selected, ControllerInput::B,      glfwGetKey(mWindow, mKeyboardMapping.b)      == GLFW_PRESS);
    prepareControllersState(mIsKeyboardPlayer1Selected, ControllerInput::SELECT, glfwGetKey(mWindow, mKeyboardMapping.select) == GLFW_PRESS);
    prepareControllersState(mIsKeyboardPlayer1Selected, ControllerInput::START,  glfwGetKey(mWindow, mKeyboardMapping.start)  == GLFW_PRESS);
    prepareControllersState(mIsKeyboardPlayer1Selected, ControllerInput::UP,     glfwGetKey(mWindow, mKeyboardMapping.up)     == GLFW_PRESS);
    prepareControllersState(mIsKeyboardPlayer1Selected, ControllerInput::DOWN,   glfwGetKey(mWindow, mKeyboardMapping.down)   == GLFW_PRESS);
    prepareControllersState(mIsKeyboardPlayer1Selected, ControllerInput::LEFT,   glfwGetKey(mWindow, mKeyboardMapping.left)   == GLFW_PRESS);
    prepareControllersState(mIsKeyboardPlayer1Selected, ControllerInput::RIGHT,  glfwGetKey(mWindow, mKeyboardMapping.right)  == GLFW_PRESS);
}

void GlfwApp::pollGamepads()
{
    for (int i = 0; i < 16; i++)
    {
        if (glfwJoystickIsGamepad(i))
        {
            // Skip disabled controller
            if (!mAreGamepadsEnabled[i])
                continue;

            // Get controller info
            GLFWgamepadstate state;
            glfwGetGamepadState(i, &state);
            bool isPlayer1 = mAreGamepadsPlayer1Selected[i];
            uint8_t secondAButton = mAreGamepadsLayoutAlternative[i] ?
                                    GLFW_GAMEPAD_BUTTON_Y :
                                    GLFW_GAMEPAD_BUTTON_A;
            uint8_t secondBButton = mAreGamepadsLayoutAlternative[i] ?
                                    GLFW_GAMEPAD_BUTTON_A :
                                    GLFW_GAMEPAD_BUTTON_Y;
                                    

            // Update controller
            prepareControllersState(isPlayer1, 
                                   ControllerInput::A, 
                                   (state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS) ||
                                   (state.buttons[secondAButton] == GLFW_PRESS));
            prepareControllersState(isPlayer1, 
                                   ControllerInput::B, 
                                   (state.buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS) ||
                                   (state.buttons[secondBButton] == GLFW_PRESS));
            prepareControllersState(isPlayer1, 
                                   ControllerInput::START, 
                                   state.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS);
            prepareControllersState(isPlayer1, 
                                   ControllerInput::SELECT, 
                                   state.buttons[GLFW_GAMEPAD_BUTTON_BACK] == GLFW_PRESS);
            prepareControllersState(isPlayer1, 
                                   ControllerInput::UP, 
                                   (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS) ||
                                   (state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] < -mGamepadsDeadZone[i]));
            prepareControllersState(isPlayer1, 
                                   ControllerInput::DOWN, 
                                   (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS) ||
                                   (state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] > mGamepadsDeadZone[i]));
            prepareControllersState(isPlayer1, 
                                   ControllerInput::LEFT, 
                                   (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS) ||
                                   (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] < -mGamepadsDeadZone[i]));
            prepareControllersState(isPlayer1, 
                                   ControllerInput::RIGHT, 
                                   (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS) ||
                                   (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] > mGamepadsDeadZone[i]));
        }
    }
}

void GlfwApp::updateControllersState()
{
    mController1Ref.updateControllerState(mController1State);
    mController2Ref.updateControllerState(mController2State);
}

void GlfwApp::openFile()
{
    // Try opening a ROM file 
    nfdchar_t* pathToRom = nullptr;
    nfdresult_t dialogResult = NFD_OpenDialog(nullptr, nullptr, &pathToRom);
    if (dialogResult == NFD_OKAY)
    {
        mIsRomOpened = true;
        mPathToRom = std::string(pathToRom);
    }
}

void GlfwApp::resetRom()
{
    if (mPathToRom.size() > 0)
        mIsRomOpened = true;
}

constexpr timeArray_t GlfwApp::calculateTimeArray()
{
    timeArray_t timeArray = {{ 0 }};
    for (u32 i = 0; i < BUFFER_SIZE / 2; i++)
        timeArray[i] = BUFFER_SAMPLE_PERIOD * ((s32)i - (s32)(BUFFER_SIZE / 4));

    return timeArray;
}

constexpr soundBufferF32_t GlfwApp::calculateFrequencyArray()
{
    soundBufferF32_t frequencyArray = {{ 0 }};
    for (u32 i = 0; i < BUFFER_SIZE; i++)
        frequencyArray[i] = ((float)BUFFER_SAMPLE_RATE / BUFFER_SIZE) * i;

    return frequencyArray;
}

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    GlfwApp* renderer = reinterpret_cast<GlfwApp*>(glfwGetWindowUserPointer(window));
    if (renderer)
    {
        (void)scancode; // Useless line to remove warning

        // Open file
        if ((action == GLFW_PRESS) && (key == GLFW_KEY_O) && ((mods & GLFW_MOD_CONTROL) != 0))
        {
            renderer->openFile();
            return;
        }

        // Reset
        if((action == GLFW_PRESS) && (key == GLFW_KEY_R) && ((mods & GLFW_MOD_CONTROL) != 0))
        {
            renderer->resetRom();
            return;
        }

        // Change key mapping
        keycode_t* keyToChange = renderer->getKeyToChange();
        if (keyToChange != nullptr)
        {
            // Change key mapping if not escape
            if (key != GLFW_KEY_ESCAPE)
                *keyToChange = (keycode_t)key;

            renderer->clearKeyToChange();
            return;
        }

        // Window
        if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE))
        {
            renderer->switchPauseState();
            return;
        }
    }
}

void resizeCallback(GLFWwindow *window, int windowWidth, int windowHeight)
{
    (void)window; // Remove warning
    if ((float)windowWidth / windowHeight > NES_ASPECT_RATIO)
    {
        int windowWidth4_3 = (int)(windowHeight * NES_ASPECT_RATIO);
        int windowXOffset = (windowWidth - windowWidth4_3) / 2; 
        glViewport(windowXOffset, 0, windowWidth4_3, windowHeight);
    }
    else
    {
        int windowHeight4_3 = (int)(windowWidth * (1.0 / NES_ASPECT_RATIO));
        int windowYOffset = (windowHeight - windowHeight4_3) / 2; 
        glViewport(0, windowYOffset, windowWidth, windowHeight4_3);
    }
}
