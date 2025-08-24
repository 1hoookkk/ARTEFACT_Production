#pragma once

#include <cstdlib>
#include <atomic>
#include <string>

namespace SpectralCanvas {
namespace Config {

    // Launch mode detection (minimal for baseline)
    enum class LaunchMode {
        Minimal = 0,  // VST3 compliance only
        Safe = 1,     // Essential systems
        Normal = 2,   // Full features  
        Debug = 3     // All diagnostics
    };
    
    extern std::atomic<LaunchMode> g_LaunchMode;
    
    // Basic mode queries
    inline bool IsMinimalModeActive() { return g_LaunchMode.load() == LaunchMode::Minimal; }
    inline bool IsSafeModeActive() { return g_LaunchMode.load() == LaunchMode::Safe; }
    inline bool IsFullModeActive() { return g_LaunchMode.load() == LaunchMode::Normal; }
    inline bool IsDebugModeActive() { return g_LaunchMode.load() == LaunchMode::Debug; }
    
    // Environment variable overrides (minimal)
    static bool IsMulticoreDspEnabled() {
        if (g_LaunchMode.load() == LaunchMode::Minimal || g_LaunchMode.load() == LaunchMode::Safe) {
            return false;  // Hard-off for baseline stability
        }
        const char* forceSingle = std::getenv("SC_FORCE_SINGLECORE");
        if (forceSingle && std::atoi(forceSingle) != 0) return false;
        const char* forceMulti = std::getenv("SC_FORCE_MULTICORE");
        if (forceMulti && std::atoi(forceMulti) != 0) return true;
        return false;  // Default OFF for baseline stability
    }
    
    // Feature namespaces (minimal stubs)
    namespace EngineFeatures {
        inline bool IsSpectralEngineEnabled() { return false; }  // Baseline: disabled
        inline bool IsMulticoreDspEnabled() { return Config::IsMulticoreDspEnabled(); }
        inline bool IsEffectsRackEnabled() { return false; }     // Baseline: disabled
        inline bool IsWavetableSynthEnabled() { return false; }  // Baseline: disabled
    }
    
    namespace UIFeatures {
        inline bool IsCanvasRenderingEnabled() { return false; }  // Baseline: disabled
        inline bool IsAdvancedGraphicsEnabled() { return false; } // Baseline: disabled
    }
    
    // Hierarchical safety checks (minimal enabled for HUD validation)
    inline bool ShouldAllocateSpectralEngines() { return true; }   // Enabled: for HUD testing
    inline bool ShouldInitializeLayerManager() { return true; }    // Enabled: for HUD testing
    inline bool ShouldCreateComplexUI() { return false; }          // Baseline: disabled
    
    // Command line initialization
    void InitializeFromCommandLine(const char* commandLine);
    std::string GetConfigurationSummary();

} // namespace Config
} // namespace SpectralCanvas