#pragma once
#include <JuceHeader.h>
#include "../Core/CrashToggles.h"

#if CrashToggles::ENABLE_TEXTURE_LOADING

// ==============================================================================
// ASSET REGISTRY SYSTEM
// Centralized PNG asset management for SpectralCanvas Pro
// Loads and caches assets from Source/assets_rt/{panels,sprites,frames}
// ==============================================================================

class AssetRegistry
{
public:
    // Singleton access
    static AssetRegistry& get();
    
    // ==============================================================================
    // INITIALIZATION
    // ==============================================================================
    
    // Set root directory for asset loading (for development)
    void setSearchRoot(const juce::File& root);
    
    // Initialize asset loading - call once at startup
    void initialize();
    
    // Check if assets are loaded and ready
    bool isReady() const { return assetsLoaded; }
    
    // ==============================================================================
    // ASSET ACCESS BY CATEGORY
    // ==============================================================================
    
    // Panel backgrounds and textures
    juce::Image getPanel(const juce::String& name);
    
    // UI sprites (knobs, buttons, LEDs, etc.)
    juce::Image getSprite(const juce::String& name);
    
    // Frame elements (bezels, borders)
    juce::Image getFrame(const juce::String& name);
    
    // ==============================================================================
    // PREDEFINED ASSETS
    // Common assets with friendly names
    // ==============================================================================
    
    // Panel assets
    juce::Image getTealPanel();          // Main teal panel background
    juce::Image getGraphiteRail();       // Graphite rail element
    juce::Image getAluminumNameplate();  // Aluminum nameplate
    
    // Sprite assets
    juce::Image getHardwareKnob();       // Main hardware knob
    juce::Image getSynthKnob();          // Alternative synth knob
    juce::Image getControlKnob();        // Control knob variant
    juce::Image getGraphiteButton();     // Graphite button
    juce::Image getMatteButton();        // Matte button variant
    juce::Image getToggleSwitch();       // Toggle switch
    
    // LED and display assets
    juce::Image getGreenLED();           // Green LED indicator
    juce::Image getRedLED();             // Red LED indicator
    juce::Image getDarkGreenLEDSegment(); // Dark green LED segment
    juce::Image getLimeLCD();            // Lime green LCD display
    juce::Image getLCDSegment();         // LCD segment render
    juce::Image getVUMeter();            // VU meter element
    
    // ==============================================================================
    // ASSET UTILITIES
    // ==============================================================================
    
    // Get all available asset names in a category
    juce::StringArray getPanelNames() const;
    juce::StringArray getSpriteNames() const;
    juce::StringArray getFrameNames() const;
    
    // Asset information
    juce::String getAssetPath(const juce::String& category, const juce::String& name) const;
    int getAssetCount() const;
    
    // ==============================================================================
    // DEVELOPMENT HELPERS
    // ==============================================================================
    
    // Log all available assets to debug console
    void logAvailableAssets() const;
    
    // Validate that all expected assets are present
    bool validateAssets() const;
    
private:
    AssetRegistry() = default;
    ~AssetRegistry() = default;
    
    // Asset loading
    juce::Image loadAsset(const juce::File& file);
    juce::Image loadAssetFromCategory(const juce::String& category, const juce::String& name);
    void scanDirectory(const juce::File& dir, juce::StringArray& names) const;
    
    // Asset caching using JUCE's built-in cache
    juce::String makeCacheKey(const juce::String& category, const juce::String& name) const;
    
    // Member variables
    juce::File assetsRoot;
    bool assetsLoaded = false;
    
    // Asset name mappings (for friendly names)
    struct AssetMapping {
        juce::String friendlyName;
        juce::String category;
        juce::String fileName;
    };
    
    static const AssetMapping assetMappings[];
    static const int numAssetMappings;
    
    // Asset validation
    struct ExpectedAsset {
        juce::String category;
        juce::String pattern;  // Partial filename match
        bool required;
    };
    
    static const ExpectedAsset expectedAssets[];
    static const int numExpectedAssets;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AssetRegistry)
};

#endif // CrashToggles::ENABLE_TEXTURE_LOADING