#pragma once

#include <JuceHeader.h>
#include "Telemetry/HudMetrics.h"

/**
 * Real-time HUD overlay component for displaying audio engine metrics.
 * 
 * Features:
 * - Non-interactive overlay (mouse events pass through)
 * - Timer-based updates at 30Hz for smooth refresh
 * - Lock-free queue polling for RT-safe metric collection
 * - Clean monospace text rendering in top-left corner
 * - Graceful handling of empty queue states
 * - Show/hide visibility control
 * 
 * Usage:
 *   HudOverlay hud(hudQueue);
 *   addAndMakeVisible(hud);
 *   hud.setBounds(getLocalBounds());
 *   hud.setVisible(true);
 */
class HudOverlay : public juce::Component, public juce::Timer
{
public:
    /**
     * Constructor
     * @param queue Reference to the HUD metrics queue for data polling
     */
    explicit HudOverlay(SpectralCanvas::HudQueue& queue);
    
    /**
     * Destructor - ensures timer is stopped
     */
    ~HudOverlay() override;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    /**
     * Show the HUD overlay and start timer updates
     */
    void showHud();
    
    /**
     * Hide the HUD overlay and stop timer updates
     */
    void hideHud();
    
    /**
     * Toggle HUD visibility
     */
    void toggleHud();
    
    /**
     * Check if HUD is currently visible
     */
    bool isHudVisible() const { return isVisible(); }

private:
    // Timer callback for metric updates
    void timerCallback() override;
    
    // Update cached metrics from queue
    void updateMetrics();
    
    // Format metrics for display
    juce::String formatMetrics() const;
    
    // Calculate text bounds for proper positioning
    juce::Rectangle<float> calculateTextBounds() const;

    // Data members
    SpectralCanvas::HudQueue& hudQueue;
    
    // Cached metrics (updated from queue)
    struct CachedMetrics
    {
        float peakLevel = 0.0f;
        float rmsLevel = 0.0f;
        int eventsProcessed = 0;
        int queueDepth = 0;
        int maxQueueDepth = 0;
        bool hasData = false;
    } cachedMetrics;
    
    // Display settings
    static constexpr int TIMER_INTERVAL_MS = 33; // ~30Hz
    static constexpr int MARGIN = 10;
    static constexpr float FONT_SIZE = 12.0f;
    
    // Fonts and colors
    juce::Font monospaceFont;
    juce::Colour textColour;
    juce::Colour backgroundColour;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HudOverlay)
};

/**
 * Implementation
 */

inline HudOverlay::HudOverlay(SpectralCanvas::HudQueue& queue)
    : hudQueue(queue)
    , monospaceFont(juce::Font::getDefaultMonospacedFontName(), FONT_SIZE, juce::Font::plain)
    , textColour(juce::Colours::lightgreen)
    , backgroundColour(juce::Colours::black.withAlpha(0.7f))
{
    // Make overlay non-interactive - mouse events pass through
    setInterceptsMouseClicks(false, false);
    
    // Initially hidden
    setVisible(false);
}

inline HudOverlay::~HudOverlay()
{
    stopTimer();
}

inline void HudOverlay::paint(juce::Graphics& g)
{
    if (!cachedMetrics.hasData)
        return;
    
    // Get text bounds
    auto textBounds = calculateTextBounds();
    
    // Draw semi-transparent background
    g.setColour(backgroundColour);
    g.fillRoundedRectangle(textBounds.expanded(5.0f), 3.0f);
    
    // Draw border
    g.setColour(textColour.withAlpha(0.5f));
    g.drawRoundedRectangle(textBounds.expanded(5.0f), 3.0f, 1.0f);
    
    // Draw metrics text
    g.setColour(textColour);
    g.setFont(monospaceFont);
    
    auto metricsText = formatMetrics();
    g.drawText(metricsText, textBounds, juce::Justification::topLeft, false);
}

inline void HudOverlay::resized()
{
    // HUD positioning is handled in paint() based on text bounds
    // No specific layout needed here
}

inline void HudOverlay::showHud()
{
    setVisible(true);
    startTimer(TIMER_INTERVAL_MS);
}

inline void HudOverlay::hideHud()
{
    setVisible(false);
    stopTimer();
}

inline void HudOverlay::toggleHud()
{
    if (isVisible())
        hideHud();
    else
        showHud();
}

inline void HudOverlay::timerCallback()
{
    updateMetrics();
    repaint();
}

inline void HudOverlay::updateMetrics()
{
    // Try to read latest metrics from queue
    SpectralCanvas::HudMetrics latestMetrics;
    bool hasNewData = false;
    
    // Drain queue to get most recent data
    while (hudQueue.pop(latestMetrics))
    {
        hasNewData = true;
    }
    
    if (hasNewData)
    {
        cachedMetrics.peakLevel = juce::jmax(latestMetrics.peakL, latestMetrics.peakR);
        cachedMetrics.rmsLevel = juce::jmax(latestMetrics.rmsL, latestMetrics.rmsR);
        cachedMetrics.eventsProcessed = static_cast<int>(latestMetrics.evPushed);
        cachedMetrics.queueDepth = static_cast<int>(latestMetrics.evPopped);
        cachedMetrics.maxQueueDepth = static_cast<int>(latestMetrics.maxQDepth);
        cachedMetrics.hasData = true;
    }
}

inline juce::String HudOverlay::formatMetrics() const
{
    if (!cachedMetrics.hasData)
        return "HUD: No Data";
    
    juce::String result;
    result << "SPECTRAL CANVAS HUD\n";
    result << "===================\n";
    result << juce::String::formatted("Peak: %6.2f dB\n", 
                                     juce::Decibels::gainToDecibels(cachedMetrics.peakLevel + 1e-9f));
    result << juce::String::formatted("RMS:  %6.2f dB\n", 
                                     juce::Decibels::gainToDecibels(cachedMetrics.rmsLevel + 1e-9f));
    result << juce::String::formatted("Pushed: %7d\n", cachedMetrics.eventsProcessed);
    result << juce::String::formatted("Popped: %7d\n", cachedMetrics.queueDepth);
    result << juce::String::formatted("Q Max:  %7d\n", cachedMetrics.maxQueueDepth);
    
    return result;
}

inline juce::Rectangle<float> HudOverlay::calculateTextBounds() const
{
    auto text = formatMetrics();
    auto lines = juce::StringArray::fromLines(text);
    
    float maxWidth = 0.0f;
    for (const auto& line : lines)
    {
        float lineWidth = monospaceFont.getStringWidthFloat(line);
        maxWidth = juce::jmax(maxWidth, lineWidth);
    }
    
    float totalHeight = lines.size() * monospaceFont.getHeight();
    
    return juce::Rectangle<float>(
        static_cast<float>(MARGIN), 
        static_cast<float>(MARGIN),
        maxWidth, 
        totalHeight
    );
}