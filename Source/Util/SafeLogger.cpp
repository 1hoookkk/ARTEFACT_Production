// Source/Util/SafeLogger.cpp
// RT-safe lock-free audio event logging implementation

#include "SafeLogger.h"

namespace SpectralCanvas {

//==============================================================================
// Global trace buffer instance for test suite (avoids ODR violations)
AudioTraceBuffer<4096> g_safeLoggerBuffer;

// Provide g_traceBuffer symbol for compatibility with SpectralSynthEngine
// This is the same instance as g_safeLoggerBuffer
AudioTraceBuffer<4096> g_traceBuffer;

} // namespace SpectralCanvas