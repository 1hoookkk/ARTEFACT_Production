#include "TubePanel.h"
#include "../Core/PluginProcessor.h"
#include "../LookAndFeelRetro.h"

TubePanel::TubePanel(ARTEFACTAudioProcessor& processor)
    : audioProcessor(processor)
{
    // Title label
    titleLabel = std::make_unique<juce::Label>("title", "TUBE STAGE");
    titleLabel->setFont(juce::Font(14.0f, juce::Font::bold));
    titleLabel->setColour(juce::Label::textColourId, juce::Colour(LookAndFeelRetro::RetroColors::lcdForeground));
    titleLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(*titleLabel);
    
    // Enable toggle
    enableToggle = std::make_unique<juce::ToggleButton>("Tube On");
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
    
    createSlider(driveSlider, driveLabel, "DRIVE", " dB");
    createSlider(biasSlider, biasLabel, "BIAS");
    createSlider(toneSlider, toneLabel, "TONE");
    createSlider(outputSlider, outputLabel, "OUTPUT", " dB");
    
    // Oversampling combo box
    oversamplingCombo = std::make_unique<juce::ComboBox>();
    oversamplingCombo->addItem("1x", 1);
    oversamplingCombo->addItem("2x", 2);
    oversamplingCombo->addItem("4x", 3);
    oversamplingCombo->setSelectedId(1);
    addAndMakeVisible(*oversamplingCombo);
    
    oversamplingLabel = std::make_unique<juce::Label>("oversampling", "OS");
    oversamplingLabel->setFont(juce::Font(10.0f));
    oversamplingLabel->setColour(juce::Label::textColourId, juce::Colour(LookAndFeelRetro::RetroColors::lcdForeground));
    oversamplingLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(*oversamplingLabel);
    
    // Set up parameter attachments - using available parameter IDs from PluginProcessor
    auto& apvts = audioProcessor.getParameters();
    
    // Map to existing effects and processing parameters
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "distortionAmount", *driveSlider);
    biasAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "harmonicContent", *biasSlider);
    toneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "filterCutoff", *toneSlider);
    outputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "masterGain", *outputSlider);
    enableAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "adaptivePerformance", *enableToggle);
    oversamplingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "qualityMode", *oversamplingCombo);
}

void TubePanel::paint(juce::Graphics& g)
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

void TubePanel::resized()
{
    auto bounds = getLocalBounds();
    const int margin = 8;
    const int knobSize = 60;
    const int labelHeight = 16;
    const int toggleHeight = 24;
    const int comboHeight = 20;
    
    bounds.reduce(margin, margin);
    
    // Title
    titleLabel->setBounds(bounds.removeFromTop(20));
    bounds.removeFromTop(4);
    
    // Enable toggle
    enableToggle->setBounds(bounds.removeFromTop(toggleHeight));
    bounds.removeFromTop(8);
    
    // Drive knob (main control)
    auto driveArea = bounds.removeFromTop(knobSize + labelHeight + 4).withSizeKeepingCentre(knobSize + 4, knobSize + labelHeight);
    driveLabel->setBounds(driveArea.removeFromTop(labelHeight));
    driveSlider->setBounds(driveArea.reduced(2));
    
    bounds.removeFromTop(8);
    
    // Bias and Tone knobs in row
    auto knobRow = bounds.removeFromTop(knobSize + labelHeight + 4);
    auto leftKnob = knobRow.removeFromLeft(knobRow.getWidth() / 2);
    auto rightKnob = knobRow;
    
    auto biasArea = leftKnob.removeFromTop(knobSize + labelHeight);
    biasLabel->setBounds(biasArea.removeFromTop(labelHeight));
    biasSlider->setBounds(biasArea.reduced(2));
    
    auto toneArea = rightKnob.removeFromTop(knobSize + labelHeight);
    toneLabel->setBounds(toneArea.removeFromTop(labelHeight));
    toneSlider->setBounds(toneArea.reduced(2));
    
    bounds.removeFromTop(8);
    
    // Oversampling combo
    auto comboArea = bounds.removeFromTop(labelHeight + comboHeight + 4);
    oversamplingLabel->setBounds(comboArea.removeFromTop(labelHeight));
    oversamplingCombo->setBounds(comboArea.removeFromTop(comboHeight));
    
    bounds.removeFromTop(8);
    
    // Output knob
    if (bounds.getHeight() >= knobSize + labelHeight)
    {
        auto outputArea = bounds.removeFromTop(knobSize + labelHeight + 4).withSizeKeepingCentre(knobSize + 4, knobSize + labelHeight);
        outputLabel->setBounds(outputArea.removeFromTop(labelHeight));
        outputSlider->setBounds(outputArea.reduced(2));
    }
}