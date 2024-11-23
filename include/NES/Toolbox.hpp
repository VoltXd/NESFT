#pragma once

#include <string>
#include <array>
#include "NES/Config.hpp"

void testAndExitWithMessage(bool condition, const std::string& message);

template <typename T, size_t S>
s32 getScopeTriggerOffset(const std::array<T, S>& array)
{
    T minValue = *std::min_element(array.begin(), array.end());
    T maxValue = *std::max_element(array.begin(), array.end());
    T average = 0.5f * (minValue + maxValue);

    s32 sampleMiddleOffset = 0;
    for (int i = (BUFFER_SIZE / 4); i < (3 * (BUFFER_SIZE / 4)); i++)
    {
        T currentValue = array[i];
        T nextValue = array[i + 1];
        if (currentValue <= average && average < nextValue)
        {
            sampleMiddleOffset = i;
            break;
        }
    }

    return sampleMiddleOffset > 0 ? sampleMiddleOffset - (BUFFER_SIZE / 4) : 0;
}