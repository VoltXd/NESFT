#pragma once

#include <al.h>
#include <array>
#include <deque>

#include "NES/Config.hpp"

constexpr u32     BUFFER_SIZE   = 1 << 11;
constexpr ALenum  BUFFER_FORMAT = AL_FORMAT_MONO8;
constexpr ALsizei BUFFER_SAMPLE_RATE   = 44'100;
constexpr float BUFFER_SAMPLE_PERIOD   = 1.0f / BUFFER_SAMPLE_RATE; 

using soundFIFO_t = std::deque<float>;
using soundBuffer_t = std::array<u8, BUFFER_SIZE>;
using soundBufferF32_t = std::array<float, BUFFER_SIZE>;

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

	StreamStatus streamSound(const soundBufferF32_t& soundBuffer);

private:
	void checkError();
	void convertBuffer(const soundBufferF32_t &buffer, soundBuffer_t& byteBuffer);
	
	// Buffers
	static constexpr u8 NUM_BUFFERS = 3;
	std::array<ALuint, NUM_BUFFERS> mBuffers;

	ALuint mSource;
};
