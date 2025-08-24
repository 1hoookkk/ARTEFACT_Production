#include "../Source/Core/PluginProcessor.h"
#include <cstdio>

int main() {
    std::puts("Starting constructor-only test...");
    
    try {
        std::puts("Constructing ARTEFACTAudioProcessor...");
        auto* p = new ARTEFACTAudioProcessor(); // <- if this crashes, we've found the zone
        
        std::puts("Processor constructed successfully!");
        std::puts("Destroying processor...");
        delete p;
        
        std::puts("Constructor test PASSED - crash is not in processor constructor");
        return 0;
    }
    catch (...) {
        std::puts("Constructor test FAILED - crash is in processor constructor");
        return 1;
    }
}