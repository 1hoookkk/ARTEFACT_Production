/*
  ==============================================================================

    ParamIDs.h
    Created: 16 Aug 2025
    Author:  SpectralCanvas Pro Team

    Parameter ID constants for SpectralCanvas Pro.
    
    This file serves as the single source of truth for all parameter ID strings
    used throughout the application. All parameter references should use these
    constants to ensure consistency and prevent typos.

  ==============================================================================
*/

#pragma once

namespace ParamIDs
{
    //==============================================================================
    // MASTER SECTION
    // Core control parameters available in all safety modes
    
    static constexpr const char* const masterGain = "masterGain";
    static constexpr const char* const paintActive = "paintActive";
    static constexpr const char* const processingMode = "processingMode";

    //==============================================================================
    // HEARTBEAT SLICE SECTION
    // Core paint-to-audio engine parameters - the heart of SpectralCanvas Pro
    
    static constexpr const char* const CPS = "CPS";                    // Columns Per Second - scan speed
    static constexpr const char* const Gamma = "Gamma";                // Brightness Gamma - color mapping
    static constexpr const char* const TopN = "TopN";                  // Band Density - spectral focus

    //==============================================================================
    // PAINT ENGINE SECTION
    // Advanced painting and interaction parameters (safe+ modes only)
    
    static constexpr const char* const brushSize = "brushSize";
    static constexpr const char* const pressureSensitivity = "pressureSensitivity";
    static constexpr const char* const colorIntensity = "colorIntensity";
    static constexpr const char* const frequencyRange = "frequencyRange";
    static constexpr const char* const paintDecay = "paintDecay";
    static constexpr const char* const paintMode = "paintMode";
    static constexpr const char* const spatialWidth = "spatialWidth";
    static constexpr const char* const quantizeToKey = "quantizeToKey";

    //==============================================================================
    // SYNTHESIS ENGINE SECTION
    // Spectral synthesis and oscillator parameters
    
    static constexpr const char* const oscillatorCount = "oscillatorCount";
    static constexpr const char* const spectralMode = "spectralMode";
    static constexpr const char* const filterCutoff = "filterCutoff";
    static constexpr const char* const filterResonance = "filterResonance";
    static constexpr const char* const spectralMorph = "spectralMorph";
    static constexpr const char* const harmonicContent = "harmonicContent";
    static constexpr const char* const topNBands = "topNBands";

    //==============================================================================
    // EFFECTS SECTION
    // Audio effects and processing parameters
    
    static constexpr const char* const reverbAmount = "reverbAmount";
    static constexpr const char* const delayAmount = "delayAmount";
    static constexpr const char* const distortionAmount = "distortionAmount";
    static constexpr const char* const chorusAmount = "chorusAmount";

    //==============================================================================
    // PERFORMANCE SECTION
    // System performance and optimization parameters
    
    static constexpr const char* const cpuLimit = "cpuLimit";
    static constexpr const char* const qualityMode = "qualityMode";
    static constexpr const char* const latencyMs = "latencyMs";
    static constexpr const char* const adaptivePerformance = "adaptivePerformance";

    //==============================================================================
    // LAYER MANAGEMENT SECTION
    // Multi-layer painting and blending parameters
    
    static constexpr const char* const activeLayer = "activeLayer";
    static constexpr const char* const layerOpacity = "layerOpacity";
    static constexpr const char* const layerBlendMode = "layerBlendMode";

    //==============================================================================
    // MASK SNAPSHOT SECTION
    // Advanced masking and temporal effects parameters
    
    static constexpr const char* const maskBlend = "maskBlend";
    static constexpr const char* const maskStrength = "maskStrength";
    static constexpr const char* const featherTime = "featherTime";
    static constexpr const char* const featherFreq = "featherFreq";
    static constexpr const char* const threshold = "threshold";
    static constexpr const char* const protectHarmonics = "protectHarmonics";

    //==============================================================================
    // PARAMETER GROUPS
    // Convenience collections for bulk operations
    
    namespace Groups
    {
        // Essential parameters available in minimal mode
        static constexpr const char* const essential[] = {
            masterGain
        };
        
        // Core paint-to-audio parameters (heartbeat slice)
        static constexpr const char* const heartbeatSlice[] = {
            CPS, Gamma, TopN
        };
        
        // Master control parameters
        static constexpr const char* const master[] = {
            masterGain, paintActive, processingMode
        };
        
        // Real-time critical parameters (affect audio thread)
        static constexpr const char* const rtCritical[] = {
            masterGain, paintActive, CPS, Gamma, TopN, 
            oscillatorCount, filterCutoff, filterResonance
        };
        
        // UI-only parameters (do not affect audio processing)
        static constexpr const char* const uiOnly[] = {
            activeLayer, layerOpacity, layerBlendMode,
            cpuLimit, qualityMode, latencyMs
        };
    }

    //==============================================================================
    // PARAMETER COUNTS
    // For validation and array sizing
    
    static constexpr int TOTAL_PARAMETERS = 35;
    static constexpr int ESSENTIAL_COUNT = 1;
    static constexpr int MASTER_COUNT = 3;
    static constexpr int HEARTBEAT_SLICE_COUNT = 3;
    static constexpr int PAINT_ENGINE_COUNT = 8;
    static constexpr int SYNTHESIS_ENGINE_COUNT = 7;
    static constexpr int EFFECTS_COUNT = 4;
    static constexpr int PERFORMANCE_COUNT = 4;
    static constexpr int LAYER_MANAGEMENT_COUNT = 3;
    static constexpr int MASK_SNAPSHOT_COUNT = 6;

    //==============================================================================
    // UTILITY FUNCTIONS
    // Helper functions for parameter management
    
    /**
     * Check if a parameter ID is in the essential (minimal mode) group
     */
    inline bool isEssentialParameter(const char* paramID)
    {
        return (strcmp(paramID, masterGain) == 0);
    }
    
    /**
     * Check if a parameter ID is in the heartbeat slice (core) group
     */
    inline bool isHeartbeatSliceParameter(const char* paramID)
    {
        return (strcmp(paramID, CPS) == 0 ||
                strcmp(paramID, Gamma) == 0 ||
                strcmp(paramID, TopN) == 0);
    }
    
    /**
     * Check if a parameter ID is real-time critical (affects audio thread)
     */
    inline bool isRealTimeCritical(const char* paramID)
    {
        return (strcmp(paramID, masterGain) == 0 ||
                strcmp(paramID, paintActive) == 0 ||
                strcmp(paramID, CPS) == 0 ||
                strcmp(paramID, Gamma) == 0 ||
                strcmp(paramID, TopN) == 0 ||
                strcmp(paramID, oscillatorCount) == 0 ||
                strcmp(paramID, filterCutoff) == 0 ||
                strcmp(paramID, filterResonance) == 0);
    }
}