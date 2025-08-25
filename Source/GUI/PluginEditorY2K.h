#pragma once
#include <JuceHeader.h>
#include "ThemeAwareLookAndFeelStub.h"
#include "PixelCanvasComponent.h"
#include "WaveformThumbnailComponent.h"
#include "PianoRollComponent.h"
#include "LookAndFeelTokens.h"

// Forward declarations
class ARTEFACTAudioProcessor;
class SpectralSynthEngineStub;

/**
 * @brief Y2K-themed plugin editor for SpectralCanvas Pro
 * 
 * This editor implements the single polished Y2K theme with:
 * - Chrome beveled panels and controls
 * - 128x128 pixel canvas with CRT effects
 * - RT-safe paint-to-audio pipeline
 * - Accessibility and safety features
 * 
 * The editor serves as a complete replacement for complex feature-flag
 * based editors, focusing on a single polished aesthetic.
 */
class PluginEditorY2K : public juce::AudioProcessorEditor,
                        public juce::KeyListener,
                        public juce::Timer,
                        public juce::AudioProcessorValueTreeState::Listener
{
public:
    explicit PluginEditorY2K(ARTEFACTAudioProcessor& processor);
    ~PluginEditorY2K() override;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;

    // Key handling for panic hotkey and accessibility
    bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) override;

    // Accessibility and safety
    void panicDisableAllEffects();
    void setReduceMotion(bool reduce);
    
    // Parameter listener
    void parameterChanged(const juce::String& parameterID, float newValue) override;

private:
    // Core references
    ARTEFACTAudioProcessor& audioProcessor_;
    
    // Theme system
    std::unique_ptr<ThemeAwareLookAndFeel> themeLookAndFeel_;
    scp::ThemeTokens currentTokens_;
    
    // Main UI components (Phase 2)
    std::unique_ptr<PixelCanvasComponent> pixelCanvas_;
    std::unique_ptr<WaveformThumbnailComponent> waveformThumbnail_;
    std::unique_ptr<PianoRollComponent> pianoRoll_;
    std::unique_ptr<juce::TextButton> loadButton_;
    
    // Control panels (simplified for Y2K theme)
    std::unique_ptr<juce::Component> leftControlPanel_;
    std::unique_ptr<juce::Component> rightControlPanel_;
    std::unique_ptr<juce::Component> bottomPanel_;
    
    // Basic controls for demonstration
    std::unique_ptr<juce::Slider> gainSlider_;
    std::unique_ptr<juce::Slider> decaySlider_;
    std::unique_ptr<juce::Slider> freqMinSlider_;
    std::unique_ptr<juce::Slider> freqMaxSlider_;
    std::unique_ptr<juce::ToggleButton> testToneButton_;
    std::unique_ptr<juce::ToggleButton> bypassSecretSauceButton_;
    std::unique_ptr<juce::TextButton> clearButton_;
    std::unique_ptr<juce::ToggleButton> previewButton_;
    
    // Labels for sliders
    std::unique_ptr<juce::Label> gainLabel_;
    std::unique_ptr<juce::Label> decayLabel_;
    std::unique_ptr<juce::Label> freqMinLabel_;
    std::unique_ptr<juce::Label> freqMaxLabel_;
    
    #if defined(ENABLE_DEBUG_BUTTON)
    std::unique_ptr<juce::TextButton> debugButton_;
    #endif
    
    // Parameter attachments (using correct JUCE types)
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::vector<std::unique_ptr<SliderAttachment>> sliderAttachments_;
    std::vector<std::unique_ptr<ButtonAttachment>> buttonAttachments_;
    
    // State management
    std::atomic<bool> reduceMotion_{false};
    std::atomic<bool> effectsDisabled_{false};
    
    // Layout constants
    static constexpr int kMinWidth = 800;
    static constexpr int kMinHeight = 600;
    static constexpr int kDefaultWidth = 1000;
    static constexpr int kDefaultHeight = 700;
    static constexpr int kLeftPanelWidth = 200;
    static constexpr int kRightPanelWidth = 200;
    static constexpr int kBottomPanelHeight = 80;
    static constexpr int kPanelPadding = 12;
    
    // Internal methods
    void createControls();
    void setupParameterAttachments();
    void layoutComponents();
    void updateThemeColors();
    
    // Panel creation methods
    void createLeftControlPanel();
    void createRightControlPanel(); 
    void createBottomPanel();
    
    // Control creation helpers
    std::unique_ptr<juce::Slider> createSlider(const juce::String& name, 
                                               juce::Slider::SliderStyle style = juce::Slider::RotaryHorizontalVerticalDrag);
    std::unique_ptr<juce::ToggleButton> createToggleButton(const juce::String& name);
    std::unique_ptr<juce::TextButton> createTextButton(const juce::String& name);
    std::unique_ptr<juce::Label> createLabel(const juce::String& text);
    
    // Timer callback for periodic updates
    void timerCallback() override;
    
    // Safety and accessibility methods
    void checkAccessibilitySettings();
    bool isHighContrastMode() const;
    bool isReducedMotionRequested() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditorY2K)
};