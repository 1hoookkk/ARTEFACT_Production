#include "AssetRegistry.h"
#include "../Core/CrashToggles.h"

#if CrashToggles::ENABLE_TEXTURE_LOADING

// ==============================================================================
// ASSET MAPPINGS
// Map friendly names to actual file patterns
// ==============================================================================

const AssetRegistry::AssetMapping AssetRegistry::assetMappings[] = {
    // Panel assets
    {"TealPanel",       "panels", "Matte_Teal_Panel"},
    {"GraphiteRail",    "panels", "Graphite_Rail"},
    {"AluminumPlate",   "panels", "Aluminum_Nameplate"},
    
    // Knob sprites
    {"HardwareKnob",    "sprites", "Hardware_Knob_gen_01k2grf20refs"},
    {"SynthKnob",       "sprites", "Synth_Knob"},
    {"ControlKnob",     "sprites", "Control_Knob"},
    
    // Button sprites
    {"GraphiteButton",  "sprites", "Graphite_Hardware_Button"},
    {"MatteButton",     "sprites", "Matte_Graphite_Button"},
    {"ToggleSwitch",    "sprites", "Toggle_Switch"},
    
    // LED and display sprites
    {"GreenLED",        "sprites", "Green_LED_Display"},
    {"RedLED",          "sprites", "Red_LED_Indicator"},
    {"DarkGreenSegment", "sprites", "Dark_Green_LED_Segment"},
    {"LimeLCD",         "sprites", "Lime_Green_LCD"},
    {"LCDSegment",      "sprites", "LCD_Segment"},
    {"VUMeter",         "sprites", "VU_Meter"}
};

const int AssetRegistry::numAssetMappings = sizeof(assetMappings) / sizeof(AssetMapping);

// Expected assets for validation
const AssetRegistry::ExpectedAsset AssetRegistry::expectedAssets[] = {
    {"panels",  "Teal_Panel",     true},
    {"sprites", "Hardware_Knob",  true},
    {"sprites", "LED",           false},
    {"sprites", "Button",        false}
};

const int AssetRegistry::numExpectedAssets = sizeof(expectedAssets) / sizeof(ExpectedAsset);

// ==============================================================================
// SINGLETON AND INITIALIZATION
// ==============================================================================

AssetRegistry& AssetRegistry::get()
{
    static AssetRegistry instance;
    return instance;
}

void AssetRegistry::setSearchRoot(const juce::File& root)
{
    assetsRoot = root;
    DBG("AssetRegistry: Search root set to " + root.getFullPathName());
}

void AssetRegistry::initialize()
{
    if (assetsLoaded)
    {
        DBG("AssetRegistry: Already initialized");
        return;
    }
    
    // Default to assets_rt relative to executable
    if (!assetsRoot.exists())
    {
        auto executableDir = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
                            .getParentDirectory();
        
        // Try multiple possible locations
        juce::Array<juce::File> possibleRoots = {
            executableDir.getChildFile("../../Source/assets_rt"),      // From build folder
            executableDir.getChildFile("../Source/assets_rt"),        // From parent
            executableDir.getChildFile("Source/assets_rt"),           // Direct
            executableDir.getChildFile("assets_rt")                   // Root level
        };
        
        for (auto& candidate : possibleRoots)
        {
            if (candidate.exists() && candidate.isDirectory())
            {
                assetsRoot = candidate;
                DBG("AssetRegistry: Found assets root at " + candidate.getFullPathName());
                break;
            }
        }
    }
    
    if (!assetsRoot.exists())
    {
        DBG("AssetRegistry: WARNING - Assets root not found! Asset loading will fail.");
        return;
    }
    
    // Validate directory structure
    auto panelsDir = assetsRoot.getChildFile("panels");
    auto spritesDir = assetsRoot.getChildFile("sprites");
    auto framesDir = assetsRoot.getChildFile("frames");
    
    DBG("AssetRegistry: Scanning asset directories...");
    DBG("  Panels dir: " + juce::String(panelsDir.exists() ? "Found" : "Missing"));
    DBG("  Sprites dir: " + juce::String(spritesDir.exists() ? "Found" : "Missing"));  
    DBG("  Frames dir: " + juce::String(framesDir.exists() ? "Found" : "Missing"));
    
    assetsLoaded = true;
    logAvailableAssets();
}

// ==============================================================================
// ASSET LOADING
// ==============================================================================

juce::Image AssetRegistry::loadAsset(const juce::File& file)
{
    if (!file.existsAsFile())
    {
        DBG("AssetRegistry: File not found: " + file.getFullPathName());
        return {};
    }
    
    auto image = juce::ImageFileFormat::loadFrom(file);
    if (image.isNull())
    {
        DBG("AssetRegistry: Failed to load image: " + file.getFullPathName());
        return {};
    }
    
    DBG("AssetRegistry: Loaded " + file.getFileName() + 
        " (" + juce::String(image.getWidth()) + "x" + juce::String(image.getHeight()) + ")");
    
    return image;
}

juce::Image AssetRegistry::loadAssetFromCategory(const juce::String& category, const juce::String& name)
{
    if (!assetsLoaded)
        initialize();
    
    auto categoryDir = assetsRoot.getChildFile(category);
    if (!categoryDir.exists())
    {
        DBG("AssetRegistry: Category directory not found: " + category);
        return {};
    }
    
    // Use JUCE ImageCache for efficient caching
    auto cacheKey = makeCacheKey(category, name);
    auto cachedImage = juce::ImageCache::getFromHashCode(cacheKey.hashCode());
    if (cachedImage.isValid())
    {
        return cachedImage;
    }
    
    // Try to find file by partial name match
    for (juce::DirectoryIterator iter(categoryDir, false, "*.png", juce::File::findFiles); iter.next();)
    {
        auto file = iter.getFile();
        auto fileName = file.getFileNameWithoutExtension();
        
        if (fileName.contains(name))
        {
            auto image = loadAsset(file);
            if (image.isValid())
            {
                // Cache the loaded image
                juce::ImageCache::addImageToCache(image, cacheKey.hashCode());
                return image;
            }
        }
    }
    
    DBG("AssetRegistry: Asset not found: " + category + "/" + name);
    return {};
}

juce::String AssetRegistry::makeCacheKey(const juce::String& category, const juce::String& name) const
{
    return "AssetRegistry_" + category + "_" + name;
}

// ==============================================================================
// PUBLIC ASSET ACCESS
// ==============================================================================

juce::Image AssetRegistry::getPanel(const juce::String& name)
{
    return loadAssetFromCategory("panels", name);
}

juce::Image AssetRegistry::getSprite(const juce::String& name) 
{
    return loadAssetFromCategory("sprites", name);
}

juce::Image AssetRegistry::getFrame(const juce::String& name)
{
    return loadAssetFromCategory("frames", name);
}

// ==============================================================================
// PREDEFINED ASSETS (Friendly names)
// ==============================================================================

juce::Image AssetRegistry::getTealPanel()
{
    return getPanel("Matte_Teal_Panel");
}

juce::Image AssetRegistry::getGraphiteRail()
{
    return getPanel("Graphite_Rail");
}

juce::Image AssetRegistry::getAluminumNameplate()
{
    return getPanel("Aluminum_Nameplate");
}

juce::Image AssetRegistry::getHardwareKnob()
{
    return getSprite("Hardware_Knob");
}

juce::Image AssetRegistry::getSynthKnob()
{
    return getSprite("Synth_Knob");
}

juce::Image AssetRegistry::getControlKnob()
{
    return getSprite("Control_Knob");
}

juce::Image AssetRegistry::getGraphiteButton()
{
    return getSprite("Graphite_Hardware_Button");
}

juce::Image AssetRegistry::getMatteButton()
{
    return getSprite("Matte_Graphite_Button");
}

juce::Image AssetRegistry::getToggleSwitch()
{
    return getSprite("Toggle_Switch");
}

juce::Image AssetRegistry::getGreenLED()
{
    return getSprite("Green_LED_Display");
}

juce::Image AssetRegistry::getRedLED()
{
    return getSprite("Red_LED_Indicator");
}

juce::Image AssetRegistry::getDarkGreenLEDSegment()
{
    return getSprite("Dark_Green_LED_Segment");
}

juce::Image AssetRegistry::getLimeLCD()
{
    return getSprite("Lime_Green_LCD");
}

juce::Image AssetRegistry::getLCDSegment()
{
    return getSprite("LCD_Segment");
}

juce::Image AssetRegistry::getVUMeter()
{
    return getSprite("VU_Meter");
}

// ==============================================================================
// ASSET UTILITIES
// ==============================================================================

void AssetRegistry::scanDirectory(const juce::File& dir, juce::StringArray& names) const
{
    if (!dir.exists())
        return;
    
    for (juce::DirectoryIterator iter(dir, false, "*.png", juce::File::findFiles); iter.next();)
    {
        names.add(iter.getFile().getFileNameWithoutExtension());
    }
}

juce::StringArray AssetRegistry::getPanelNames() const
{
    juce::StringArray names;
    scanDirectory(assetsRoot.getChildFile("panels"), names);
    return names;
}

juce::StringArray AssetRegistry::getSpriteNames() const
{
    juce::StringArray names;
    scanDirectory(assetsRoot.getChildFile("sprites"), names);
    return names;
}

juce::StringArray AssetRegistry::getFrameNames() const
{
    juce::StringArray names;
    scanDirectory(assetsRoot.getChildFile("frames"), names);
    return names;
}

juce::String AssetRegistry::getAssetPath(const juce::String& category, const juce::String& name) const
{
    return assetsRoot.getChildFile(category).getChildFile(name + ".png").getFullPathName();
}

int AssetRegistry::getAssetCount() const
{
    return getPanelNames().size() + getSpriteNames().size() + getFrameNames().size();
}

void AssetRegistry::logAvailableAssets() const
{
    DBG("=== ASSET REGISTRY CONTENTS ===");
    DBG("Assets root: " + assetsRoot.getFullPathName());
    
    auto panelNames = getPanelNames();
    DBG("Panels (" + juce::String(panelNames.size()) + "):");
    for (auto& name : panelNames)
        DBG("  " + name);
    
    auto spriteNames = getSpriteNames();
    DBG("Sprites (" + juce::String(spriteNames.size()) + "):");
    for (auto& name : spriteNames)
        DBG("  " + name);
    
    auto frameNames = getFrameNames();
    DBG("Frames (" + juce::String(frameNames.size()) + "):");
    for (auto& name : frameNames)
        DBG("  " + name);
    
    DBG("Total assets: " + juce::String(getAssetCount()));
    DBG("===============================");
}

bool AssetRegistry::validateAssets() const
{
    DBG("AssetRegistry: Validating expected assets...");
    
    bool allValid = true;
    for (int i = 0; i < numExpectedAssets; ++i)
    {
        auto& expected = expectedAssets[i];
        auto categoryDir = assetsRoot.getChildFile(expected.category);
        
        if (!categoryDir.exists())
        {
            if (expected.required)
            {
                DBG("AssetRegistry: MISSING required category: " + expected.category);
                allValid = false;
            }
            continue;
        }
        
        bool found = false;
        for (juce::DirectoryIterator iter(categoryDir, false, "*.png", juce::File::findFiles); iter.next();)
        {
            if (iter.getFile().getFileName().contains(expected.pattern))
            {
                found = true;
                break;
            }
        }
        
        if (!found && expected.required)
        {
            DBG("AssetRegistry: MISSING required asset matching: " + expected.pattern);
            allValid = false;
        }
    }
    
    DBG("AssetRegistry: Validation " + (allValid ? "PASSED" : "FAILED"));
    return allValid;
}

#endif // CrashToggles::ENABLE_TEXTURE_LOADING