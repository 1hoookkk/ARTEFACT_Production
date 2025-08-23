#pragma once

#include <JuceHeader.h>

namespace Branding
{
    // ============================================================================
    // REALMS BRANDING SYSTEM
    // ============================================================================
    
    namespace Realms
    {
        // Primary realm names for UI navigation
        static const juce::String PAINT_REALM = "Paint Realm";
        static const juce::String MIXER_REALM = "Mixer Realm";
        static const juce::String SPECTRAL_REALM = "Spectral Realm";
        static const juce::String FORGE_REALM = "Forge Realm";
        static const juce::String PORTAL_REALM = "Portal Realm";
        
        // Mystical transformation levels
        static const juce::String TRANSFORM_SUBTLE = "Whisper";
        static const juce::String TRANSFORM_MODERATE = "Incantation";
        static const juce::String TRANSFORM_INTENSE = "Metamorphosis";
        static const juce::String TRANSFORM_EXTREME = "Transcendence";
        
        // Character switch mystery labels (purposely cryptic)
        static const juce::String CHARACTER_ALPHA = "α";
        static const juce::String CHARACTER_BETA = "β";
        static const juce::String CHARACTER_GAMMA = "γ";
        static const juce::String CHARACTER_DELTA = "δ";
        static const juce::String CHARACTER_OMEGA = "Ω";
    }
    
    namespace Colors
    {
        // Mystical color palette for vintage aesthetic
        static const juce::Colour REALM_BACKGROUND = juce::Colour(0xff1a1a1a);
        static const juce::Colour REALM_ACCENT = juce::Colour(0xff8b4513);
        static const juce::Colour MYSTICAL_GOLD = juce::Colour(0xffdaa520);
        static const juce::Colour ANCIENT_COPPER = juce::Colour(0xffb87333);
        static const juce::Colour ETHEREAL_BLUE = juce::Colour(0xff4169e1);
        static const juce::Colour SPECTRAL_RED = juce::Colour(0xffdc143c);
        static const juce::Colour SHADOW_GREY = juce::Colour(0xff2f2f2f);
        static const juce::Colour HIGHLIGHT_AMBER = juce::Colour(0xffffbf00);
        
        // Canvas specific colors
        static const juce::Colour CANVAS_BACKGROUND = juce::Colour(0xff0f0f0f);
        static const juce::Colour STROKE_PRIMARY = SPECTRAL_RED;
        static const juce::Colour GRID_LINES = juce::Colour(0xff333333);
        static const juce::Colour FREQUENCY_MARKERS = MYSTICAL_GOLD.withAlpha(0.6f);
    }
    
    namespace Typography
    {
        // Font specifications for hardware-style appearance
        static const float HEADER_SIZE = 16.0f;
        static const float CONTROL_SIZE = 12.0f;
        static const float MYSTERY_SIZE = 18.0f;
        static const float CANVAS_SIZE = 10.0f;
        
        static const juce::Font::FontStyleFlags VINTAGE_STYLE = juce::Font::bold;
        static const juce::String MYSTICAL_FONT = "Courier New";  // Monospace for mystery
    }
    
    namespace Dimensions
    {
        // Canvas specifications
        static const int CANVAS_SIZE = 128;
        static const int CANVAS_GRID_DIVISIONS = 8;
        
        // Control dimensions for hardware aesthetic
        static const int CONTROL_HEIGHT = 32;
        static const int MYSTERY_SWITCH_WIDTH = 80;
        static const int TRANSFORM_BUTTON_WIDTH = 120;
        static const int REALM_HEADER_HEIGHT = 40;
        
        // Spacing and margins
        static const int STANDARD_MARGIN = 8;
        static const int CONTROL_SPACING = 12;
        static const int REALM_PADDING = 16;
    }
    
    namespace Audio
    {
        // Character transformation parameters (EXAGGERATED values)
        namespace CharacterMaps
        {
            // Alpha - Crystalline clarity with sharp transients
            static const float ALPHA_BRIGHTNESS = 2.5f;
            static const float ALPHA_CLARITY = 3.0f;
            static const float ALPHA_TRANSIENT_BOOST = 4.0f;
            
            // Beta - Warm saturation with harmonic richness
            static const float BETA_WARMTH = 2.8f;
            static const float BETA_SATURATION = 3.5f;
            static const float BETA_HARMONIC_DENSITY = 2.2f;
            
            // Gamma - Aggressive dynamics with spectral edge
            static const float GAMMA_DYNAMICS = 4.5f;
            static const float GAMMA_SPECTRAL_TILT = 3.8f;
            static const float GAMMA_EDGE_ENHANCEMENT = 5.0f;
            
            // Delta - Deep spatial dimension with ethereal decay
            static const float DELTA_SPATIAL_WIDTH = 6.0f;
            static const float DELTA_REVERB_DEPTH = 4.2f;
            static const float DELTA_ETHEREAL_BLEND = 3.3f;
            
            // Omega - Ultimate transformation (extreme all parameters)
            static const float OMEGA_TOTAL_METAMORPHOSIS = 10.0f;
            static const float OMEGA_REALITY_DISTORTION = 8.5f;
            static const float OMEGA_DIMENSIONAL_SHIFT = 7.8f;
        }
        
        // Frequency mapping for canvas log-scale
        static const float MIN_FREQUENCY = 20.0f;
        static const float MAX_FREQUENCY = 20000.0f;
        static const float NYQUIST_REFERENCE = 22050.0f;
        
        // Transform intensities for demo mode
        static const float DEMO_INTENSITY_LOW = 0.3f;
        static const float DEMO_INTENSITY_MID = 0.7f;
        static const float DEMO_INTENSITY_HIGH = 1.0f;
    }
    
    namespace Messages
    {
        // UI feedback strings
        static const juce::String REALM_ENTERED = "Entering ";
        static const juce::String CHARACTER_SELECTED = "Character: ";
        static const juce::String TRANSFORM_INITIATED = "Transformation Initiated...";
        static const juce::String CANVAS_ACTIVE = "Canvas Activated";
        static const juce::String STROKE_CAPTURED = "Gesture Captured";
        
        // Error/warning messages
        static const juce::String AUDIO_THREAD_WARNING = "RT-Audio Thread Active";
        static const juce::String MEMORY_WARNING = "Memory Allocation Detected";
        static const juce::String LOCK_WARNING = "Lock Detected on Audio Thread";
    }
    
    namespace Config
    {
        // Performance and safety settings
        static const int MAX_STROKE_HISTORY = 1024;
        static const int PAINT_QUEUE_SIZE = 2048;
        static const float CANVAS_SENSITIVITY = 1.0f;
        static const float CHARACTER_BLEND_TIME = 0.1f;  // seconds
        static const bool ENABLE_RT_SAFETY_CHECKS = true;
        
        // Hardware simulation settings
        static const bool ENABLE_DETENTED_CONTROLS = true;
        static const int DETENT_POSITIONS = 5;  // For character switch
        static const float DETENT_SNAP_THRESHOLD = 0.1f;
        static const bool VINTAGE_RESPONSE_CURVE = true;
    }
}