#pragma once
/**
 * Absolutely Minimal MainComponent for emergency testing
 * Contains only JUCE basics, no custom code
 */

#include <JuceHeader.h>

class MainComponentMinimal : public juce::Component
{
public:
    MainComponentMinimal();
    ~MainComponentMinimal() override = default;
    
    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    juce::Label statusLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponentMinimal)
};