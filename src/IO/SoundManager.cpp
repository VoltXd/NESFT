#include "IO/SoundManager.hpp"

#include <alc.h>
#include <iostream>

int SoundManager::initialise()
{
	// Reset
	alcCloseDevice(alcOpenDevice(nullptr));
	
	// Open sound device
    ALCdevice* devicePtr = alcOpenDevice(nullptr);
	if (devicePtr == nullptr)
	{
		std::cout << "Could not open default sound device." << std::endl;
		return EXIT_FAILURE;
	}

	// Create OpenAL context (default device, no properties)
	ALCcontext* contextPtr = alcCreateContext(devicePtr, nullptr);
	alcMakeContextCurrent(contextPtr);

	// Clear error
	ALenum error;
	error = alGetError();

	// Create sound buffers
	alGenBuffers(NUM_BUFFERS, mBuffers.data());
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		std::cout << "Error: Could not generate sound buffer (code: 0x" 
		          << std::hex
				  << error
				  << ')' << std::endl;
		return EXIT_FAILURE;
	}

	// Create sound source
	alGenSources(1, &mSource);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		std::cout << "Error: Could not generate sound source (code: 0x" 
		          << std::hex
				  << error
				  << ')' << std::endl;
		return EXIT_FAILURE;
	}
	alSourcef(mSource, AL_PITCH, 1.0f);
	alSourcef(mSource, AL_GAIN, 1.0f);


	return EXIT_SUCCESS;
}

SoundManager::~SoundManager()
{
	// Unbind buffers
	alSourcei(mSource, AL_BUFFER, 0);

	// Delete source & buffers
	alDeleteSources(1, &mSource);
	alDeleteBuffers(NUM_BUFFERS, mBuffers.data());

	// I think it is not OK not to check for nullptrs
	// I guess I'll find out if it's an issue while testing the code -\_('U')_/-
	ALCcontext* contextPtr = alcGetCurrentContext();
	ALCdevice* devicePtr = alcGetContextsDevice(contextPtr);
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(contextPtr);
	alcCloseDevice(devicePtr);
}

StreamStatus SoundManager::streamSound(const soundBuffer_t& bufferData)
{
	ALint numProcessed, numQueued;
	StreamStatus streamStatus;

	// Get number of QUEUED, PROCESSED & TOTAL buffer
	alGetSourcei(mSource, AL_BUFFERS_QUEUED, &numQueued);
	alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &numProcessed);
	ALint totalInSource = numProcessed + numQueued;

	if (totalInSource < NUM_BUFFERS)
	{
		// Start of the stream
		// Fill the source with buffers
		alBufferData(mBuffers[totalInSource], BUFFER_FORMAT, bufferData.data(), BUFFER_SIZE, BUFFER_SAMPLE_RATE);
		alSourceQueueBuffers(mSource, 1, &mBuffers[totalInSource]);

		streamStatus = StreamStatus::QUEUED;
	}
	else if (numProcessed == 0)
	{
		// Not finished playing buffers
		streamStatus = StreamStatus::NOT_QUEUED;
	}
	else
	{
		// Finished playing some buffers
		// Unqueue a buffer, modify it's data and requeue it
		ALuint buffer;
		alSourceUnqueueBuffers(mSource, 1, &buffer);
		alBufferData(buffer, BUFFER_FORMAT, bufferData.data(), BUFFER_SIZE, BUFFER_SAMPLE_RATE);
		alSourceQueueBuffers(mSource, 1, &buffer);

		streamStatus = StreamStatus::QUEUED;
	}

	// Play source
	ALint sourceState;
	alGetSourcei(mSource, AL_SOURCE_STATE, &sourceState);
	if (sourceState != AL_PLAYING)
		alSourcePlay(mSource);

	// Just in case (poorly placed)
	checkError();
	
	return streamStatus;
}

void SoundManager::checkError()
{
	ALenum error;
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		std::cout << std::hex << "OpenAL ERROR: 0x" << error << std::endl;
		exit(EXIT_FAILURE);
	}
}