#include "App.hpp"

#include <cstdlib>
#include <iostream>
#include <chrono>
#include "NES/NES.hpp"
#include "NES/Cartridge.hpp"
#include "NES/Toolbox.hpp"
#include "IO/GlfwApp.hpp"

#include "IO/SoundManager.hpp"
#include <cmath>

App::App()
{
}

int App::run()
{
	GlfwApp appWindow(mController);
	testAndExitWithMessage(mSoundManager.initialise() == EXIT_FAILURE, "Cannot initialise sound manager...");
	mAppState = AppState::IDLE;

	

	// Infinite loop
	while (!appWindow.shouldWindowClose())
	{
		switch (mAppState)
		{
		case AppState::IDLE:
			processIdleState(appWindow);
			break;

		case AppState::PLAYING:
			playGame(appWindow);
			break;
		
		default:
			break;
		}
	}

	return EXIT_SUCCESS;
}

void App::processIdleState(GlfwApp& appWindow)
{
	constexpr picture_t BLANK_SCREEN = { 0 };

	while (!appWindow.shouldWindowClose() && !appWindow.isRomOpened())
	{
		appWindow.draw(BLANK_SCREEN);
	}
	
	mAppState = AppState::PLAYING;
}

void App::playGame(GlfwApp& appWindow)
{
	// *************** NES Emulation *************** //
	std::chrono::steady_clock::time_point time0 = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point time1;

	double elapsedTime, elapsedTimeOffset = 0;
	NES nes(mController, appWindow.getRomName());
	appWindow.clearIsRomOpened();

	while (!appWindow.shouldWindowClose())
	{
		// Change ROM if new one opened 
		if (appWindow.isRomOpened())
			break;

		// Update
		if (appWindow.isPaused())
		{
			appWindow.draw(nes.getPicture());
			time0 = std::chrono::steady_clock::now();
			continue;
		}

		nes.runOneCpuInstruction();
		if (nes.isImageReady())
		{
			nes.clearIsImageReady();

			// Wait before rendering -> 60 FPS
			do
			{
				time1 = std::chrono::steady_clock::now();
				elapsedTime = std::chrono::duration<double>(time1 - time0).count();
			} while (elapsedTime + elapsedTimeOffset < FRAME_PERIOD_NTSC);
			time0 = time1;
			elapsedTimeOffset = elapsedTime + elapsedTimeOffset - FRAME_PERIOD_NTSC;

			if (appWindow.isSoundChannelsWindowOpen())
				prepareBuffersAndSendToWindow(nes, appWindow);
						
			// Render
			appWindow.draw(nes.getPicture());
		}

		if (nes.isSoundBufferReady())
		{
			nes.clearIsSoundBufferReady();

			mSoundManager.streamSound(*nes.getSoundBufferPtr());
		}
	}
}

void App::prepareBuffersAndSendToWindow(NES &nes, GlfwApp &window)
{
	nes.prepareDrawnSoundBuffers();

	window.setSoundBufferPtr(nes.getBufferToDrawPtr());
	window.setP1BufferPtr(nes.getP1BufferToDrawPtr());
	window.setP2BufferPtr(nes.getP2BufferToDrawPtr());
	window.setTriangleBufferPtr(nes.getTriangleBufferToDrawPtr());
	window.setNoiseBufferPtr(nes.getNoiseBufferToDrawPtr());
	window.setDmcBufferPtr(nes.getDmcBufferToDrawPtr());
}
