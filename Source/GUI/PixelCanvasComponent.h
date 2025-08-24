#pragma once
#include <JuceHeader.h>
#include "../Core/PaintQueue.h"
#include "LookAndFeelTokens.h"
#include "ThemeAwareLookAndFeel.h"

/**
 * @brief 128x128 pixel canvas with Y2K CRT aesthetic and RT-safe paint-to-audio pipeline
 * 
 * This component implements the core visual interface for SpectralCanvas Pro:
 * - Vector-based 128x128 pixel grid (scales crisply at any DPI)
 * - Real-time paint gestures with tactile feedback
 * - CRT effects: scanlines, phosphor bloom, subtle chromatic fringing
 * - RT-safe communication to audio thread via PaintQueue
 * - Y2K aesthetic with chrome bezel and vintage character
 */
class PixelCanvasComponent : public juce::Component,
                             public juce::MouseListener,
                             private juce::Timer
{
public:
    PixelCanvasComponent();
    ~PixelCanvasComponent() override;

    // Paint queue connection for RT-safe UI→Audio communication
    void setPaintQueue(SpectralPaintQueue* paintQueue) noexcept;
    
    // Canvas manipulation
    void clearCanvas();
    void loadPixelData(const std::vector<uint8_t>& pixelData);
    std::vector<uint8_t> getPixelData() const;
    
    // Visual state control
    void setReduceMotion(bool reduce) noexcept { reduceMotion_ = reduce; }
    void panicDisableVisuals(); // Immediate safe mode - stops all animations
    
    // CRT effects control
    void setScanlinesEnabled(bool enabled) { scanlinesEnabled_ = enabled; repaint(); }
    void setBloomEnabled(bool enabled) { bloomEnabled_ = enabled; repaint(); }
    void setBloomIntensity(float intensity) { bloomIntensity_ = jlimit(0.0f, 1.0f, intensity); }
    
    // Painting parameters
    void setBrushSize(int size) { brushSize_ = jlimit(1, 8, size); }
    void setBrushIntensity(float intensity) { brushIntensity_ = jlimit(0.0f, 1.0f, intensity); }
    
    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;

private:
    // Canvas data
    static constexpr int kCanvasWidth = 128;
    static constexpr int kCanvasHeight = 128;
    static constexpr int kCanvasSize = kCanvasWidth * kCanvasHeight;
    
    std::array<uint8_t, kCanvasSize> pixelBuffer_{};
    
    // Visual layout caching
    float pixelSize_ = 4.0f;
    float canvasOffsetX_ = 0.0f;
    float canvasOffsetY_ = 0.0f;
    juce::Rectangle<float> canvasBounds_;
    
    // Paint state
    bool isPainting_ = false;
    int lastPaintX_ = -1;
    int lastPaintY_ = -1;
    int brushSize_ = 2;
    float brushIntensity_ = 1.0f;
    
    // Visual effects state
    bool reduceMotion_ = false;
    bool scanlinesEnabled_ = true;
    bool bloomEnabled_ = true;
    float bloomIntensity_ = 0.0f;    // Current bloom level (decays over time)
    float targetBloomIntensity_ = 0.6f; // Target when painting
    
    // Tactile feedback (Y2K character)
    float jitterOffsetX_ = 0.0f;
    float jitterOffsetY_ = 0.0f;
    int jitterRemainingMs_ = 0;
    
    // Paint queue for RT-safe audio communication
    SpectralPaintQueue* paintQueue_ = nullptr;
    
    // Theme tokens (cached for performance)
    scp::ThemeTokens tokens_;
    
    // Internal methods
    void updateLayout();
    void localToCanvas(const juce::Point<int>& localPoint, juce::Point<int>& canvasPoint) const;
    void canvasToLocal(const juce::Point<int>& canvasPoint, juce::Point<float>& localPoint) const;
    
    void applyBrushStroke(int centerX, int centerY, int radius, uint8_t intensity);
    void interpolateBrush(int fromX, int fromY, int toX, int toY, int radius, uint8_t intensity);
    
    void pushPaintGestureToQueue(float normalizedX, float normalizedY, float pressure, uint32_t flags);
    void triggerTactileFeedback();
    
    // Rendering methods
    void drawCanvasPixels(juce::Graphics& g);
    void drawChromeBezel(juce::Graphics& g);
    void drawCRTEffects(juce::Graphics& g);
    void drawHUDOverlay(juce::Graphics& g);
    
    // Timer callback for animations
    void timerCallback() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PixelCanvasComponent)
};