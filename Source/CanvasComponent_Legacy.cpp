/******************************************************************************
 * File: CanvasComponent.cpp
 * Description: Implementation of professional spectral canvas for SPECTRAL CANVAS PRO
 * 
 * Professional frequency domain painting interface with vintage DAW aesthetics.
 * Features precise coordinate mapping, technical labeling, and real-time analysis.
 * 
 * Copyright (c) 2025 Spectral Audio Systems
 ******************************************************************************/

#include "CanvasComponent.h"
#include "Core/SampleMaskingEngine.h"
#include "Core/SpectralSynthEngine.h"
#include "Core/Commands.h"
#include <cmath>

//==============================================================================
// Constructor - Initialize Professional Spectral Canvas
//==============================================================================

CanvasComponent::CanvasComponent()
{
    // Enable mouse interaction for professional painting
    setInterceptsMouseClicks(true, false);
    
    // Initialize professional cursor position
    currentCursorPos = juce::Point<float>(0.5f, 0.5f); // Center of canvas
}

//==============================================================================
// Professional Canvas Rendering
//==============================================================================

void CanvasComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    //==============================================================================
    // Retro Brutalist Canvas Background
    
    g.fillAll(juce::Colour(BrutalistCanvasColors::backgroundMain));
    
    //==============================================================================
    // Pixelated Grid System (Terminal Style)
    
    drawPixelatedGrid(g, bounds);
    
    //==============================================================================
    // Technical Frequency and Time Labeling
    
    if (showFrequencyLabels)
        drawTechnicalFrequencyLabels(g, bounds);
    
    if (showTimeLabels)
        drawTechnicalTimeLabels(g, bounds);
    
    //==============================================================================
    // Real-Time Spectral Analysis Display
    
    if (showSpectralAnalysis && spectralEngine)
        drawDitheredSpectralData(g, bounds);
    
    //==============================================================================
    // Blocky Paint Stroke Rendering
    
    drawBlockyPaintStrokes(g, bounds);
    
    //==============================================================================
    // Retro Cursor Crosshairs
    
    if (showCrosshairs)
        drawRetrosCrosshairs(g, bounds);
    
    //==============================================================================
    // Brutalist Canvas Border
    
    drawBrutalistBorder(g, bounds);
    
    //==============================================================================
    // Terminal Status Display
    
    drawTerminalStatusText(g, bounds);
    
    //==============================================================================
    // Terminal Instructions (When Empty)
    
    if (paintStrokes.empty())
    {
        g.setColour(juce::Colour(BrutalistCanvasColors::textLabels));
        g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 11.0f, juce::Font::plain));
        g.drawText("SPECTRAL CANVAS - FREQ DOMAIN PAINT", 
                  bounds.reduced(40), juce::Justification::centred);
        
        g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 9.0f, juce::Font::plain));
        g.drawText("X=TIME | Y=FREQ | PRESSURE=INTENSITY", 
                  bounds.removeFromBottom(20), juce::Justification::centred);
    }
}

//==============================================================================
// Pixelated Grid Rendering (Terminal Style)
//==============================================================================

void CanvasComponent::drawPixelatedGrid(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Smooth anti-aliased grid lines for professional appearance
    g.setColour(juce::Colour(BrutalistCanvasColors::gridDark));
    
    int numFreqLines = gridDensity; // Use configurable grid density
    for (int i = 1; i < numFreqLines; ++i)
    {
        float y = static_cast<float>(i * bounds.getHeight()) / numFreqLines;
        g.drawHorizontalLine(static_cast<int>(y), 0.0f, static_cast<float>(bounds.getWidth()));
    }
    
    // Smooth time grid (vertical lines)
    int numTimeLines = gridDensity;
    for (int i = 1; i < numTimeLines; ++i)
    {
        float x = static_cast<float>(i * bounds.getWidth()) / numTimeLines;
        g.drawVerticalLine(static_cast<int>(x), 0.0f, static_cast<float>(bounds.getHeight()));
    }
    
    // Major grid lines (every 4th line) - brighter and slightly thicker
    g.setColour(juce::Colour(BrutalistCanvasColors::gridMajor));
    
    for (int i = 4; i < numFreqLines; i += 4)
    {
        float y = static_cast<float>(i * bounds.getHeight()) / numFreqLines;
        juce::Line<float> line(0.0f, y, static_cast<float>(bounds.getWidth()), y);
        g.drawLine(line, 1.5f); // Slightly thicker major lines
    }
    
    for (int i = 4; i < numTimeLines; i += 4)
    {
        float x = static_cast<float>(i * bounds.getWidth()) / numTimeLines;
        juce::Line<float> line(x, 0.0f, x, static_cast<float>(bounds.getHeight()));
        g.drawLine(line, 1.5f); // Slightly thicker major lines
    }
}

//==============================================================================
// Technical Frequency Labeling (Terminal Style)
//==============================================================================

void CanvasComponent::drawTechnicalFrequencyLabels(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    g.setColour(juce::Colour(BrutalistCanvasColors::frequencyScale));
    g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 8.0f, juce::Font::plain));
    
    // Technical frequency markers with brutal abbreviations
    const float frequencies[] = { 20.0f, 50.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 
                                 2000.0f, 5000.0f, 10000.0f, 20000.0f };
    const char* freqLabels[] = { "20H", "50H", "100H", "200H", "500H", "1K", 
                                "2K", "5K", "10K", "20K" };
    
    int numLabels = sizeof(frequencies) / sizeof(frequencies[0]);
    
    for (int i = 0; i < numLabels; ++i)
    {
        float freqNorm = hzToFreqNorm(frequencies[i]);
        float y = freqNormToScreenY(freqNorm, bounds);
        
        if (y >= 10.0f && y <= bounds.getHeight() - 10.0f)
        {
            // Draw frequency label
            g.drawText(freqLabels[i], 2, static_cast<int>(y - 8), 35, 16, 
                      juce::Justification::left);
            
            // Draw technical tick mark (single pixel)
            g.setColour(juce::Colour(BrutalistCanvasColors::gridMajor));
            g.drawHorizontalLine(static_cast<int>(y), 0.0f, 30.0f);
            g.setColour(juce::Colour(BrutalistCanvasColors::frequencyScale));
        }
    }
}

//==============================================================================
// Technical Time Labeling (Terminal Style)
//==============================================================================

void CanvasComponent::drawTechnicalTimeLabels(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    g.setColour(juce::Colour(BrutalistCanvasColors::timeScale));
    g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 8.0f, juce::Font::plain));
    
    // Technical time markers with brutal precision
    float timeRange = endTimeSec - startTimeSec;
    int numTimeMarkers = 8;
    
    for (int i = 0; i <= numTimeMarkers; ++i)
    {
        float timeSec = startTimeSec + (static_cast<float>(i) / numTimeMarkers) * timeRange;
        float timeNorm = secToTimeNorm(timeSec);
        float x = timeNormToScreenX(timeNorm, bounds);
        
        if (x >= 30.0f && x <= bounds.getWidth() - 30.0f)
        {
            // Format time display with brutal technical precision
            juce::String timeText;
            if (timeSec < 1.0f)
            {
                timeText = juce::String(timeSec * 1000.0f, 0) + "MS";
            }
            else
            {
                timeText = juce::String(timeSec, 1) + "S";
            }
            
            // Draw time label
            g.drawText(timeText, static_cast<int>(x - 20), bounds.getHeight() - 15, 
                      40, 12, juce::Justification::centred);
            
            // Draw technical tick mark (single pixel)
            g.setColour(juce::Colour(BrutalistCanvasColors::gridMajor));
            g.drawVerticalLine(static_cast<int>(x), static_cast<float>(bounds.getHeight() - 15), 
                              static_cast<float>(bounds.getHeight()));
            g.setColour(juce::Colour(BrutalistCanvasColors::timeScale));
        }
    }
}

//==============================================================================
// Dithered Spectral Data Display (Authentic Terminal Style)
//==============================================================================

void CanvasComponent::drawDitheredSpectralData(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Real-time spectral data visualization from SpectralSynthEngine
    if (!spectralEngine) return;
    
    // Get real FFT analysis data
    auto spectralAnalysis = spectralEngine->getCurrentSpectralAnalysis();
    const auto& frequencyBins = spectralAnalysis.frequencyBins;
    const auto& amplitudeLevels = spectralAnalysis.amplitudeLevels;
    
    if (frequencyBins.empty() || amplitudeLevels.empty()) return;
    
    // Enable anti-aliasing for smooth rendering
    g.setColour(juce::Colour(BrutalistCanvasColors::spectralData));
    
    // Draw smooth spectral content using real FFT data
    int numBins = static_cast<int>(frequencyBins.size());
    float binWidth = static_cast<float>(bounds.getWidth()) / numBins;
    
    for (int bin = 0; bin < numBins; ++bin)
    {
        float amplitude = (bin < amplitudeLevels.size()) ? amplitudeLevels[bin] : 0.0f;
        
        // Convert amplitude to screen height
        float barHeight = amplitude * bounds.getHeight() * 0.8f; // Scale to 80% of canvas height
        
        // Calculate screen position
        float x = bin * binWidth;
        float y = bounds.getHeight() - barHeight;
        
        // Draw smooth anti-aliased bars
        juce::Rectangle<float> bar(x, y, binWidth - 1.0f, barHeight);
        
        // Color intensity based on amplitude
        float intensity = juce::jlimit(0.0f, 1.0f, amplitude * 2.0f);
        juce::Colour barColor = juce::Colour(BrutalistCanvasColors::spectralData).withAlpha(intensity);
        
        g.setColour(barColor);
        g.fillRect(bar);
        
        // Add bright highlights for strong signals
        if (amplitude > 0.7f)
        {
            g.setColour(juce::Colour(BrutalistCanvasColors::cursorCrosshair).withAlpha(0.8f));
            g.drawRect(bar, 1.0f);
        }
    }
}

//==============================================================================
// Blocky Paint Stroke Rendering (Pixelated Style)
//==============================================================================

void CanvasComponent::drawBlockyPaintStrokes(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    for (const auto& stroke : paintStrokes)
    {
        float x = timeNormToScreenX(stroke.timeNorm, bounds);
        float y = freqNormToScreenY(stroke.freqNorm, bounds);
        
        // Smooth paint stroke visualization with anti-aliasing
        juce::Colour strokeColor = stroke.color.isTransparent() ? 
            juce::Colour(BrutalistCanvasColors::paintStroke) : stroke.color;
        
        // Calculate brush size based on pressure
        float brushRadius = (brushSize * stroke.pressure) / 2.0f;
        brushRadius = juce::jmax(2.0f, brushRadius); // Minimum radius
        
        // Draw smooth circular paint stroke
        g.setColour(strokeColor.withAlpha(0.8f));
        g.fillEllipse(x - brushRadius, y - brushRadius, brushRadius * 2.0f, brushRadius * 2.0f);
        
        // Add smooth border for high contrast visibility
        g.setColour(juce::Colour(BrutalistCanvasColors::textLabels).withAlpha(0.6f));
        g.drawEllipse(x - brushRadius, y - brushRadius, brushRadius * 2.0f, brushRadius * 2.0f, 1.0f);
        
        // Pressure indication with smooth highlight
        if (stroke.pressure > 0.7f)
        {
            g.setColour(juce::Colour(BrutalistCanvasColors::cursorCrosshair).withAlpha(0.9f));
            float highlightRadius = brushRadius * 0.3f;
            g.fillEllipse(x - highlightRadius, y - highlightRadius, 
                         highlightRadius * 2.0f, highlightRadius * 2.0f);
        }
    }
}

//==============================================================================
// Retro Cursor Crosshairs (Terminal Style)
//==============================================================================

void CanvasComponent::drawRetrosCrosshairs(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    float x = timeNormToScreenX(currentCursorPos.getX(), bounds);
    float y = freqNormToScreenY(currentCursorPos.getY(), bounds);
    
    g.setColour(juce::Colour(BrutalistCanvasColors::cursorCrosshair).withAlpha(0.8f));
    
    // Smooth anti-aliased crosshair lines
    juce::Line<float> verticalLine(x, 0.0f, x, static_cast<float>(bounds.getHeight()));
    juce::Line<float> horizontalLine(0.0f, y, static_cast<float>(bounds.getWidth()), y);
    
    g.drawLine(verticalLine, 1.0f);
    g.drawLine(horizontalLine, 1.0f);
    
    // Technical coordinate display box with smooth edges
    g.setColour(juce::Colour(BrutalistCanvasColors::backgroundMain).withAlpha(0.9f));
    juce::Rectangle<float> infoBox(x + 10.0f, y - 20.0f, 100.0f, 15.0f);
    g.fillRect(infoBox);
    
    g.setColour(juce::Colour(BrutalistCanvasColors::textLabels));
    g.drawRect(infoBox, 1.0f);
    
    // Technical coordinate text (monospace)
    g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 8.0f, juce::Font::plain));
    juce::String coordText = getCurrentFrequencyText() + "|" + getCurrentTimeText();
    g.drawText(coordText, infoBox.reduced(1.0f), juce::Justification::centred);
}

//==============================================================================
// Brutalist Canvas Border (Single Pixel Frame)
//==============================================================================

void CanvasComponent::drawBrutalistBorder(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Brutalist single-pixel border frame
    g.setColour(juce::Colour(BrutalistCanvasColors::borderFrame));
    g.drawRect(bounds, 1);
}

//==============================================================================
// Terminal Status Text Display
//==============================================================================

void CanvasComponent::drawTerminalStatusText(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Technical status display in bottom corner
    juce::Rectangle<int> statusArea = bounds.removeFromBottom(12).removeFromRight(200);
    
    g.setColour(juce::Colour(BrutalistCanvasColors::backgroundMain));
    g.fillRect(statusArea);
    
    g.setColour(juce::Colour(BrutalistCanvasColors::textLabels));
    g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 8.0f, juce::Font::plain));
    
    juce::String statusText = "GRID:16X16 STROKES:" + juce::String(paintStrokes.size());
    g.drawText(statusText, statusArea, juce::Justification::centredRight);
}

//==============================================================================
// Professional Mouse Interaction
//==============================================================================

void CanvasComponent::mouseDown(const juce::MouseEvent& e)
{
    auto bounds = getLocalBounds();
    
    float timeNorm = screenXToTimeNorm(e.position.x, bounds);
    float freqNorm = screenYToFreqNorm(e.position.y, bounds);
    
    // Validate paint position
    if (!isValidPaintPosition(juce::Point<float>(timeNorm, freqNorm)))
        return;
    
    // Update cursor position
    currentCursorPos = juce::Point<float>(timeNorm, freqNorm);
    
    //==============================================================================
    // Enhanced Brush Characteristics Tracking (Following Gemini's recommendations)
    
    // Calculate pressure (TODO: Get real tablet pressure when available)
    float pressure = 0.8f; // Default pressure, will be replaced with tablet input
    
    // Update brush tracker
    brushTracker.updatePressure(pressure);
    brushTracker.updatePosition(currentCursorPos);
    
    // Pass brush characteristics to SpectralSynthEngine for tube amp control
    if (spectralEngine)
    {
        spectralEngine->updateBrushCharacteristics(
            brushTracker.currentPressure,
            brushTracker.velocity,
            currentPaintColor
        );
    }
    
    // Add professional paint stroke
    addPaintStroke(currentCursorPos, pressure);
    
    isCurrentlyPainting = true;
    updateEngineFromStrokes();
    
    // Trigger immediate spectral analysis update for real-time feedback
    if (spectralEngine)
        spectralEngine->enableSpectralAnalysis(true);
    
    repaint();
}

void CanvasComponent::mouseDrag(const juce::MouseEvent& e)
{
    if (!isCurrentlyPainting) return;
    
    auto bounds = getLocalBounds();
    
    float timeNorm = screenXToTimeNorm(e.position.x, bounds);
    float freqNorm = screenYToFreqNorm(e.position.y, bounds);
    
    // Validate paint position
    if (!isValidPaintPosition(juce::Point<float>(timeNorm, freqNorm)))
        return;
    
    // Update cursor position
    currentCursorPos = juce::Point<float>(timeNorm, freqNorm);
    
    //==============================================================================
    // Dynamic Pressure Simulation and Brush Tracking
    
    // Simulate pressure based on mouse movement speed (until tablet input available)
    float mouseSpeed = e.position.getDistanceFrom(brushTracker.previousPosition);
    float simulatedPressure = juce::jlimit(0.2f, 1.0f, 0.8f - mouseSpeed * 0.01f); // Faster = lighter pressure
    
    // Update brush tracker with new pressure and position
    brushTracker.updatePressure(simulatedPressure);
    brushTracker.updatePosition(currentCursorPos);
    
    // Real-time tube amp control during painting gestures
    if (spectralEngine)
    {
        spectralEngine->updateBrushCharacteristics(
            brushTracker.currentPressure,
            brushTracker.velocity,
            currentPaintColor
        );
    }
    
    // Add professional paint stroke with dynamic pressure
    addPaintStroke(currentCursorPos, simulatedPressure);
    
    updateEngineFromStrokes();
    
    // Trigger spectral analysis update for continuous feedback
    if (spectralEngine)
        spectralEngine->enableSpectralAnalysis(true);
    
    repaint();
}

void CanvasComponent::mouseUp(const juce::MouseEvent&)
{
    isCurrentlyPainting = false;
    
    // Reset brush pressure to zero when not painting
    brushTracker.updatePressure(0.0f);
    
    // Final tube amp update with zero pressure
    if (spectralEngine)
    {
        spectralEngine->updateBrushCharacteristics(
            0.0f,  // No pressure when not painting
            brushTracker.velocity,
            currentPaintColor
        );
    }
    
    updateEngineFromStrokes(); // Final engine update
}

//==============================================================================
// Professional Canvas Management
//==============================================================================

void CanvasComponent::clearCanvas()
{
    paintStrokes.clear();
    
    // Clear associated engine masks via command system
    if (commandProcessor)
    {
        Command clearCmd(SampleMaskingCommandID::ClearAllMasks);
        commandProcessor(clearCmd);
    }
    
    repaint();
}

//==============================================================================
// Professional Display Configuration
//==============================================================================

void CanvasComponent::setFrequencyRange(float minHz, float maxHz)
{
    minFrequencyHz = juce::jlimit(10.0f, 30000.0f, minHz);
    maxFrequencyHz = juce::jlimit(minFrequencyHz + 100.0f, 30000.0f, maxHz);
    repaint();
}

void CanvasComponent::setTimeRange(float startSec, float endSec)
{
    startTimeSec = juce::jmax(0.0f, startSec);
    endTimeSec = juce::jmax(startTimeSec + 0.1f, endSec);
    repaint();
}

//==============================================================================
// Professional Coordinate System
//==============================================================================

float CanvasComponent::screenXToTimeNorm(float x, juce::Rectangle<int> bounds) const
{
    return juce::jlimit(0.0f, 1.0f, x / bounds.getWidth());
}

float CanvasComponent::screenYToFreqNorm(float y, juce::Rectangle<int> bounds) const
{
    return juce::jlimit(0.0f, 1.0f, 1.0f - (y / bounds.getHeight()));
}

float CanvasComponent::timeNormToScreenX(float timeNorm, juce::Rectangle<int> bounds) const
{
    return timeNorm * bounds.getWidth();
}

float CanvasComponent::freqNormToScreenY(float freqNorm, juce::Rectangle<int> bounds) const
{
    return (1.0f - freqNorm) * bounds.getHeight();
}

//==============================================================================
// Professional Frequency Conversion (Logarithmic)
//==============================================================================

float CanvasComponent::freqNormToHz(float freqNorm) const
{
    // Professional logarithmic frequency mapping
    float logMin = std::log10(minFrequencyHz);
    float logMax = std::log10(maxFrequencyHz);
    float logFreq = logMin + freqNorm * (logMax - logMin);
    return std::pow(10.0f, logFreq);
}

float CanvasComponent::hzToFreqNorm(float hz) const
{
    // Professional logarithmic frequency mapping (inverse)
    float logMin = std::log10(minFrequencyHz);
    float logMax = std::log10(maxFrequencyHz);
    float logHz = std::log10(juce::jlimit(minFrequencyHz, maxFrequencyHz, hz));
    return (logHz - logMin) / (logMax - logMin);
}

float CanvasComponent::timeNormToSec(float timeNorm) const
{
    return startTimeSec + timeNorm * (endTimeSec - startTimeSec);
}

float CanvasComponent::secToTimeNorm(float sec) const
{
    float timeRange = endTimeSec - startTimeSec;
    return (sec - startTimeSec) / timeRange;
}

//==============================================================================
// Professional Cursor Information
//==============================================================================

juce::String CanvasComponent::getCurrentFrequencyText() const
{
    float hz = getCurrentFrequencyHz();
    
    if (hz < 1000.0f)
    {
        return juce::String(hz, 0) + "Hz";
    }
    else
    {
        return juce::String(hz / 1000.0f, 1) + "kHz";
    }
}

juce::String CanvasComponent::getCurrentTimeText() const
{
    float sec = getCurrentTimeSec();
    
    if (sec < 1.0f)
    {
        return juce::String(sec * 1000.0f, 0) + "ms";
    }
    else
    {
        return juce::String(sec, 2) + "s";
    }
}

float CanvasComponent::getCurrentFrequencyHz() const
{
    return freqNormToHz(currentCursorPos.getY());
}

float CanvasComponent::getCurrentTimeSec() const
{
    return timeNormToSec(currentCursorPos.getX());
}

//==============================================================================
// Professional Audio Engine Integration
//==============================================================================

void CanvasComponent::updateEngineFromStrokes()
{
    if (!paintStrokes.empty())
    {
        const auto& latestStroke = paintStrokes.back();
        
        // Update SampleMaskingEngine via command system
        if (commandProcessor)
        {
            // Professional frequency-to-processing mode mapping
            SampleMaskingEngine::MaskingMode mode = SampleMaskingEngine::MaskingMode::Volume;
            
            float freqHz = freqNormToHz(latestStroke.freqNorm);
            
            // Professional frequency band processing assignments
            if (freqHz > 10000.0f)          // High frequencies
                mode = SampleMaskingEngine::MaskingMode::Filter;
            else if (freqHz > 4000.0f)      // Mid-high frequencies
                mode = SampleMaskingEngine::MaskingMode::Pitch;
            else if (freqHz > 1000.0f)      // Mid frequencies
                mode = SampleMaskingEngine::MaskingMode::Ring;
            else if (freqHz > 200.0f)       // Low-mid frequencies
                mode = SampleMaskingEngine::MaskingMode::Stutter;
            else                            // Low frequencies
                mode = SampleMaskingEngine::MaskingMode::Distortion;
            
            // Create professional processing mask via command system
            Command createMaskCmd(SampleMaskingCommandID::CreatePaintMask, 
                                 static_cast<float>(static_cast<int>(mode)));
            createMaskCmd.color = latestStroke.color;
            commandProcessor(createMaskCmd);
            
            Command paintStrokeCmd(SampleMaskingCommandID::BeginPaintStroke, 
                                  latestStroke.timeNorm, latestStroke.freqNorm, 
                                  latestStroke.pressure, latestStroke.color);
            paintStrokeCmd.floatParam = static_cast<float>(static_cast<int>(mode));
            commandProcessor(paintStrokeCmd);
        }
        
        // Update SpectralSynthEngine directly for real-time synthesis
        if (spectralEngine)
        {
            float freqHz = freqNormToHz(latestStroke.freqNorm);
            float timeSec = timeNormToSec(latestStroke.timeNorm);
            
            // Process paint data for spectral synthesis
            SpectralSynthEngine::PaintData paintData;
            paintData.timeNorm = latestStroke.timeNorm;
            paintData.freqNorm = latestStroke.freqNorm;
            paintData.pressure = latestStroke.pressure;
            paintData.color = latestStroke.color;
            paintData.frequencyHz = freqHz;
            paintData.amplitude = latestStroke.pressure;
            paintData.timestamp = latestStroke.timestamp;
            
            spectralEngine->processPaintStroke(paintData);
        }
    }
}

juce::Colour CanvasComponent::getFrequencyBasedColor(float freqNorm) const
{
    // Professional frequency-to-color mapping (MetaSynth inspired)
    float hue = freqNorm * 0.75f; // Map to blue-to-red spectrum
    return juce::Colour::fromHSV(hue, 0.8f, 0.9f, 0.8f);
}

int CanvasComponent::getProcessingModeFromColor(juce::Colour paintColor) const
{
    // Professional color-to-processing mode mapping
    float hue = paintColor.getHue();
    
    if (hue < 0.1f || hue > 0.9f)      return 0; // Red: Volume
    else if (hue < 0.2f)               return 1; // Orange: Distortion
    else if (hue < 0.35f)              return 2; // Yellow: Filter
    else if (hue < 0.5f)               return 3; // Green: Ring mod
    else if (hue < 0.65f)              return 4; // Cyan: Pitch
    else                               return 5; // Blue/Purple: Stutter
}

//==============================================================================
// Professional Paint Stroke Management
//==============================================================================

void CanvasComponent::updateCursorPosition(const juce::MouseEvent& e)
{
    auto bounds = getLocalBounds();
    float timeNorm = screenXToTimeNorm(e.position.x, bounds);
    float freqNorm = screenYToFreqNorm(e.position.y, bounds);
    currentCursorPos = juce::Point<float>(timeNorm, freqNorm);
}

bool CanvasComponent::isValidPaintPosition(juce::Point<float> position) const
{
    // Professional bounds checking
    return position.getX() >= 0.0f && position.getX() <= 1.0f &&
           position.getY() >= 0.0f && position.getY() <= 1.0f;
}

void CanvasComponent::addPaintStroke(juce::Point<float> position, float pressure)
{
    // Create professional paint stroke with frequency-based color
    juce::Colour strokeColor = (currentPaintColor == juce::Colour(0xFF00FFFF)) ? 
                               getFrequencyBasedColor(position.getY()) : currentPaintColor;
    
    paintStrokes.emplace_back(position.getX(), position.getY(), pressure, strokeColor);
}