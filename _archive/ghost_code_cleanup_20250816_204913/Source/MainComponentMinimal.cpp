/**
 * Absolutely Minimal MainComponent implementation
 * Contains only JUCE basics, no custom code
 */

#include "MainComponentMinimal.h"

MainComponentMinimal::MainComponentMinimal()
{
    // Only basic JUCE initialization
    statusLabel.setText("Minimal Test - JUCE Working", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(statusLabel);
    
    setSize(400, 300);
}

void MainComponentMinimal::paint(juce::Graphics& g)
{
    // Basic paint - just fill background
    g.fillAll(juce::Colours::darkgrey);
    
    // Simple text
    g.setColour(juce::Colours::white);
    g.setFont(16.0f);
    g.drawText("Minimal Component Test", 
               getLocalBounds().removeFromTop(50),
               juce::Justification::centred, true);
}

void MainComponentMinimal::resized()
{
    // Basic layout
    auto bounds = getLocalBounds();
    bounds.removeFromTop(50); // Skip title area
    statusLabel.setBounds(bounds.reduced(20));
}