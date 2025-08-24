/**
 * EMU Asset Manager Implementation
 * Handles loading and caching of AI-generated EMU visual assets
 */

#include "EMUAssetManager.h"
#include <algorithm>

//=============================================================================
// EMUAssetManager Implementation

EMUAssetManager::EMUAssetManager()
{
    // Set default asset directory relative to executable
    auto appDir = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();
    assetDirectory = appDir.getChildFile("Assets").getChildFile("EMU");
    
    // Initialize default filename mappings
    initializeDefaultFilenames();
}

EMUAssetManager& EMUAssetManager::getInstance()
{
    static EMUAssetManager instance;
    return instance;
}

void EMUAssetManager::loadAllAssets()
{
    if (!assetDirectory.exists())
    {
        DBG("EMU Asset directory does not exist: " << assetDirectory.getFullPathName());
        return;
    }
    
    // Load each asset type
    for (int i = 0; i < static_cast<int>(EMUAssetType::Count); ++i)
    {
        auto assetType = static_cast<EMUAssetType>(i);
        auto filename = defaultFilenames[assetType];
        
        if (filename.isNotEmpty())
        {
            loadAsset(assetType, filename);
        }
    }
}

void EMUAssetManager::loadAsset(EMUAssetType type, const juce::String& filename)
{
    auto index = static_cast<size_t>(type);
    if (index >= assets.size())
        return;
        
    auto& asset = assets[index];
    asset.filename = filename;
    asset.filepath = assetDirectory.getChildFile(getAssetSubdirectory(type)).getChildFile(filename);
    asset.state = AssetLoadState::Loading;
    
    if (asset.filepath.existsAsFile())
    {
        if (loadImageFile(asset.filepath, asset.image))
        {
            asset.state = AssetLoadState::Loaded;
            asset.lastModified = asset.filepath.getLastModificationTime();
            DBG("Loaded EMU asset: " << filename);
        }
        else
        {
            asset.state = AssetLoadState::Failed;
            DBG("Failed to load EMU asset: " << filename);
        }
    }
    else
    {
        asset.state = AssetLoadState::Failed;
        DBG("EMU asset file not found: " << asset.filepath.getFullPathName());
        
        // Create fallback if enabled
        if (fallbackEnabled)
        {
            asset.image = createFallbackImage(type);
            asset.state = AssetLoadState::Loaded;
        }
    }
}

void EMUAssetManager::reloadAsset(EMUAssetType type)
{
    auto index = static_cast<size_t>(type);
    if (index >= assets.size())
        return;
        
    const auto& asset = assets[index];
    if (!asset.filename.isEmpty())
    {
        loadAsset(type, asset.filename);
    }
}

void EMUAssetManager::unloadAllAssets()
{
    for (auto& asset : assets)
    {
        asset.image = juce::Image();
        asset.state = AssetLoadState::NotLoaded;
    }
}

const EMUAsset& EMUAssetManager::getAsset(EMUAssetType type) const
{
    auto index = static_cast<size_t>(type);
    if (index < assets.size())
    {
        return assets[index];
    }
    
    static EMUAsset invalidAsset;
    return invalidAsset;
}

juce::Image EMUAssetManager::getAssetImage(EMUAssetType type) const
{
    const auto& asset = getAsset(type);
    return asset.isValid() ? asset.image : juce::Image();
}

bool EMUAssetManager::isAssetLoaded(EMUAssetType type) const
{
    return getAsset(type).isValid();
}

void EMUAssetManager::setAssetDirectory(const juce::File& directory)
{
    assetDirectory = directory;
}

juce::File EMUAssetManager::getAssetDirectory() const
{
    return assetDirectory;
}

void EMUAssetManager::refreshAssetList()
{
    // Scan asset directory for new files and reload changed files
    if (hotReloadEnabled)
    {
        checkForAssetUpdates();
    }
}

void EMUAssetManager::enableHotReload(bool enabled)
{
    hotReloadEnabled = enabled;
    if (enabled)
    {
        lastHotReloadCheck = juce::Time::getCurrentTime();
    }
}

void EMUAssetManager::checkForAssetUpdates()
{
    if (!hotReloadEnabled)
        return;
        
    auto currentTime = juce::Time::getCurrentTime();
    if ((currentTime - lastHotReloadCheck).inMilliseconds() < HOT_RELOAD_INTERVAL_MS)
        return;
        
    lastHotReloadCheck = currentTime;
    
    // Check each loaded asset for modifications
    for (int i = 0; i < static_cast<int>(EMUAssetType::Count); ++i)
    {
        auto assetType = static_cast<EMUAssetType>(i);
        auto& asset = assets[i];
        
        if (asset.state == AssetLoadState::Loaded && asset.filepath.existsAsFile())
        {
            auto lastModified = asset.filepath.getLastModificationTime();
            if (lastModified > asset.lastModified)
            {
                DBG("Reloading modified EMU asset: " << asset.filename);
                reloadAsset(assetType);
            }
        }
    }
}

int EMUAssetManager::getLoadedAssetCount() const
{
    int count = 0;
    for (const auto& asset : assets)
    {
        if (asset.state == AssetLoadState::Loaded)
            count++;
    }
    return count;
}

float EMUAssetManager::getLoadingProgress() const
{
    int totalAssets = static_cast<int>(EMUAssetType::Count);
    int loadedAssets = getLoadedAssetCount();
    return totalAssets > 0 ? (float)loadedAssets / totalAssets : 0.0f;
}

juce::Array<juce::String> EMUAssetManager::getFailedAssets() const
{
    juce::Array<juce::String> failedAssets;
    
    for (int i = 0; i < static_cast<int>(EMUAssetType::Count); ++i)
    {
        const auto& asset = assets[i];
        if (asset.state == AssetLoadState::Failed)
        {
            failedAssets.add(getAssetTypeName(static_cast<EMUAssetType>(i)) + ": " + asset.filename);
        }
    }
    
    return failedAssets;
}

void EMUAssetManager::enableFallbackRendering(bool enabled)
{
    fallbackEnabled = enabled;
}

bool EMUAssetManager::isFallbackRenderingEnabled() const
{
    return fallbackEnabled;
}

juce::Image EMUAssetManager::getScaledAsset(EMUAssetType type, float scaleFactor) const
{
    auto originalImage = getAssetImage(type);
    if (!originalImage.isValid())
        return originalImage;
        
    float totalScale = scaleFactor * globalScaleFactor;
    if (totalScale == 1.0f)
        return originalImage;
        
    int newWidth = (int)(originalImage.getWidth() * totalScale);
    int newHeight = (int)(originalImage.getHeight() * totalScale);
    
    return originalImage.rescaled(newWidth, newHeight, juce::Graphics::highResamplingQuality);
}

void EMUAssetManager::setGlobalScaleFactor(float factor)
{
    globalScaleFactor = juce::jmax(0.1f, factor);
}

void EMUAssetManager::initializeDefaultFilenames()
{
    // Knobs and rotary controls
    defaultFilenames[EMUAssetType::FilterKnob] = "filter_knob_001.png";
    defaultFilenames[EMUAssetType::VolumeKnob] = "volume_knob_001.png";
    defaultFilenames[EMUAssetType::FineKnob] = "fine_knob_001.png";
    defaultFilenames[EMUAssetType::ResonanceKnob] = "resonance_knob_001.png";
    
    // Panels and backgrounds
    defaultFilenames[EMUAssetType::LargePanel] = "main_panel_large.png";
    defaultFilenames[EMUAssetType::SidePanel] = "side_panel_medium.png";
    defaultFilenames[EMUAssetType::HeaderPanel] = "header_panel.png";
    defaultFilenames[EMUAssetType::CornerPanel] = "corner_detail.png";
    defaultFilenames[EMUAssetType::CanvasBackground] = "canvas_background_800x400.png";
    defaultFilenames[EMUAssetType::BorderTexture] = "canvas_border_frame.png";
    
    // LCD displays and readouts
    defaultFilenames[EMUAssetType::LCDActive] = "lcd_active_001.png";
    defaultFilenames[EMUAssetType::LCDInactive] = "lcd_inactive_001.png";
    defaultFilenames[EMUAssetType::ParameterDisplay] = "parameter_display.png";
    defaultFilenames[EMUAssetType::ValueDisplay] = "value_display.png";
    
    // VU meters and indicators
    defaultFilenames[EMUAssetType::VUHorizontal] = "vu_horizontal_001.png";
    defaultFilenames[EMUAssetType::VUVertical] = "vu_vertical_001.png";
    defaultFilenames[EMUAssetType::VUPeak] = "vu_peak_001.png";
    defaultFilenames[EMUAssetType::LEDActive] = "led_active.png";
    defaultFilenames[EMUAssetType::LEDInactive] = "led_inactive.png";
    
    // Special elements
    defaultFilenames[EMUAssetType::SpectrumBackground] = "spectrum_background.png";
    defaultFilenames[EMUAssetType::BorderTexture] = "border_texture.png";
}

bool EMUAssetManager::loadImageFile(const juce::File& file, juce::Image& image)
{
    if (!file.existsAsFile())
        return false;
        
    juce::PNGImageFormat pngFormat;
    juce::JPEGImageFormat jpegFormat;
    
    juce::FileInputStream stream(file);
    if (!stream.openedOk())
        return false;
        
    // Try PNG first, then JPEG
    if (pngFormat.canUnderstand(stream))
    {
        stream.setPosition(0);
        image = pngFormat.decodeImage(stream);
    }
    else if (jpegFormat.canUnderstand(stream))
    {
        stream.setPosition(0);
        image = jpegFormat.decodeImage(stream);
    }
    else
    {
        return false;
    }
    
    return image.isValid();
}

juce::Image EMUAssetManager::createFallbackImage(EMUAssetType type) const
{
    // Create simple colored rectangles as fallback
    int width = 64, height = 64;
    juce::Colour fallbackColor = juce::Colours::darkblue;
    
    switch (type)
    {
        case EMUAssetType::FilterKnob:
        case EMUAssetType::VolumeKnob:
        case EMUAssetType::FineKnob:
        case EMUAssetType::ResonanceKnob:
            // Circular fallback for knobs
            {
                auto image = juce::Image(juce::Image::ARGB, width, height, true);
                juce::Graphics g(image);
                g.setColour(fallbackColor);
                g.fillEllipse(2, 2, width-4, height-4);
                g.setColour(juce::Colours::white);
                g.drawEllipse(2, 2, width-4, height-4, 2);
                return image;
            }
            
        case EMUAssetType::LargePanel:
            width = 256; height = 128;
            break;
            
        case EMUAssetType::CanvasBackground:
            width = 800; height = 400; // Proper canvas dimensions
            fallbackColor = juce::Colour(30, 58, 95); // EMU metallic blue
            {
                auto image = juce::Image(juce::Image::ARGB, width, height, true);
                juce::Graphics g(image);
                
                // Professional EMU-style brushed metal background
                g.setColour(fallbackColor);
                g.fillRect(0, 0, width, height);
                
                // Add subtle horizontal brushed metal lines
                g.setColour(fallbackColor.brighter(0.05f));
                for (int y = 0; y < height; y += 3)
                {
                    g.fillRect(0, y, width, 1);
                }
                
                // Add very subtle vertical grain
                g.setColour(fallbackColor.brighter(0.02f));
                for (int x = 0; x < width; x += 8)
                {
                    g.fillRect(x, 0, 1, height);
                }
                
                return image;
            }
            break;
            
        case EMUAssetType::BorderTexture:
            width = 808; height = 408; // Canvas + border
            fallbackColor = juce::Colour(74, 127, 187); // Light EMU blue
            {
                auto image = juce::Image(juce::Image::ARGB, width, height, true);
                juce::Graphics g(image);
                g.setColour(juce::Colours::transparentBlack);
                g.fillAll();
                
                // Professional beveled border
                auto bounds = juce::Rectangle<int>(0, 0, width, height);
                
                // Outer highlight
                g.setColour(fallbackColor);
                g.drawRect(bounds, 2);
                
                // Inner shadow
                g.setColour(juce::Colour(10, 22, 40)); // Dark EMU blue
                g.drawRect(bounds.reduced(1), 1);
                
                return image;
            }
            
        default:
            break;
    }
    
    auto image = juce::Image(juce::Image::ARGB, width, height, true);
    juce::Graphics g(image);
    g.setColour(fallbackColor);
    g.fillRect(0, 0, width, height);
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.drawRect(0, 0, width, height, 1);
    
    return image;
}

juce::String EMUAssetManager::getAssetTypeName(EMUAssetType type) const
{
    switch (type)
    {
        case EMUAssetType::FilterKnob: return "FilterKnob";
        case EMUAssetType::VolumeKnob: return "VolumeKnob";
        case EMUAssetType::FineKnob: return "FineKnob";
        case EMUAssetType::ResonanceKnob: return "ResonanceKnob";
        case EMUAssetType::LargePanel: return "LargePanel";
        case EMUAssetType::SidePanel: return "SidePanel";
        case EMUAssetType::HeaderPanel: return "HeaderPanel";
        case EMUAssetType::CornerPanel: return "CornerPanel";
        case EMUAssetType::CanvasBackground: return "CanvasBackground";
        case EMUAssetType::LCDActive: return "LCDActive";
        case EMUAssetType::LCDInactive: return "LCDInactive";
        case EMUAssetType::ParameterDisplay: return "ParameterDisplay";
        case EMUAssetType::ValueDisplay: return "ValueDisplay";
        case EMUAssetType::VUHorizontal: return "VUHorizontal";
        case EMUAssetType::VUVertical: return "VUVertical";
        case EMUAssetType::VUPeak: return "VUPeak";
        case EMUAssetType::LEDActive: return "LEDActive";
        case EMUAssetType::LEDInactive: return "LEDInactive";
        case EMUAssetType::SpectrumBackground: return "SpectrumBackground";
        case EMUAssetType::BorderTexture: return "BorderTexture";
        default: return "Unknown";
    }
}

juce::String EMUAssetManager::getAssetSubdirectory(EMUAssetType type) const
{
    switch (type)
    {
        case EMUAssetType::FilterKnob:
        case EMUAssetType::VolumeKnob:
        case EMUAssetType::FineKnob:
        case EMUAssetType::ResonanceKnob:
            return "Knobs";
            
        case EMUAssetType::LargePanel:
        case EMUAssetType::SidePanel:
        case EMUAssetType::HeaderPanel:
        case EMUAssetType::CornerPanel:
        case EMUAssetType::CanvasBackground:
        case EMUAssetType::BorderTexture:
            return "Panels";
            
        case EMUAssetType::LCDActive:
        case EMUAssetType::LCDInactive:
        case EMUAssetType::ParameterDisplay:
        case EMUAssetType::ValueDisplay:
        case EMUAssetType::LEDActive:
        case EMUAssetType::LEDInactive:
            return "Displays";
            
        case EMUAssetType::VUHorizontal:
        case EMUAssetType::VUVertical:
        case EMUAssetType::VUPeak:
        case EMUAssetType::SpectrumBackground:
            return "Meters";
            
        default:
            return "";
    }
}

//=============================================================================
// EMUAssetDrawing Utilities

namespace EMUAssetDrawing
{
    void drawAsset(juce::Graphics& g, EMUAssetType type, 
                   const juce::Rectangle<int>& area,
                   juce::RectanglePlacement placement)
    {
        auto& manager = EMUAssetManager::getInstance();
        auto image = manager.getAssetImage(type);
        
        if (image.isValid())
        {
            g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
            g.drawImageWithin(image, area.getX(), area.getY(), 
                            area.getWidth(), area.getHeight(), 
                            placement, false);
        }
        else if (manager.isFallbackRenderingEnabled())
        {
            // Draw simple colored rectangle as fallback
            g.setColour(juce::Colours::darkblue.withAlpha(0.5f));
            g.fillRect(area);
            g.setColour(juce::Colours::white.withAlpha(0.3f));
            g.drawRect(area, 1);
        }
    }
    
    void drawAsset(juce::Graphics& g, EMUAssetType type,
                   const juce::Rectangle<int>& area,
                   float opacity,
                   float scaleFactor)
    {
        auto& manager = EMUAssetManager::getInstance();
        auto image = manager.getScaledAsset(type, scaleFactor);
        
        if (image.isValid())
        {
            g.setOpacity(opacity);
            g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
            g.drawImageWithin(image, area.getX(), area.getY(), 
                            area.getWidth(), area.getHeight(), 
                            juce::RectanglePlacement::centred, false);
        }
    }
    
    void drawTiledAsset(juce::Graphics& g, EMUAssetType type,
                        const juce::Rectangle<int>& area)
    {
        auto& manager = EMUAssetManager::getInstance();
        auto image = manager.getAssetImage(type);
        
        if (image.isValid())
        {
            g.setTiledImageFill(image, 0, 0, 1.0f);
            g.fillRect(area);
        }
    }
    
    void drawRotatedAsset(juce::Graphics& g, EMUAssetType type,
                          const juce::Rectangle<int>& area,
                          float rotationRadians,
                          const juce::Point<float>& rotationCenter)
    {
        auto& manager = EMUAssetManager::getInstance();
        auto image = manager.getAssetImage(type);
        
        if (image.isValid())
        {
            auto transform = juce::AffineTransform::rotation(rotationRadians, 
                                                           rotationCenter.x, 
                                                           rotationCenter.y);
            
            g.saveState();
            g.addTransform(transform);
            g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
            g.drawImageWithin(image, area.getX(), area.getY(), 
                            area.getWidth(), area.getHeight(), 
                            juce::RectanglePlacement::centred, false);
            g.restoreState();
        }
    }
    
    juce::Image adjustAssetColor(EMUAssetType type, 
                                const juce::Colour& tintColor,
                                float tintAmount)
    {
        auto& manager = EMUAssetManager::getInstance();
        auto originalImage = manager.getAssetImage(type);
        
        if (!originalImage.isValid())
            return originalImage;
            
        auto tintedImage = originalImage.createCopy();
        
        for (int y = 0; y < tintedImage.getHeight(); ++y)
        {
            for (int x = 0; x < tintedImage.getWidth(); ++x)
            {
                auto pixel = tintedImage.getPixelAt(x, y);
                auto tintedPixel = pixel.interpolatedWith(tintColor, tintAmount);
                tintedImage.setPixelAt(x, y, tintedPixel);
            }
        }
        
        return tintedImage;
    }
    
    juce::Image adjustAssetBrightness(EMUAssetType type,
                                     float brightnessMultiplier)
    {
        auto& manager = EMUAssetManager::getInstance();
        auto originalImage = manager.getAssetImage(type);
        
        if (!originalImage.isValid())
            return originalImage;
            
        auto adjustedImage = originalImage.createCopy();
        
        for (int y = 0; y < adjustedImage.getHeight(); ++y)
        {
            for (int x = 0; x < adjustedImage.getWidth(); ++x)
            {
                auto pixel = adjustedImage.getPixelAt(x, y);
                auto newPixel = pixel.brighter(brightnessMultiplier - 1.0f);
                adjustedImage.setPixelAt(x, y, newPixel);
            }
        }
        
        return adjustedImage;
    }
}

//=============================================================================
// Global Asset Management Functions

void initializeEMUAssets()
{
    auto& manager = EMUAssetManager::getInstance();
    
    // Set asset directory relative to the application
    auto assetsDir = juce::File::getSpecialLocation(juce::File::currentApplicationFile)
                        .getParentDirectory()
                        .getChildFile("Source")
                        .getChildFile("UI")
                        .getChildFile("Assets");
    
    manager.setAssetDirectory(assetsDir);
    manager.enableFallbackRendering(true);
    manager.loadAllAssets();
    
    DBG("EMU Asset Manager initialized: " << manager.getLoadedAssetCount() 
        << "/" << static_cast<int>(EMUAssetType::Count) << " assets loaded");
    
    auto failedAssets = manager.getFailedAssets();
    if (!failedAssets.isEmpty())
    {
        DBG("Failed to load assets:");
        for (const auto& failed : failedAssets)
        {
            DBG("  " << failed);
        }
    }
}

void shutdownEMUAssets()
{
    EMUAssetManager::getInstance().unloadAllAssets();
    DBG("EMU Asset Manager shut down");
}