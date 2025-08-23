// Tests/Support/AudioTrace.h
// Test-only AudioTrace shim that forwards to SafeLogger
// This header shadows Source/Dev/AudioTrace.h for test builds only

#pragma once

// Forward all AudioTrace functionality to SafeLogger
#include "../../Source/Util/SafeLogger.h"

namespace SpectralCanvas {

// Provide the g_traceBuffer symbol expected by SpectralSynthEngine
// This aliases to our SafeLogger buffer to avoid ODR violations
extern AudioTraceBuffer<4096>& g_traceBuffer;

} // namespace SpectralCanvas