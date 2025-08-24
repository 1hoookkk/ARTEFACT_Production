#include "MorphFilterPanel.h"
#include "../Core/PluginProcessor.h"
#include "../LookAndFeelRetro.h"

MorphFilterPanel::MorphFilterPanel(ARTEFACTAudioProcessor& processor)
    : audioProcessor(processor)
{
    // Title label
    titleLabel = std::make_unique<juce::Label>("title", "MORPH FILTER");
    titleLabel->setFont(juce::Font(14.0f, juce::Font::bold));
    titleLabel->setColour(juce::Label::textColourId, juce::Colour(LookAndFeelRetro::RetroColors::lcdForeground));
    titleLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(*titleLabel);
    
    // Enable toggle
    enableToggle = std::make_unique<juce::ToggleButton>("Filter On");
    addAndMakeVisible(*enableToggle);
    
    // Create sliders and labels
    auto createSlider = [this](std::unique_ptr<juce::Slider>& slider, std::unique_ptr<juce::Label>& label, 
                              const juce::String& name, const juce::String& suffix = "") {
        slider = std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow);
        slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 16);
        slider->setColour(juce::Slider::textBoxTextColourId, juce::Colour(LookAndFeelRetro::RetroColors::lcdForeground));
        slider->setTextValueSuffix(suffix);
        addAndMakeVisible(*slider);
        
        label = std::make_unique<juce::Label>(name.toLowerCase(), name);
        label->setFont(juce::Font(10.0f));
        label->setColour(juce::Label::textColourId, juce::Colour(LookAndFeelRetro::RetroColors::lcdForeground));
        label->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(*label);
    };
    
    createSlider(morphSlider, morphLabel, "MORPH");
    createSlider(cutoffSlider, cutoffLabel, "CUTOFF", " Hz");
    createSlider(resonanceSlider, resonanceLabel, "RES");
    createSlider(characterSlider, characterLabel, "CHAR");
    createSlider(driveSlider, driveLabel, "DRIVE", " dB");
    
    // Pre/Post toggle
    prePostToggle = std::make_unique<juce::ToggleButton>("Pre/Post");
    addAndMakeVisible(*prePostToggle);
    
    // Set up parameter attachments - using available parameter IDs from PluginProcessor
    auto& apvts = audioProcessor.getParameters();
    
    // Map to existing filter and effects parameters
    morphAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "spectralMorph", *morphSlider);
    cutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "filterCutoff", *cutoffSlider);
    resonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "filterResonance", *resonanceSlider);
    characterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "harmonicContent", *characterSlider);
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "distortionAmount", *driveSlider);
    enableAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "paintActive", *enableToggle);
    prePostAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "quantizeToKey", *prePostToggle);
}

void MorphFilterPanel::paint(juce::Graphics& g)
{
    // Draw panel background using retro style
    if (auto* retroLAF = dynamic_cast<LookAndFeelRetro*>(&getLookAndFeel()))
    {
        retroLAF->drawPanel(g, getLocalBounds(), false);
    }
    else
    {
        g.setColour(juce::Colour(LookAndFeelRetro::RetroColors::faceplate));
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 4.0f);
    }
}

void MorphFilterPanel::resized()
{
    auto bounds = getLocalBounds();
    const int margin = 8;
    const int knobSize = 60;
    const int labelHeight = 16;
    const int toggleHeight = 24;
    
    bounds.reduce(margin, margin);
    
    // Title
    titleLabel->setBounds(bounds.removeFromTop(20));
    bounds.removeFromTop(4);
    
    // Enable toggle
    enableToggle->setBounds(bounds.removeFromTop(toggleHeight));
    bounds.removeFromTop(8);
    
    // Arrange knobs in 2x2 grid for main controls
    auto knobArea = bounds.removeFromTop(knobSize + labelHeight + 4);
    auto leftCol = knobArea.removeFromLeft(knobArea.getWidth() / 2);
    auto rightCol = knobArea;
    
    // First row
    auto morphArea = leftCol.removeFromTop(knobSize + labelHeight);
    morphLabel->setBounds(morphArea.removeFromTop(labelHeight));
    morphSlider->setBounds(morphArea.reduced(2));
    
    auto cutoffArea = rightCol.removeFromTop(knobSize + labelHeight);
    cutoffLabel->setBounds(cutoffArea.removeFromTop(labelHeight));
    cutoffSlider->setBounds(cutoffArea.reduced(2));
    
    bounds.removeFromTop(8);
    
    // Second row
    knobArea = bounds.removeFromTop(knobSize + labelHeight + 4);
    leftCol = knobArea.removeFromLeft(knobArea.getWidth() / 2);
    rightCol = knobArea;
    
    auto resArea = leftCol.removeFromTop(knobSize + labelHeight);
    resonanceLabel->setBounds(resArea.removeFromTop(labelHeight));
    resonanceSlider->setBounds(resArea.reduced(2));
    
    auto charArea = rightCol.removeFromTop(knobSize + labelHeight);
    characterLabel->setBounds(charArea.removeFromTop(labelHeight));
    characterSlider->setBounds(charArea.reduced(2));
    
    bounds.removeFromTop(8);
    
    // Drive knob (centered)
    auto driveArea = bounds.removeFromTop(knobSize + labelHeight + 4).withSizeKeepingCentre(knobSize + 4, knobSize + labelHeight);
    driveLabel->setBounds(driveArea.removeFromTop(labelHeight));
    driveSlider->setBounds(driveArea.reduced(2));
    
    bounds.removeFromTop(8);
    
    // Pre/Post toggle
    if (bounds.getHeight() >= toggleHeight)
        prePostToggle->setBounds(bounds.removeFromTop(toggleHeight));
}