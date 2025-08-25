#include <JuceHeader.h>
#include "Source/GUI/Colors.h"
#include "Source/GUI/MusicalGridOverlay.h"
#include "Source/GUI/EnhancedPianoRollComponent.h"
#include "Source/GUI/ChordRecognitionOverlay.h"
#include "Source/GUI/EnhancedExportPod.h"
#include "Source/GUI/ScaleSelectorComponent.h"

int main()
{
    // Test that all Phase 2 UI components can be instantiated
    juce::ScopedJuceInitialiser_GUI juce_gui;
    
    // Test color definitions
    auto bgColor = UIColour::Background;
    auto percColor = UIColour::PercColor;
    auto harmColor = UIColour::HarmColor;
    
    // Test component instantiation
    MusicalGridOverlay gridOverlay;
    EnhancedPianoRollComponent pianoRoll;
    ChordRecognitionOverlay chordRecognition;
    EnhancedExportPod exportPod;
    ScaleSelectorComponent scaleSelector;
    
    juce::Logger::writeToLog("Phase 2 UI components compiled successfully!");
    return 0;
}
