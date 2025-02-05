#include "IO/SoundManager.hpp"

#include <al.h>
#include <alc.h>
#include <iostream>

#ifdef _MSC_VER
#define ASSERT(x) if (!(x)) __debugbreak()
#else
#define ASSERT(x) if (!(x)) exit(EXIT_FAILURE);
#endif

#ifdef AL_DEBUG
#define AL_CALL(x) alClearError();\
                   x;\
				   ASSERT(alCheckError(__FUNCTION__, __FILE__, __LINE__))
#else
#define AL_CALL(x) x
#endif

static void alClearError()
{
	while (alGetError() != AL_NO_ERROR);
}

static bool alCheckError(const char* function, const char* file, int line)
{
	bool isNotError = true;
	while (ALenum error = alGetError() != AL_NO_ERROR)
	{
		std::cout << std::hex << "OpenAL ERROR (0x" << error << "): " << function << ' ' << file << ':' << line << std::endl;
		isNotError = false;
	}

	return isNotError;
}

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
	alClearError();

	// Create sound buffers
	alGenBuffers(NUM_BUFFERS, mBuffers.data());
	alCheckError(__FUNCTION__, __FILE__, __LINE__);

	// Create sound source
	alGenSources(1, &mSource);
	alCheckError(__FUNCTION__, __FILE__, __LINE__);

	AL_CALL(alSourcef(mSource, AL_PITCH, 1.0f));
	AL_CALL(alSourcef(mSource, AL_GAIN, 1.0f));

	return EXIT_SUCCESS;
}

SoundManager::~SoundManager()
{
	// Unbind buffers
	AL_CALL(alSourcei(mSource, AL_BUFFER, 0));

	// Delete source & buffers
	AL_CALL(alDeleteSources(1, &mSource));
	AL_CALL(alDeleteBuffers(NUM_BUFFERS, mBuffers.data()));

	// I think it is not OK not to check for nullptrs
	// I guess I'll find out if it's an issue while testing the code -\_('U')_/-
	ALCcontext* contextPtr = alcGetCurrentContext();
	ALCdevice* devicePtr = alcGetContextsDevice(contextPtr);
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(contextPtr);
	alcCloseDevice(devicePtr);
}

StreamStatus SoundManager::streamSound(const soundBufferF32_t& soundBuffer)
{
	ALint numProcessed, numQueued;
	StreamStatus streamStatus;

	// Get number of QUEUED, PROCESSED & TOTAL buffer
	AL_CALL(alGetSourcei(mSource, AL_BUFFERS_QUEUED, &numQueued));
	AL_CALL(alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &numProcessed));

	if (numQueued < NUM_BUFFERS)
	{
		// Start of the stream
		// Transform data
		soundBuffer_t byteBuffer;
		convertBuffer(soundBuffer, byteBuffer);
		
		// Fill the source with buffers
		AL_CALL(alBufferData(mBuffers[numQueued], BUFFER_FORMAT, byteBuffer.data(), BUFFER_SIZE, BUFFER_SAMPLE_RATE));
		AL_CALL(alSourceQueueBuffers(mSource, 1, &mBuffers[numQueued]));

		streamStatus = StreamStatus::QUEUED;
	}
	else if (numProcessed <= 0)
	{
		// Not finished playing buffers
		streamStatus = StreamStatus::NOT_QUEUED;
	}
	else
	{
		// Finished playing some buffers
		// Transform data
		soundBuffer_t byteBuffer;
		convertBuffer(soundBuffer, byteBuffer);

		// Unqueue a buffer, modify it's data and requeue it
		ALuint buffer;
		AL_CALL(alSourceUnqueueBuffers(mSource, 1, &buffer));
		AL_CALL(alBufferData(buffer, BUFFER_FORMAT, byteBuffer.data(), BUFFER_SIZE, BUFFER_SAMPLE_RATE));
		AL_CALL(alSourceQueueBuffers(mSource, 1, &buffer));

		streamStatus = StreamStatus::QUEUED;
	}

	// Play source
	ALint sourceState;
	AL_CALL(alGetSourcei(mSource, AL_SOURCE_STATE, &sourceState));
	if (sourceState != AL_PLAYING)
		AL_CALL(alSourcePlay(mSource));
	
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

void SoundManager::convertBuffer(const soundBufferF32_t &buffer, soundBuffer_t& byteBuffer)
{
	for (u32 i = 0; i < BUFFER_SIZE; i++)
		byteBuffer[i] = (u8)(0.5f * 255.0f * (buffer[i] + 1));
}