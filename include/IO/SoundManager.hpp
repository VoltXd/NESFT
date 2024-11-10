#pragma once

#include <al.h>
#include <array>

#include "NES/Config.hpp"

constexpr u32 BUFFER_SIZE = 2048;

using soundBuffer_t = std::array<u8, BUFFER_SIZE>;

enum class StreamStatus
{
	QUEUED,
	NOT_QUEUED
};

class SoundManager
{
public:
	int initialise();
	~SoundManager();

	StreamStatus streamSound(const soundBuffer_t& bufferData);

private:
	void checkError();
	
	// Buffers
	static constexpr u8      NUM_BUFFERS   = 4;
	static constexpr ALenum  BUFFER_FORMAT = AL_FORMAT_MONO8;
	static constexpr ALsizei SAMPLE_RATE   = 44'100;
	std::array<ALuint, NUM_BUFFERS> mBuffers;

	ALuint mSource;
};
