#pragma once
#include <JuceHeader.h>
#include "AssetManager.h"

class ARTEFACTAudioProcessor;  // Forward declaration

namespace sc::ui {

// Mystery button component for filter/tube amp secret sauce
class MysteryButton : public juce::Button
{
public:
    MysteryButton() : juce::Button("Mystery")
    {
        setClickingTogglesState(true);
        setToggleState(true, juce::dontSendNotification);  // Default: ON as requested
        setSize(48, 48);  // Square button for better layout
    }
    
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto& assetMgr = sc::assets::AssetManager::getInstance();
        
        // Get the appropriate state image
        auto buttonImage = getToggleState() ? 
            assetMgr.getAsset("mystery_button_on") : 
            assetMgr.getAsset("mystery_button_off");
            
        if (buttonImage.isValid()) {
            // Render the button sprite
            auto bounds = getLocalBounds();
            g.drawImageWithin(buttonImage, 
                bounds.getX(), bounds.getY(), 
                bounds.getWidth(), bounds.getHeight(),
                juce::RectanglePlacement::centred);
        } else {
            // Fallback rendering if no asset
            auto bounds = getLocalBounds().toFloat().reduced(2);
            
            // Button body
            g.setColour(getToggleState() ? 
                juce::Colour(0xff00ff41).withAlpha(0.8f) :  // Green when on
                juce::Colour(0xff2a3140));                   // Dark when off
            g.fillRoundedRectangle(bounds, 6.0f);
            
            // Button border
            g.setColour(juce::Colour(0xff44c6d2).withAlpha(0.5f));
            g.drawRoundedRectangle(bounds, 6.0f, 1.5f);
        }
        
        // Interaction feedback
        if (shouldDrawButtonAsHighlighted || shouldDrawButtonAsDown) {
            g.setColour(juce::Colours::white.withAlpha(shouldDrawButtonAsDown ? 0.3f : 0.1f));
            g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(2), 6.0f);
        }
    }
    
    // Wire to processor parameter
    void attachToProcessor(ARTEFACTAudioProcessor& processor)
    {
        onClick = [&processor, this]() {
            // This could control filter/tube amp parameter
            // For now, just log the state change
            juce::Logger::writeToLog("Mystery button state: " + 
                juce::String(getToggleState() ? "ON" : "OFF"));
                
            // TODO: Wire to actual filter/tube amp parameter
            // processor.setSecretSauceEnabled(getToggleState());
        };
    }
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MysteryButton)
};

} // namespace sc::ui