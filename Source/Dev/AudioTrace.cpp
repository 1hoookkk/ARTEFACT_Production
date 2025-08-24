// Source/Dev/AudioTrace.cpp
// RT-safe lock-free audio event tracing implementation

#include "AudioTrace.h"

namespace SpectralCanvas {

//==============================================================================
// Global trace buffer instance for SpectralCanvas Pro
AudioTraceBuffer<4096> g_traceBuffer;

} // namespace SpectralCanvas