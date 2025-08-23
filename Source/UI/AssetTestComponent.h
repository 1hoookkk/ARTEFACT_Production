#pragma once
#include <JuceHeader.h>
#include "AssetIntegration.h"
#include "MysteryButton.h"

namespace sc::ui {

// Simple test component to verify asset integration
class AssetTestComponent : public juce::Component
{
public:
    AssetTestComponent()
    {
        // Try to integrate upscaled assets on construction
        assetsLoaded = sc::assets::AssetIntegration::integrateUpscaledAssets();
        
        if (assetsLoaded) {
            juce::Logger::writeToLog("✓ Assets integrated successfully");
        } else {
            juce::Logger::writeToLog("✗ Some assets failed to load - using fallbacks");
        }
        
        // Add mystery button for testing
        mysteryButton = std::make_unique<MysteryButton>();
        addAndMakeVisible(*mysteryButton);
        
        setSize(500, 200);
    }
    
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds();
        
        // Background
        g.fillAll(sc::theme::bg01());
        
        // Title
        g.setColour(sc::theme::text());
        g.setFont(sc::theme::type().title);
        g.drawText("Asset Integration Test", bounds.removeFromTop(30), juce::Justification::centred);
        
        // Status
        g.setColour(assetsLoaded ? juce::Colours::green : juce::Colours::orange);
        g.setFont(sc::theme::type().label);
        g.drawText(assetsLoaded ? "✓ Real assets loaded" : "⚠ Using fallback assets", 
                   bounds.removeFromTop(20), juce::Justification::centred);
        
        // Test asset rendering
        auto testArea = bounds.reduced(20);
        testArea.removeFromRight(60);  // Leave space for mystery button
        sc::assets::AssetIntegration::testAssetRendering(g, testArea);
        
        // Labels
        g.setColour(sc::theme::subT());
        g.setFont(sc::theme::type().value);
        g.drawText("Knob", 10, getHeight() - 20, 80, 15, juce::Justification::centred);
        g.drawText("LED", 90, getHeight() - 20, 40, 15, juce::Justification::centred);  
        g.drawText("Switch", 130, getHeight() - 20, 60, 15, juce::Justification::centred);
        g.drawText("Panel", 190, getHeight() - 20, 100, 15, juce::Justification::centred);
        g.drawText("Mystery", getWidth() - 70, getHeight() - 20, 60, 15, juce::Justification::centred);
    }
    
    void resized() override
    {
        if (mysteryButton) {
            mysteryButton->setBounds(getWidth() - 58, 40, 48, 48);
        }
    }
    
private:
    bool assetsLoaded = false;
    std::unique_ptr<MysteryButton> mysteryButton;
};

} // namespace sc::ui