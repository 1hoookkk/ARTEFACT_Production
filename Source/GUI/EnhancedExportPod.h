#pragma once
#include <JuceHeader.h>
#include "Colors.h"
#include "WaveformThumbnailComponent.h"

/**
 * @brief Enhanced export pod for Phase 2 UI
 * 
 * Real-time waveform preview with improved visual feedback and drag-to-DAW functionality.
 * Implements the "Minimal but Musical" design philosophy.
 */
class EnhancedExportPod : public juce::Component, public juce::DragAndDropContainer
{
public:
    EnhancedExportPod();
    ~EnhancedExportPod() override;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;

    // Export functionality
    void setReadyState(bool ready);
    void setRenderingState(bool rendering);
    void setAudioLevel(float levelDb);
    void setCurrentFileName(const juce::String& filename);
    
    // Waveform preview
    void updateWaveformPreview(const juce::AudioBuffer<float>& buffer);
    void setWaveformData(const std::vector<float>& samples);
    
    // Callbacks
    std::function<void()> onCaptureRequested;
    std::function<void(const juce::String&)> onFileDropped;

private:
    // Visual state
    bool isReady = false;
    bool isRendering = false;
    float audioLevelDb = -60.0f;
    juce::String currentFileName = "untitled.wav";
    
    // Waveform data
    std::vector<float> waveformSamples;
    juce::AudioBuffer<float> audioBuffer;
    
    // Child components
    std::unique_ptr<WaveformThumbnailComponent> waveformComponent;
    
    // Drag and drop
    bool isDragging = false;
    juce::Point<int> dragStartPosition;
    
    // Helper methods
    void drawCaptureButton(juce::Graphics& g);
    void drawStatusIndicator(juce::Graphics& g);
    void drawWaveformPreview(juce::Graphics& g);
    void drawDragHandle(juce::Graphics& g);
    juce::Rectangle<int> getCaptureButtonBounds() const;
    juce::Rectangle<int> getStatusBounds() const;
    juce::Rectangle<int> getWaveformBounds() const;
    juce::Rectangle<int> getDragHandleBounds() const;
    
    // Colors and styling
    juce::Colour getCaptureButtonColor() const;
    juce::Colour getStatusColor() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnhancedExportPod)
};
