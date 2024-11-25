#pragma once

#include <string>

#include "NES/NES.hpp"
#include "NES/Controller.hpp"
#include "IO/GlfwApp.hpp"
#include "IO/SoundManager.hpp"

class App
{
public:
	App();

	int run();

private:
	enum class AppState
	{
		IDLE,
		PLAYING
	} mAppState;
	
	void processIdleState(GlfwApp& appWindow);
	void playGame(GlfwApp& appWindow);
	void prepareBuffersAndSendToWindow(NES& nes, GlfwApp& window);

	Controller mController;
	SoundManager mSoundManager;
};
