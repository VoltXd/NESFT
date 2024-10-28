#pragma once

#ifdef TEST_6502
#include "NES/Memory6502.hpp"
using Memory = Memory6502;

#else
#include "NES/MemoryNES.hpp"
using Memory = MemoryNES;

#endif