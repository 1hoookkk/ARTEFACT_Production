#pragma once
#include "tdd-guard.h"

// This include will fail because the file does not exist yet. This is our goal.
#include "DSP/SC_Mapping.h"

TEST(MappingLogic, YtoFrequency)
{
    const int imageHeight = 1024;
    const float minFreq = 20.0f;
    const float maxFreq = 20000.0f;

    // This code will cause a compile error because the function doesn't exist.
    float freqAtTop = SC_Mapping::mapYToFrequency(0, imageHeight, minFreq, maxFreq);
    ASSERT_EQ(freqAtTop, maxFreq);
}