#pragma once

#include <al.h>
#include <array>

#include "NES/Config.hpp"

constexpr u32     BUFFER_SIZE   = 1 << 10;
constexpr ALenum  BUFFER_FORMAT = AL_FORMAT_MONO8;
constexpr ALsizei BUFFER_SAMPLE_RATE   = 44'100;
constexpr float BUFFER_SAMPLE_PERIOD   = 1.0f / BUFFER_SAMPLE_RATE; 

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
	static constexpr u8 NUM_BUFFERS = 16;
	std::array<ALuint, NUM_BUFFERS> mBuffers;

	ALuint mSource;
};
