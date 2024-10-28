#pragma once

#include <cstdint>
#include <array>

#include "NES/Config.hpp"

constexpr uint32_t PPU_OUTPUT_WIDTH = 256;
constexpr uint32_t PPU_OUTPUT_HEIGHT = 240;
constexpr uint32_t PPU_OUTPUT_CHANNELS = 3;

using picture_t = std::array<std::array<std::array<byte, PPU_OUTPUT_CHANNELS>, PPU_OUTPUT_WIDTH>, PPU_OUTPUT_HEIGHT>;