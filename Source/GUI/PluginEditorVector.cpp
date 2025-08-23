#include "PluginEditorVector.h"
#include "../Core/PluginProcessor.h"
#include "../Core/PaintQueue.h"

PluginEditorVector::PluginEditorVector(ARTEFACTAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    // Create the main panel
    panel = std::make_unique<ui::Panel>();
    addAndMakeVisible(panel.get());
    
    // Set up callbacks
    panel->onCanvasTouch = [this](juce::Point<float> point, bool isDown) {
        handleCanvasTouch(point, isDown);
    };
    
    panel->onRealmChanged = [this](int realm) {
        handleRealmChange(realm);
    };
    
    panel->onActionButton = [this](int button) {
        handleActionButton(button);
    };
    
    // Create invisible sliders for parameter attachments
    for (int i = 0; i < 6; ++i) {
        knobSliders[i] = std::make_unique<juce::Slider>();
        knobSliders[i]->setVisible(false);
        addAndMakeVisible(knobSliders[i].get());
    }
    
    // Set up initial parameter attachments
    updateKnobAttachments();
    
    // Listen to realm parameter changes
    processorRef.getAPVTS().addParameterListener("realm", this);
    
    // Set default size (1:1 aspect ratio)
    setSize(900, 900);
    setResizable(true, true);
    
    // Start timer for UI updates (30Hz is sufficient for smooth LED/animation)
    startTimerHz(30);
}

PluginEditorVector::~PluginEditorVector() {
    processorRef.getAPVTS().removeParameterListener("realm", this);
}

void PluginEditorVector::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black);
}

void PluginEditorVector::resized() {
    // Keep content square, letterbox if needed
    auto bounds = getLocalBounds();
    int size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    auto squareBounds = juce::Rectangle<int>(0, 0, size, size).withCentre(bounds.getCentre());
    
    panel->setBounds(squareBounds);
}

void PluginEditorVector::parameterChanged(const juce::String& parameterID, float newValue) {
    if (parameterID == "realm") {
        int newRealm = (int)newValue;
        if (newRealm != currentRealm) {
            currentRealm = newRealm;
            panel->setActiveRealm(currentRealm);
            updateKnobAttachments();
        }
    }
}

void PluginEditorVector::timerCallback() {
    // Update LED bar with current audio level (placeholder)
    // In real implementation, get this from processor's level meter
    float level = 0.5f; // TODO: Get actual level from processor
    panel->setLEDLevel(level);
    
    // Update knob values to reflect current parameters
    updateKnobValues();
}

void PluginEditorVector::handleCanvasTouch(juce::Point<float> normalizedPoint, bool isDown) {
    // Convert normalized coordinates to paint gesture and push to queue
    // This integrates with existing PaintQueue system
    
    if (auto* queue = processorRef.getPaintQueue()) {
        PaintEvent event;
        event.nx = normalizedPoint.x;
        event.ny = normalizedPoint.y;
        event.pressure = 1.0f; // TODO: Get actual pressure from touch/tablet
        event.flags = isDown ? kStrokeStart : kStrokeMove; // Use proper stroke flags
        event.color = 0xFFFFFFFF; // White color for now
        
        queue->push(event);
    }
}

void PluginEditorVector::handleRealmChange(int newRealm) {
    // Update the realm parameter in APVTS
    if (auto* param = processorRef.getAPVTS().getParameter("realm")) {
        param->setValueNotifyingHost(param->convertTo0to1((float)newRealm));
    }
}

void PluginEditorVector::handleActionButton(int buttonIndex) {
    // Handle action button presses
    switch (buttonIndex) {
        case 0: // FREEZE
            // TODO: Implement freeze functionality
            break;
        case 1: // MORPH  
            // TODO: Implement morph functionality
            break;
        case 2: // RANDOM
            // TODO: Implement randomize functionality
            break;
        case 3: // STORE
            // TODO: Implement preset store functionality
            break;
    }
}

void PluginEditorVector::updateKnobAttachments() {
    // Clear existing attachments
    for (auto& attachment : knobAttachments) {
        attachment.reset();
    }
    
    // Get parameter IDs for current realm
    auto paramIDs = getCurrentRealmKnobIDs();
    
    // Create new attachments
    for (int i = 0; i < 6; ++i) {
        if (paramIDs[i].isNotEmpty()) {
            knobAttachments[i] = std::make_unique<SliderAttachment>(
                processorRef.getAPVTS(), paramIDs[i], *knobSliders[i]);
        }
    }
}

void PluginEditorVector::updateKnobValues() {
    // This is where we'd update the visual knobs based on slider values
    // For now, this is a placeholder - the Panel draws knobs at fixed 0.5 value
    // TODO: Pass actual knob values to panel for drawing
}

std::array<juce::String, 6> PluginEditorVector::getCurrentRealmKnobIDs() const {
    switch (currentRealm) {
        case 0: // Blue realm
            return {
                "blue_brushSize", "blue_pressure", "blue_decay", 
                "blue_snap", "blue_harmDepth", "blue_panColor"
            };
        case 1: // Purple realm  
            return {
                "purple_cutoff", "purple_resonance", "purple_drive",
                "purple_lpBpMix", "purple_keyTrack", "purple_env"
            };
        case 2: // Yellow realm
            return {
                "yellow_drive", "yellow_bias", "yellow_color",
                "yellow_mix", "yellow_oversample", "yellow_comp"
            };
        default:
            return {};
    }
}

juce::String PluginEditorVector::getKnobParameterID(int knobIndex) const {
    auto paramIDs = getCurrentRealmKnobIDs();
    return (knobIndex >= 0 && knobIndex < 6) ? paramIDs[knobIndex] : juce::String();
}