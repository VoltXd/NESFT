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

GlfwApp::GlfwApp(Controller& controllerRef)
    : mControllerRef(controllerRef)
{
    // Audio
    mMasterVolume = 1.0f;

    // Video
    mCurrentFiltering = FILTERING_ITEMS[1];

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, PPU_OUTPUT_WIDTH, PPU_OUTPUT_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);

    // OpenGL settings
    glEnable(GL_CULL_FACE);

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
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
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

    glDeleteVertexArrays(1, &mScreenVao);
    glDeleteBuffers(1, &mScreenVbo);
    glDeleteBuffers(1, &mScreenEbo);
    glDeleteTextures(1, &mScreenTexture);
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void GlfwApp::draw(const picture_t& pictureBuffer)
{
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

    // Draw main menu bar
    drawMainMenuBar();

    // Bind screen vao & shader
    mScreenShader->use();
    glBindVertexArray(mScreenVao);  

    // Update screen texture
    // TODO: implement with TexSubImage2D (try FBO & PBO) ?
    // SubImage
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mScreenTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, PPU_OUTPUT_WIDTH, PPU_OUTPUT_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pictureBuffer.data()->data()->data());

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

void GlfwApp::updateControllerState(ControllerInput input, bool isPressed)
{
    mControllerRef.updateControllerState(input, isPressed);
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

            if (ImPlot::BeginPlot("Sound Fourier transform", { -1, -1 }))
            {
                ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_Lock);
                // ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
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

        if (ImGui::Button("Close"))
            mIsVideoSettingsWindowOpen = false;
    }
    ImGui::End();
}

void GlfwApp::drawInputSettingsWindow()
{
    if (ImGui::Begin("Input settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        // TODO
        ImGui::TextUnformatted("WIP...");

        if (ImGui::Button("Close"))
            mIsInputSettingsWindowOpen = false;
    }
    ImGui::End();
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

        // Handle keyboard press
        bool isPressed;
        ControllerInput input;

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

        // Window
        if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE))
        {
            renderer->switchPauseState();
            return;
        }

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
