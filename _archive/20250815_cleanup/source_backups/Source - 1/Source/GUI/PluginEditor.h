// Source/GUI/PluginEditor.h
#pragma once

#include <JuceHeader.h>
#include "Core/Commands.h"
#include "Core/ForgeProcessor.h"
#include "Core/PaintEngine.h"
#include "Core/ParameterBridge.h"
#include "../CanvasComponent.h"
#include "GUI/PaintControlPanel.h"

// Forward declarations
class ARTEFACTAudioProcessor;
class ArtefactLookAndFeel;
class HeaderBarComponent;
class ForgePanel;

class ARTEFACTAudioProcessorEditor : public juce::AudioProcessorEditor,
    public juce::Button::Listener,
    public juce::Timer,
    public juce::ComponentListener,
    public juce::AsyncUpdater
{
public:
    explicit ARTEFACTAudioProcessorEditor(ARTEFACTAudioProcessor&);
    ~ARTEFACTAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void buttonClicked(juce::Button*) override;
    void timerCallback() override;
    
    // ComponentListener overrides for visibility-based timer control
    void componentVisibilityChanged(juce::Component& component) override;
    void componentParentHierarchyChanged(juce::Component& component) override;
    
    // AsyncUpdater override (now NO-OP for passive editor)
    void handleAsyncUpdate() override;

private:
    ARTEFACTAudioProcessor& audioProcessor;
    
    // UI Components
    std::unique_ptr<ArtefactLookAndFeel> artefactLookAndFeel;
    std::unique_ptr<HeaderBarComponent> headerBar;
    std::unique_ptr<ForgePanel> forgePanel;
    std::unique_ptr<CanvasComponent> canvasComponent;
    std::unique_ptr<PaintControlPanel> paintControlPanel;
    juce::TextButton testButton {"Test"};
    
    // Parameter attachments (destroyed before child components)
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> buttonAttachments;
    
    // Thread safety with SafePointer pattern (JUCE 7.0.12 compatibility)
    juce::Component::SafePointer<ARTEFACTAudioProcessorEditor> safeThis{this};
    std::atomic<bool> shuttingDown{false};
    std::atomic<bool> initialised{false};
    
    #if 0  // DISABLED: Audio control flags removed for passive editor
    bool pendingAudioPause = false;
    bool pendingAudioResume = false;
    #endif
    
    // Deferred initialization methods (UI-only)
    void deferredInit();
    void setupEngineConnections();
    void setupComponentListener();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARTEFACTAudioProcessorEditor)
};