#pragma once
#include <JuceHeader.h>
#include <unordered_map>
#include "AssetMap.h"

namespace sc::assets {

class AssetManager
{
public:
    static AssetManager& getInstance()
    {
        static AssetManager instance;
        return instance;
    }

    // Load asset from BinaryData and cache it
    juce::Image getAsset(const std::string& name)
    {
        auto it = imageCache.find(name);
        if (it != imageCache.end()) {
            return it->second;
        }

        // Try to load from BinaryData (will be populated when assets are added)
        juce::Image img;
        
        // For now, return empty image - assets will be added to BinaryData later
        if (img.isValid()) {
            imageCache[name] = img;
        }
        
        return img;
    }
    
    // Add processed asset directly to cache (for integration testing)
    void addProcessedAsset(const std::string& name, const juce::Image& asset)
    {
        if (asset.isValid()) {
            imageCache[name] = asset;
            juce::Logger::writeToLog("Added processed asset: " + juce::String(name) + 
                " (" + juce::String(asset.getWidth()) + "x" + juce::String(asset.getHeight()) + ")");
        }
    }

    // Create fallback textures for testing (vector-based)
    juce::Image createFallbackKnobFace(int size = 64)
    {
        juce::Image img(juce::Image::ARGB, size, size, true);
        juce::Graphics g(img);
        
        auto bounds = juce::Rectangle<float>(0, 0, (float)size, (float)size);
        auto radius = size * 0.45f;
        auto center = bounds.getCentre();
        
        // Dark metal base
        g.setColour(juce::Colour(0xff2a3140));
        g.fillEllipse(center.x - radius, center.y - radius, radius * 2, radius * 2);
        
        // Brushed metal effect (simplified)
        g.setColour(juce::Colour(0xff383f4d));
        for (int i = 0; i < 8; ++i) {
            auto angle = i * juce::MathConstants<float>::pi / 4.0f;
            auto startX = center.x + std::cos(angle) * radius * 0.3f;
            auto startY = center.y + std::sin(angle) * radius * 0.3f;
            auto endX = center.x + std::cos(angle) * radius * 0.9f;
            auto endY = center.y + std::sin(angle) * radius * 0.9f;
            g.drawLine(startX, startY, endX, endY, 1.0f);
        }
        
        // Center dot
        g.setColour(juce::Colour(0xff44c6d2));
        g.fillEllipse(center.x - 2, center.y - 2, 4, 4);
        
        return img;
    }

    juce::Image createFallbackLEDBloom(int size = 32)
    {
        juce::Image img(juce::Image::ARGB, size, size, true);
        juce::Graphics g(img);
        
        auto center = juce::Point<float>(size * 0.5f, size * 0.5f);
        auto radius = size * 0.4f;
        
        // Create radial gradient for bloom
        juce::ColourGradient gradient(
            juce::Colour(0xffffb366),  // Amber center
            center.x, center.y,
            juce::Colour(0x00ffb366),  // Transparent edge
            center.x + radius, center.y,
            true
        );
        
        g.setGradientFill(gradient);
        g.fillEllipse(center.x - radius, center.y - radius, radius * 2, radius * 2);
        
        return img;
    }

    juce::Image createFallbackPanelTexture(int size = 256)
    {
        juce::Image img(juce::Image::RGB, size, size, false);
        juce::Graphics g(img);
        
        // Dark teal base
        g.fillAll(juce::Colour(0xff1d2a32));
        
        // Add subtle noise texture
        juce::Random rand(42); // Seed for consistent texture
        for (int y = 0; y < size; y += 2) {
            for (int x = 0; x < size; x += 2) {
                auto brightness = rand.nextFloat() * 0.02f;
                auto color = juce::Colour(0xff1d2a32).brighter(brightness);
                g.setColour(color);
                g.fillRect(x, y, 2, 2);
            }
        }
        
        return img;
    }

    juce::Image createFallbackSwitch()
    {
        juce::Image img(juce::Image::ARGB, AssetSpecs::Switch::TOGGLE_W, AssetSpecs::Switch::TOGGLE_H, true);
        juce::Graphics g(img);
        
        auto bounds = img.getBounds().toFloat();
        
        // Dark graphite housing
        g.setColour(juce::Colour(0xff2a3140));
        g.fillRoundedRectangle(bounds.reduced(2), 4.0f);
        
        // Metal toggle in center
        auto toggleW = 8.0f;
        auto toggleH = bounds.getHeight() - 6;
        auto toggleX = bounds.getCentreX() - toggleW * 0.5f;
        auto toggleY = 3.0f;
        
        g.setColour(juce::Colour(0xff888888));
        g.fillRoundedRectangle(toggleX, toggleY, toggleW, toggleH, 2.0f);
        
        return img;
    }

    // SYSTEMATIC ASSET ACCESS - prevents chaos from last time
    juce::Image getKnobFace() {
        auto asset = getAsset(AssetSpecs::Knob::FACE_KEY);
        if (asset.isValid() && AssetValidator::validateKnobFace(asset)) {
            return asset;
        }
        return createFallbackKnobFace(AssetSpecs::Knob::FACE_SIZE);
    }

    juce::Image getLEDBloom() {
        auto asset = getAsset(AssetSpecs::LED::BLOOM_KEY);
        if (asset.isValid() && AssetValidator::validateLEDBloom(asset)) {
            return asset;
        }
        return createFallbackLEDBloom(AssetSpecs::LED::BLOOM_SIZE);
    }

    juce::Image getPanelTexture() {
        auto asset = getAsset(AssetSpecs::Panel::MAIN_KEY);
        if (asset.isValid() && AssetValidator::validatePanelTexture(asset)) {
            return asset;
        }
        return createFallbackPanelTexture(AssetSpecs::Panel::TILE_SIZE);
    }

    juce::Image getMysterySwitch() {
        auto asset = getAsset(AssetSpecs::Switch::MYSTERY_KEY);
        if (asset.isValid() && AssetValidator::validateSwitch(asset)) {
            return asset;
        }
        return createFallbackSwitch();
    }

    // Render asset with proper placement strategy
    void renderAsset(juce::Graphics& g, const AssetPlacement::RenderStrategy& strategy, const juce::Image& asset) {
        if (!asset.isValid()) return;

        switch (strategy.component) {
            case AssetPlacement::Component::ROTARY_KNOB:
                g.drawImageWithin(asset, 
                    strategy.bounds.getX(), strategy.bounds.getY(),
                    strategy.bounds.getWidth(), strategy.bounds.getHeight(),
                    strategy.placement);
                break;

            case AssetPlacement::Component::PANEL_BACKGROUND:
                // Tile the texture
                for (int y = strategy.bounds.getY(); y < strategy.bounds.getBottom(); y += asset.getHeight()) {
                    for (int x = strategy.bounds.getX(); x < strategy.bounds.getRight(); x += asset.getWidth()) {
                        g.drawImageAt(asset, x, y);
                    }
                }
                break;

            case AssetPlacement::Component::LED_INDICATOR:
            case AssetPlacement::Component::TOGGLE_SWITCH:
                g.drawImageWithin(asset,
                    strategy.bounds.getX(), strategy.bounds.getY(),
                    strategy.bounds.getWidth(), strategy.bounds.getHeight(),
                    strategy.placement);
                break;
        }
    }

private:
    std::unordered_map<std::string, juce::Image> imageCache;
    
    AssetManager() = default;
    ~AssetManager() = default;
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;
};

} // namespace sc::assets