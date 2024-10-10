#pragma once

#ifdef TEST_6502
#include "Memory6502.hpp"
using Memory = Memory6502;

#else
#include "MemoryNES.hpp"
using Memory = MemoryNES;

#endif