/**
 * EMU Asset Manager
 * Handles loading and caching of AI-generated EMU visual assets
 * Integrates with LookAndFeel classes for professional EMU Audity styling
 */

#pragma once
#include <JuceHeader.h>
#include <unordered_map>
#include <memory>

/**
 * Asset types for EMU Rompler interface elements
 * Enhanced for desktop interface (1400x900px) with scaled EMU hardware aesthetics
 */
enum class EMUAssetType
{
    // Desktop Knobs and Rotary Controls (80px scale)
    DesktopFilterKnob = 0,
    DesktopVolumeKnob,
    DesktopResonanceKnob,
    DesktopLFOKnob,
    DesktopEnvelopeKnob,
    DesktopEffectsKnob,
    
    // Legacy knobs (40px scale - maintained for compatibility)
    FilterKnob,
    VolumeKnob,
    FineKnob,
    ResonanceKnob,
    
    // Desktop Panels (authentic EMU hardware scaling)
    DesktopTopPanel,        // 1400x120px top header panel
    DesktopBottomPanel,     // 1400x80px bottom control panel
    DesktopLeftPanel,       // 200x700px left side panel
    DesktopRightPanel,      // 200x700px right side panel
    DesktopCanvasFrame,     // 1040x540px canvas frame (1000x500 + 20px border)
    
    // Legacy panels (maintained for compatibility)
    LargePanel,
    SidePanel,
    HeaderPanel,
    CornerPanel,
    CanvasBackground,
    
    // Desktop LCD Displays (scaled for visibility)
    DesktopMainLCD,         // 400x60px main EMU branding display
    DesktopParameterLCD1,   // 200x30px parameter display
    DesktopParameterLCD2,   // 200x30px parameter display
    DesktopSectionLCD,      // 180x24px section labels
    
    // Legacy LCD displays
    LCDActive,
    LCDInactive,
    ParameterDisplay,
    ValueDisplay,
    
    // Desktop Buttons (100x50px EMU-style)
    DesktopSampleButton,    // Sample slot selection buttons
    DesktopPresetButton,    // Preset management buttons
    DesktopFunctionButton,  // Function/utility buttons
    DesktopToggleButton,    // Toggle switches with LED
    
    // Desktop VU Meters and Indicators (scaled up)
    DesktopVUHorizontal,    // 200x20px horizontal VU
    DesktopVUVertical,      // 20x100px vertical VU
    DesktopLEDIndicator,    // 12x12px LED indicators
    DesktopStatusLED,       // 8x8px status indicators
    
    // Legacy VU meters
    VUHorizontal,
    VUVertical,
    VUPeak,
    LEDActive,
    LEDInactive,
    
    // Desktop Special Elements
    DesktopEMUBranding,     // EMU logo and model name graphics
    DesktopMetallicTexture, // Authentic blue metallic texture
    DesktopOrangeAccent,    // Xtreme Lead-1 style orange highlights
    DesktopGreenLCDGlow,    // Audity 2000 LCD glow effect
    DesktopBrushedMetal,    // Professional brushed metal texture
    
    // Legacy special elements
    SpectrumBackground,
    BorderTexture,
    
    Count // Total number of asset types
};

/**
 * Asset loading states for progress tracking
 */
enum class AssetLoadState
{
    NotLoaded,
    Loading,
    Loaded,
    Failed
};

/**
 * Individual asset information
 */
struct EMUAsset
{
    juce::Image image;
    AssetLoadState state = AssetLoadState::NotLoaded;
    juce::String filename;
    juce::File filepath;
    juce::Time lastModified;
    
    bool isValid() const { return image.isValid() && state == AssetLoadState::Loaded; }
};

/**
 * EMU Asset Manager
 * Singleton for loading, caching, and providing AI-generated EMU visual assets
 */
class EMUAssetManager
{
public:
    static EMUAssetManager& getInstance();
    
    // Asset loading
    void loadAllAssets();
    void loadAsset(EMUAssetType type, const juce::String& filename);
    void reloadAsset(EMUAssetType type);
    void unloadAllAssets();
    
    // Asset access
    const EMUAsset& getAsset(EMUAssetType type) const;
    juce::Image getAssetImage(EMUAssetType type) const;
    bool isAssetLoaded(EMUAssetType type) const;
    
    // Asset management
    void setAssetDirectory(const juce::File& directory);
    juce::File getAssetDirectory() const;
    void refreshAssetList();
    
    // Hot reloading for development
    void enableHotReload(bool enabled);
    void checkForAssetUpdates();
    
    // Asset information
    int getLoadedAssetCount() const;
    float getLoadingProgress() const;
    juce::Array<juce::String> getFailedAssets() const;
    
    // Fallback handling
    void enableFallbackRendering(bool enabled);
    bool isFallbackRenderingEnabled() const;
    
    // Desktop interface asset scaling
    juce::Image getScaledAsset(EMUAssetType type, float scaleFactor) const;
    juce::Image getDesktopAsset(EMUAssetType type, bool highDPI = true) const;
    void setGlobalScaleFactor(float factor);
    void setDesktopMode(bool enabled);
    
    // Multi-frame asset support (for animated knobs)
    juce::Image getKnobFrame(EMUAssetType knobType, int frameIndex) const;
    int getKnobFrameCount(EMUAssetType knobType) const;
    void loadAnimatedKnob(EMUAssetType type, const juce::Array<juce::File>& frameFiles);
    
private:
    EMUAssetManager();
    ~EMUAssetManager() = default;
    
    // Asset storage
    std::array<EMUAsset, static_cast<size_t>(EMUAssetType::Count)> assets;
    
    // Configuration
    juce::File assetDirectory;
    bool hotReloadEnabled = false;
    bool fallbackEnabled = true;
    bool desktopMode = true;        // Enable desktop-specific assets
    float globalScaleFactor = 1.0f;
    
    // Desktop interface settings
    struct DesktopSettings
    {
        bool useHighDPIAssets = true;
        bool enableKnobAnimation = true;
        bool useAuthenticTextures = true;
        int maxKnobFrames = 64;
        float desktopScaleFactor = 2.0f; // 2x for desktop interface
    } desktopSettings;
    
    // Asset filename mapping
    std::unordered_map<EMUAssetType, juce::String> defaultFilenames;
    std::unordered_map<EMUAssetType, juce::String> desktopFilenames;
    
    // Animated knob frame storage
    std::unordered_map<EMUAssetType, juce::Array<juce::Image>> knobFrames;
    
    // Loading utilities
    void initializeDefaultFilenames();
    bool loadImageFile(const juce::File& file, juce::Image& image);
    juce::Image createFallbackImage(EMUAssetType type) const;
    juce::String getAssetTypeName(EMUAssetType type) const;
    juce::String getAssetSubdirectory(EMUAssetType type) const;
    
    // Desktop asset utilities
    void initializeDesktopFilenames();
    juce::Image createDesktopFallbackImage(EMUAssetType type) const;
    bool isDesktopAssetType(EMUAssetType type) const;
    EMUAssetType getLegacyAssetType(EMUAssetType desktopType) const;
    juce::String getDesktopAssetPath(EMUAssetType type) const;
    
    // Knob animation utilities
    void loadKnobAnimation(EMUAssetType type, const juce::String& basePath);
    juce::Image interpolateKnobFrame(EMUAssetType type, float position) const;
    
    // Hot reload monitoring
    juce::Time lastHotReloadCheck;
    static constexpr int HOT_RELOAD_INTERVAL_MS = 1000;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EMUAssetManager)
};

/**
 * Asset-based drawing utilities for LookAndFeel classes
 */
namespace EMUAssetDrawing
{
    // Draw scaled asset with proper aspect ratio
    void drawAsset(juce::Graphics& g, EMUAssetType type, 
                   const juce::Rectangle<int>& area,
                   juce::RectanglePlacement placement = juce::RectanglePlacement::centred);
    
    // Draw asset with custom scaling and opacity
    void drawAsset(juce::Graphics& g, EMUAssetType type,
                   const juce::Rectangle<int>& area,
                   float opacity,
                   float scaleFactor = 1.0f);
    
    // Draw tiled asset for backgrounds
    void drawTiledAsset(juce::Graphics& g, EMUAssetType type,
                        const juce::Rectangle<int>& area);
    
    // Draw asset with rotation (for knob positions)
    void drawRotatedAsset(juce::Graphics& g, EMUAssetType type,
                          const juce::Rectangle<int>& area,
                          float rotationRadians,
                          const juce::Point<float>& rotationCenter);
    
    // Asset color adjustment utilities  
    juce::Image adjustAssetColor(EMUAssetType type, 
                                const juce::Colour& tintColor,
                                float tintAmount = 0.5f);
    
    juce::Image adjustAssetBrightness(EMUAssetType type,
                                     float brightnessMultiplier);
}

/**
 * Convenience macros for asset-based drawing in LookAndFeel classes
 */
#define EMU_DRAW_ASSET(graphics, assetType, bounds) \
    EMUAssetDrawing::drawAsset(graphics, assetType, bounds)

#define EMU_DRAW_KNOB(graphics, assetType, bounds, rotation) \
    EMUAssetDrawing::drawRotatedAsset(graphics, assetType, bounds, rotation, bounds.getCentre().toFloat())

#define EMU_DRAW_BACKGROUND(graphics, assetType, bounds) \
    EMUAssetDrawing::drawTiledAsset(graphics, assetType, bounds)

/**
 * Asset initialization helper
 * Call this early in application startup
 */
void initializeEMUAssets();
void shutdownEMUAssets();