// Tests/Support/Dev/AudioTrace.h
// Test-only AudioTrace shim that forwards to SafeLogger
// This shadows Source/Dev/AudioTrace.h for relative includes

#pragma once

// Forward all AudioTrace functionality to SafeLogger
#include "../../../Source/Util/SafeLogger.h"

namespace SpectralCanvas {

// Use the same API as original AudioTrace but with SafeLogger backend
// This avoids ODR violations while maintaining compatibility
using AudioTraceBuffer = AudioTraceBuffer<4096>;
using AudioTraceEvent = AudioTraceEvent;
using AudioEventType = AudioEventType;

// Reference to SafeLogger buffer (defined in SafeLogger.cpp)
extern AudioTraceBuffer<4096>& g_traceBuffer;

// Import all the logging functions from SafeLogger
using SpectralCanvas::logAudioEvent;
using SpectralCanvas::readAudioEvent;
using SpectralCanvas::getAudioEventCount;
using SpectralCanvas::getDroppedEventCount;
using SpectralCanvas::resetDroppedEventCount;

} // namespace SpectralCanvas