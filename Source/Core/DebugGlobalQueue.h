#pragma once
#include "PaintQueue.h" // or include the project queue type header
namespace dbg {
    // Temporary global queue instance (single-producer single-consumer)
    inline static SpectralPaintQueue* globalPaintQueue = nullptr;
}