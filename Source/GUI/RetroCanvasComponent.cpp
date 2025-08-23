#include "RetroCanvasComponent.h"
#include "Core/PluginProcessor.h"
#include <cmath>

//==============================================================================
// SpectralCanvas Modern Color Definitions

const juce::Colour RetroCanvasComponent::CanvasColors::CANVAS_IVORY     = juce::Colour(0xffFFFFF8);  // Ivory canvas
const juce::Colour RetroCanvasComponent::CanvasColors::EMERALD_GREEN    = juce::Colour(0xff27AE60);  // Emerald green
const juce::Colour RetroCanvasComponent::CanvasColors::BRIGHT_ORANGE    = juce::Colour(0xffF39C12);  // Bright orange
const juce::Colour RetroCanvasComponent::CanvasColors::BRIGHT_BLUE      = juce::Colour(0xff3498DB);  // Bright blue
const juce::Colour RetroCanvasComponent::CanvasColors::DARK_BLUE_GRAY   = juce::Colour(0xff2C3E50);  // Dark blue-gray
const juce::Colour RetroCanvasComponent::CanvasColors::VIBRANT_RED      = juce::Colour(0xffE74C3C);  // Vibrant red
const juce::Colour RetroCanvasComponent::CanvasColors::ROYAL_BLUE       = juce::Colour(0xff4169E1);  // Royal blue
const juce::Colour RetroCanvasComponent::CanvasColors::PURPLE           = juce::Colour(0xff9B59B6);  // Purple
const juce::Colour RetroCanvasComponent::CanvasColors::CANVAS_WHITE     = juce::Colour(0xffFFFFF8);  // Pure white canvas
const juce::Colour RetroCanvasComponent::CanvasColors::PAINT_BLUE       = juce::Colour(0xff4169E1);  // Royal blue paint
const juce::Colour RetroCanvasComponent::CanvasColors::GRID_LIGHT       = juce::Colour(0xffBDC3C7);  // Light gray grid

//==============================================================================
// Constructor and Destructor

RetroCanvasComponent::RetroCanvasComponent()
{
    setMouseCursor(juce::MouseCursor::CrosshairCursor);
    setWantsKeyboardFocus(true);
    
    // Timer will be started when component becomes visible
    
    // Initialize canvas state with MetaSynth-inspired paint-first defaults
    canvasState.minFreq = 80.0f;              // Musical bass range
    canvasState.maxFreq = 8000.0f;            // Upper treble
    canvasState.timeRange = 8.0f;             // 8 seconds visible
    canvasState.zoomLevel = 1.0f;
    canvasState.showGrid = true;
    canvasState.showSpectralOverlay = true;   // Real-time spectral feedback
    canvasState.showRulers = false;           // Hidden by default for canvas-first design
    canvasState.immersiveMode = true;         // Start in paint-focused mode
    canvasState.currentBrush = BrushType::PaintBrush;  // MetaSynth-style brush
    canvasState.paintMode = ColorChannel::Brightness;  // Start with brightness painting
    canvasState.metaSynthColoring = true;     // Enable MetaSynth color mapping
    canvasState.paintIntensity = 1.0f;
    canvasState.harmonicSpread = 0.0f;
    
    // Add component listener for visibility-based timer control
    addComponentListener(this);
    
    DBG("SpectralCanvas initialized - MetaSynth paint-to-audio interface ready!");
}

RetroCanvasComponent::~RetroCanvasComponent()
{
    stopTimer();
    removeComponentListener(this);
}

//==============================================================================
// Component Overrides

void RetroCanvasComponent::paint(juce::Graphics& g)
{
    // Calculate canvas-first layout geometry
    const auto geom = calculateGeometry();
    
    // CANVAS-FIRST RENDERING PIPELINE
    
    // 1. MetaSynth-style spectral room background (immersive environment)
    drawSpectralRoom(g, geom);
    
    // 2. Canvas background - the primary creative surface
    drawCanvasBackground(g, geom);
    
    // 3. Real-time spectral overlay (key MetaSynth feature)
    if (canvasState.showSpectralOverlay)
    {
        drawRealTimeSpectralOverlay(g, geom);
    }
    
    // 4. Paint canvas border - minimal, non-intrusive
    if (!canvasState.immersiveMode)
    {
        drawTerminalBorder(g, geom.canvasArea, "SPECTRAL PAINT CANVAS", CanvasColors::EMERALD_GREEN.withAlpha(0.6f));
    }
    
    // 5. Grid system - subtle frequency/time reference
    if (canvasState.showGrid)
    {
        drawFrequencyGrid(g, geom);
        drawTimeGrid(g, geom);
    }
    
    // 6. Painted strokes - the core audio-visual content
    drawPaintedStrokes(g, geom);
    
    // 7. Playhead - real-time position indicator
    drawPlayhead(g, geom);
    
    // 8. Contextual UI elements (shown only when needed)
    drawContextualUI(g, geom);
    
    // 9. Floating tool palette (non-intrusive)
    if (isMouseOver() || isPainting)
    {
        drawFloatingToolPalette(g, geom);
    }
    
    // 10. Paint cursor with MetaSynth color mapping
    if (showCursor && isMouseOver())
    {
        drawBrushCursor(g);
    }
    
    // 11. Minimal status bar
    drawStatusBar(g, geom);
    
    // 12. Visual effects and feedback
    drawScanlines(g, getLocalBounds());
    
    for (const auto& particle : particles)
    {
        drawParticleEffect(g, particle.position, particle.life, particle.color);
    }
}

void RetroCanvasComponent::resized()
{
    geometryNeedsUpdate = true;
}

void RetroCanvasComponent::mouseDown(const juce::MouseEvent& e)
{
    // SUB-5MS OPTIMIZATION: Pre-calculate frequently used values
    const auto& position = e.getPosition();
    
    // FAST BOUNDS CHECK: Skip expensive geometry calculation if outside canvas
    const auto& bounds = getLocalBounds();
    if (!bounds.contains(position)) return;
    
    // CACHED GEOMETRY: Use cached geometry to avoid recalculation
    const auto geom = cachedGeometry.canvasArea.isEmpty() ? calculateGeometry() : cachedGeometry;
    
    if (!geom.canvasArea.contains(position)) return;
    
    // FAST PATH COORDINATE CONVERSION: Inline calculations for minimal latency
    const float timeNorm = static_cast<float>(position.x - geom.canvasArea.getX()) / geom.canvasArea.getWidth();
    const float freqNorm = 1.0f - static_cast<float>(position.y - geom.canvasArea.getY()) / geom.canvasArea.getHeight();
    const float pressure = e.mods.isLeftButtonDown() ? 1.0f : 0.5f;
    
    // DIRECT PAINT STROKE INITIATION: Skip intermediate canvas point conversion
    beginPaintStroke({timeNorm * canvasState.timeRange, freqNormToFrequency(freqNorm)}, pressure);
    
    // OPTIMIZED METASYNTH MAPPING: Direct frequency calculation
    const float frequency = canvasState.minFreq * std::pow(canvasState.maxFreq / canvasState.minFreq, freqNorm);
    
    // MASK PAINTING: Handle mask brush types
    if (canvasState.currentBrush == BrushType::MaskBrush || canvasState.currentBrush == BrushType::EraseBrush)
    {
        paintMaskAtPosition(timeNorm, freqNorm, pressure);
    }
    
    // FAST PARTICLE GENERATION: Minimal overhead feedback
    if (canvasState.metaSynthColoring) {
        addMetaSynthParticleFast(position.toFloat(), canvasState.paintMode, pressure, frequency);
    }
}

void RetroCanvasComponent::mouseDrag(const juce::MouseEvent& e)
{
    // ULTRA-FAST MOUSE TRACKING: Direct position update without validation
    mousePosition = e.getPosition();
    
    if (!isPainting) return;
    
    // FAST BOUNDS CHECK: Use cached geometry for speed
    const auto& geom = cachedGeometry;
    const auto& position = e.getPosition();
    
    if (!geom.canvasArea.contains(position)) return;
    
    // SUB-5MS PATH: Inline all coordinate conversions
    const float timeNorm = static_cast<float>(position.x - geom.canvasArea.getX()) / geom.canvasArea.getWidth();
    const float freqNorm = 1.0f - static_cast<float>(position.y - geom.canvasArea.getY()) / geom.canvasArea.getHeight();
    const float pressure = e.mods.isLeftButtonDown() ? 1.0f : 0.5f;
    const float frequency = canvasState.minFreq * std::pow(canvasState.maxFreq / canvasState.minFreq, freqNorm);
    
    // DIRECT PAINT UPDATE: Skip intermediate processing
    updatePaintStroke({timeNorm * canvasState.timeRange, frequency}, pressure);
    
    // MASK PAINTING: Handle mask brush types during drag
    if (canvasState.currentBrush == BrushType::MaskBrush || canvasState.currentBrush == BrushType::EraseBrush)
    {
        paintMaskAtPosition(timeNorm, freqNorm, pressure);
    }
    
    // FAST GESTURE PROCESSING: Inline frequency/pan mapping
    const float panPosition = (timeNorm * 2.0f - 1.0f);  // -1 to 1
    processFrequencyGesture(frequency, pressure, panPosition);
    
    // OPTIMIZED PARTICLE GENERATION: Reduced frequency for performance
    static int particleCounter = 0;
    if ((++particleCounter & 0x3) == 0 && canvasState.metaSynthColoring) {  // Every 4th call
        addMetaSynthParticleFast(position.toFloat(), canvasState.paintMode, pressure, frequency);
    }
}

void RetroCanvasComponent::mouseUp(const juce::MouseEvent& e)
{
    juce::ignoreUnused(e);
    
    if (isPainting)
    {
        endPaintStroke();
        
        // MASK PAINTING: Commit mask changes to audio thread
        if (canvasState.currentBrush == BrushType::MaskBrush || canvasState.currentBrush == BrushType::EraseBrush)
        {
            commitMaskChanges();
        }
        
        DBG("Paint stroke completed");
    }
}

void RetroCanvasComponent::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
    // Zoom with mouse wheel
    if (e.mods.isCtrlDown())
    {
        float zoomFactor = 1.0f + (wheel.deltaY * 0.1f);
        canvasState.zoomLevel = juce::jlimit(0.1f, 10.0f, canvasState.zoomLevel * zoomFactor);
        geometryNeedsUpdate = true;
        repaint();
    }
    // Scroll vertically (frequency)
    else if (e.mods.isShiftDown())
    {
        canvasState.scrollY += wheel.deltaY * 50.0f;
        repaint();
    }
    // Scroll horizontally (time)
    else
    {
        canvasState.scrollX += wheel.deltaX * 50.0f;
        repaint();
    }
}

void RetroCanvasComponent::timerCallback()
{
    // Update animation time
    animationTime += 1.0f / 60.0f;
    
    // Update particles
    updateParticles();
    
    // Cursor blinking effect
    showCursor = std::sin(animationTime * 4.0f) > 0.0f;
    
    // Repaint for smooth animation
    repaint();
}

void RetroCanvasComponent::componentVisibilityChanged(juce::Component& component)
{
    // RT-SAFE: Control timer based on visibility
    if (&component == this)
    {
        if (component.isShowing())
        {
            startTimerHz(30); // RT-SAFE: ≤ 30 Hz for UI updates
        }
        else
        {
            stopTimer();
        }
    }
}

//==============================================================================
// Drawing Methods

void RetroCanvasComponent::drawTerminalBorder(juce::Graphics& g, juce::Rectangle<int> area, 
                                            const juce::String& title, juce::Colour borderColor)
{
    // Draw double-line ASCII border
    g.setColour(borderColor);
    
    // Outer border
    g.drawRect(area, 2);
    
    // Inner border
    area = area.reduced(3);
    g.drawRect(area, 1);
    
    // Title bar
    if (title.isNotEmpty())
    {
        const int titleHeight = 20;
        auto titleArea = area.removeFromTop(titleHeight);
        
        g.setColour(borderColor.withAlpha(0.3f));
        g.fillRect(titleArea);
        
        g.setColour(borderColor);
        g.drawRect(titleArea, 1);
        
        g.setFont(createBoldTerminalFont(12.0f));
        g.drawText("★ " + title + " ★", titleArea, juce::Justification::centred);
    }
}

void RetroCanvasComponent::drawFrequencyGrid(juce::Graphics& g, const CanvasGeometry& geom)
{
    g.setColour(CanvasColors::GRID_LIGHT);
    g.setFont(createTerminalFont(10.0f));
    
    // Draw frequency grid lines and labels
    const float logMinFreq = std::log2(canvasState.minFreq);
    const float logMaxFreq = std::log2(canvasState.maxFreq);
    
    // Draw octave lines
    for (float octave = std::ceil(logMinFreq); octave <= logMaxFreq; octave += 1.0f)
    {
        const float freq = std::pow(2.0f, octave);
        const int y = frequencyToScreenY(freq, geom);
        
        if (y >= geom.canvasArea.getY() && y <= geom.canvasArea.getBottom())
        {
            // Draw grid line
            g.drawLine(geom.canvasArea.getX(), y, geom.canvasArea.getRight(), y, 1.0f);
            
            // Draw frequency label
            juce::String freqLabel;
            if (freq >= 1000.0f)
                freqLabel = juce::String(freq / 1000.0f, 1) + "k";
            else
                freqLabel = juce::String((int)freq);
                
            g.drawText(freqLabel + "Hz", 
                      geom.freqRuler.getX(), y - 8, 
                      geom.freqRuler.getWidth() - 5, 16, 
                      juce::Justification::centredRight);
        }
    }
    
    // Draw sub-octave lines (dimmer)
    g.setColour(CanvasColors::GRID_LIGHT.withAlpha(0.3f));
    for (float octave = logMinFreq; octave <= logMaxFreq; octave += 0.5f)
    {
        const float freq = std::pow(2.0f, octave);
        const int y = frequencyToScreenY(freq, geom);
        
        if (y >= geom.canvasArea.getY() && y <= geom.canvasArea.getBottom())
        {
            g.drawLine(geom.canvasArea.getX(), y, geom.canvasArea.getRight(), y, 1.0f);
        }
    }
}

void RetroCanvasComponent::drawTimeGrid(juce::Graphics& g, const CanvasGeometry& geom)
{
    g.setColour(CanvasColors::GRID_LIGHT);
    g.setFont(createTerminalFont(10.0f));
    
    // Draw time grid lines and labels
    const float timeStep = 1.0f; // 1 second intervals
    
    for (float time = 0.0f; time <= canvasState.timeRange; time += timeStep)
    {
        const int x = timeToScreenX(time, geom);
        
        if (x >= geom.canvasArea.getX() && x <= geom.canvasArea.getRight())
        {
            // Draw grid line
            g.drawLine(x, geom.canvasArea.getY(), x, geom.canvasArea.getBottom(), 1.0f);
            
            // Draw time label
            juce::String timeLabel = juce::String(time, 1) + "s";
            g.drawText(timeLabel,
                      x - 20, geom.timeRuler.getY(),
                      40, geom.timeRuler.getHeight(),
                      juce::Justification::centred);
        }
    }
    
    // Draw sub-second lines (dimmer)
    g.setColour(CanvasColors::GRID_LIGHT.withAlpha(0.3f));
    for (float time = 0.0f; time <= canvasState.timeRange; time += 0.25f)
    {
        const int x = timeToScreenX(time, geom);
        
        if (x >= geom.canvasArea.getX() && x <= geom.canvasArea.getRight())
        {
            g.drawLine(x, geom.canvasArea.getY(), x, geom.canvasArea.getBottom(), 1.0f);
        }
    }
}

void RetroCanvasComponent::drawPaintedStrokes(juce::Graphics& g, const CanvasGeometry& geom)
{
    // TODO: Get painted strokes from PaintEngine and visualize them
    // For now, draw a placeholder pattern
    
    g.setColour(CanvasColors::PAINT_BLUE.withAlpha(0.7f));
    
    // Draw some example painted areas as bright blocks
    for (int i = 0; i < 10; ++i)
    {
        const float time = i * 0.8f;
        const float freq = 200.0f + i * 100.0f;
        const int x = timeToScreenX(time, geom);
        const int y = frequencyToScreenY(freq, geom);
        const int width = (int)(geom.pixelsPerSecond * 0.3f);
        const int height = 8;
        
        if (x >= geom.canvasArea.getX() && x <= geom.canvasArea.getRight() &&
            y >= geom.canvasArea.getY() && y <= geom.canvasArea.getBottom())
        {
            g.fillRect(x, y - height/2, width, height);
            
            // Add glow effect
            g.setColour(CanvasColors::PAINT_BLUE.withAlpha(0.3f));
            g.fillRect(x - 1, y - height/2 - 1, width + 2, height + 2);
            g.setColour(CanvasColors::PAINT_BLUE.withAlpha(0.7f));
        }
    }
}

void RetroCanvasComponent::drawPlayhead(juce::Graphics& g, const CanvasGeometry& geom)
{
    // TODO: Get actual playhead position from PaintEngine
    const float playheadTime = std::fmod(animationTime, canvasState.timeRange);
    const int playheadX = timeToScreenX(playheadTime, geom);
    
    if (playheadX >= geom.canvasArea.getX() && playheadX <= geom.canvasArea.getRight())
    {
        // Draw playhead line with glow effect
        g.setColour(CanvasColors::BRIGHT_BLUE.withAlpha(0.8f));
        g.drawLine(playheadX, geom.canvasArea.getY(), 
                   playheadX, geom.canvasArea.getBottom(), 2.0f);
        
        // Add glow
        g.setColour(CanvasColors::BRIGHT_BLUE.withAlpha(0.3f));
        g.drawLine(playheadX - 1, geom.canvasArea.getY(), 
                   playheadX - 1, geom.canvasArea.getBottom(), 4.0f);
        g.drawLine(playheadX + 1, geom.canvasArea.getY(), 
                   playheadX + 1, geom.canvasArea.getBottom(), 4.0f);
        
        // Draw playhead triangle at top
        juce::Path triangle;
        triangle.addTriangle(playheadX - 5, geom.canvasArea.getY() - 5,
                           playheadX + 5, geom.canvasArea.getY() - 5,
                           playheadX, geom.canvasArea.getY());
        g.setColour(CanvasColors::BRIGHT_BLUE);
        g.fillPath(triangle);
    }
}

void RetroCanvasComponent::drawWaveformPreview(juce::Graphics& g, const CanvasGeometry& geom)
{
    // TODO: Get actual waveform data from PaintEngine
    // For now, draw a placeholder waveform
    
    g.setColour(CanvasColors::EMERALD_GREEN.withAlpha(0.6f));
    
    const int centerY = geom.waveformArea.getCentreY();
    const int amplitude = geom.waveformArea.getHeight() / 4;
    
    juce::Path waveform;
    bool firstPoint = true;
    
    for (int x = geom.waveformArea.getX(); x < geom.waveformArea.getRight(); x += 2)
    {
        const float time = screenXToTime(x, geom);
        const float sample = std::sin(time * 440.0f * 2.0f * juce::MathConstants<float>::pi) * 
                           std::exp(-time * 2.0f); // Decay envelope
        const int y = centerY + (int)(sample * amplitude);
        
        if (firstPoint)
        {
            waveform.startNewSubPath(x, y);
            firstPoint = false;
        }
        else
        {
            waveform.lineTo(x, y);
        }
    }
    
    g.strokePath(waveform, juce::PathStrokeType(1.0f));
    
    // Draw waveform area border
    g.setColour(CanvasColors::EMERALD_GREEN);
    g.drawRect(geom.waveformArea, 1);
    
    // Add "WAVEFORM" label
    g.setFont(createTerminalFont(8.0f));
    g.drawText("WAVEFORM", 
              geom.waveformArea.getX() + 2, geom.waveformArea.getY() + 2,
              60, 10, juce::Justification::left);
}

void RetroCanvasComponent::drawStatusBar(juce::Graphics& g, const CanvasGeometry& geom)
{
    // Minimal status bar for canvas-first design
    if (geom.statusBar.isEmpty()) return;  // Skip in immersive mode
    
    g.setColour(CanvasColors::CANVAS_IVORY.withAlpha(0.8f));
    g.fillRect(geom.statusBar);
    
    g.setColour(CanvasColors::EMERALD_GREEN.withAlpha(0.6f));
    g.drawRect(geom.statusBar, 1);
    
    g.setFont(createTerminalFont(9.0f));
    g.setColour(CanvasColors::DARK_BLUE_GRAY);
    
    // Minimal status info - focus on paint-relevant data
    juce::String statusText;
    
    if (canvasState.immersiveMode)
    {
        // Ultra-minimal status for immersive mode
        statusText = juce::String::formatted("%s | %.1fx | %s",
                                           isPainting ? "PAINTING" : "READY",
                                           canvasState.zoomLevel,
                                           canvasState.metaSynthColoring ? "META" : "RETRO");
    }
    else
    {
        // Standard minimal status
        statusText = juce::String::formatted("STATUS: %s | OSC: %d | CPU: %d%% | ZOOM: %.1fx | CANVAS: %.0f%%",
                                           isPainting ? "PAINTING" : "READY",
                                           currentActiveOscillators,
                                           (int)(currentCPULoad * 100.0f),
                                           canvasState.zoomLevel,
                                           cachedGeometry.canvasOccupancy * 100.0f);
    }
    
    g.drawText(statusText, 
              geom.statusBar.reduced(5, 1), 
              juce::Justification::centredLeft);
    
    // MetaSynth paint mode indicator
    if (canvasState.metaSynthColoring)
    {
        juce::String paintModeText;
        switch (canvasState.paintMode)
        {
            case ColorChannel::Red: paintModeText = "L"; break;
            case ColorChannel::Green: paintModeText = "R"; break;
            case ColorChannel::Yellow: paintModeText = "C"; break;
            case ColorChannel::Brightness: paintModeText = "B"; break;
            case ColorChannel::Hue: paintModeText = "H"; break;
            default: paintModeText = "S"; break;
        }
        
        g.setColour(getMetaSynthColor(canvasState.paintMode, 1.0f, 440.0f));
        g.drawText(paintModeText, 
                  geom.statusBar.getRight() - 25, geom.statusBar.getY() + 1,
                  20, geom.statusBar.getHeight() - 2, 
                  juce::Justification::centred);
    }
}

void RetroCanvasComponent::drawBrushCursor(juce::Graphics& g)
{
    const auto geom = calculateGeometry();
    
    if (!geom.canvasArea.contains(mousePosition))
        return;
    
    // Draw crosshair cursor
    g.setColour(brushColor.withAlpha(0.8f));
    
    const int size = (int)(brushSize * 10.0f);
    const int x = mousePosition.x;
    const int y = mousePosition.y;
    
    // Crosshair lines
    g.drawLine(x - size, y, x + size, y, 1.0f);
    g.drawLine(x, y - size, x, y + size, 1.0f);
    
    // Brush size circle
    g.drawEllipse(x - size/2, y - size/2, size, size, 1.0f);
    
    // Add frequency/time info near cursor
    const auto canvasPoint = screenToCanvas(mousePosition);
    const float freq = screenYToFrequency(mousePosition.y, geom);
    const float time = screenXToTime(mousePosition.x, geom);
    
    g.setFont(createTerminalFont(8.0f));
    g.setColour(CanvasColors::DARK_BLUE_GRAY);
    
    juce::String infoText = juce::String::formatted("%.1fHz | %.2fs", freq, time);
    g.drawText(infoText, 
              mousePosition.x + 10, mousePosition.y - 20,
              80, 15, juce::Justification::left);
}

void RetroCanvasComponent::drawParticleEffect(juce::Graphics& g, juce::Point<float> position, 
                                            float intensity, juce::Colour color)
{
    g.setColour(color.withAlpha(intensity));
    const float size = 2.0f + intensity * 3.0f;
    g.fillEllipse(position.x - size/2, position.y - size/2, size, size);
    
    // Add glow
    g.setColour(color.withAlpha(intensity * 0.3f));
    const float glowSize = size * 2.0f;
    g.fillEllipse(position.x - glowSize/2, position.y - glowSize/2, glowSize, glowSize);
}

void RetroCanvasComponent::drawScanlines(juce::Graphics& g, juce::Rectangle<int> area)
{
    // Draw subtle CRT scanlines for authentic retro feel
    g.setColour(CanvasColors::CANVAS_IVORY.brighter(0.05f));
    
    for (int y = area.getY(); y < area.getBottom(); y += 2)
    {
        g.drawLine(area.getX(), y, area.getRight(), y, 1.0f);
    }
}

//==============================================================================
// Font Management

juce::Font RetroCanvasComponent::createTerminalFont(float size) const
{
    return juce::Font("Courier New", size, juce::Font::FontStyleFlags::plain)
           .withExtraKerningFactor(0.0f);
}

juce::Font RetroCanvasComponent::createBoldTerminalFont(float size) const
{
    return juce::Font("Courier New", size, juce::Font::FontStyleFlags::bold)
           .withExtraKerningFactor(0.0f);
}

//==============================================================================
// Coordinate Conversion

juce::Point<float> RetroCanvasComponent::screenToCanvas(juce::Point<int> screenPoint) const
{
    const auto geom = calculateGeometry();
    
    const float time = screenXToTime(screenPoint.x, geom);
    const float freq = screenYToFrequency(screenPoint.y, geom);
    
    return {time, freq};
}

juce::Point<int> RetroCanvasComponent::canvasToScreen(juce::Point<float> canvasPoint) const
{
    const auto geom = calculateGeometry();
    
    const int x = timeToScreenX(canvasPoint.x, geom);
    const int y = frequencyToScreenY(canvasPoint.y, geom);
    
    return {x, y};
}

float RetroCanvasComponent::screenYToFrequency(int screenY, const CanvasGeometry& geom) const
{
    const float normalizedY = 1.0f - (float)(screenY - geom.canvasArea.getY()) / geom.canvasArea.getHeight();
    const float logMinFreq = std::log2(canvasState.minFreq);
    const float logMaxFreq = std::log2(canvasState.maxFreq);
    const float logFreq = logMinFreq + normalizedY * (logMaxFreq - logMinFreq);
    
    return std::pow(2.0f, logFreq);
}

int RetroCanvasComponent::frequencyToScreenY(float frequency, const CanvasGeometry& geom) const
{
    const float logMinFreq = std::log2(canvasState.minFreq);
    const float logMaxFreq = std::log2(canvasState.maxFreq);
    const float logFreq = std::log2(juce::jlimit(canvasState.minFreq, canvasState.maxFreq, frequency));
    const float normalizedY = (logFreq - logMinFreq) / (logMaxFreq - logMinFreq);
    
    return geom.canvasArea.getY() + (int)((1.0f - normalizedY) * geom.canvasArea.getHeight());
}

float RetroCanvasComponent::screenXToTime(int screenX, const CanvasGeometry& geom) const
{
    return (float)(screenX - geom.canvasArea.getX()) / geom.pixelsPerSecond;
}

int RetroCanvasComponent::timeToScreenX(float time, const CanvasGeometry& geom) const
{
    return geom.canvasArea.getX() + (int)(time * geom.pixelsPerSecond);
}

RetroCanvasComponent::CanvasGeometry RetroCanvasComponent::calculateGeometry() const
{
    if (!geometryNeedsUpdate)
        return cachedGeometry;
    
    CanvasGeometry geom;
    auto bounds = getLocalBounds();
    
    // CANVAS-FIRST LAYOUT: 80% screen occupation philosophy
    const int totalWidth = bounds.getWidth();
    const int totalHeight = bounds.getHeight();
    
    if (canvasState.immersiveMode)
    {
        // Immersive mode: Canvas takes 95% of screen, minimal UI chrome
        const int minimalStatusHeight = 15;  // Tiny status bar
        geom.statusBar = bounds.removeFromBottom(minimalStatusHeight);
        geom.canvasArea = bounds;  // Canvas gets everything else
        
        // Contextual UI elements as floating overlays (positioned dynamically)
        const int paletteWidth = 120;
        const int paletteHeight = 200;
        geom.toolPalette = juce::Rectangle<int>(totalWidth - paletteWidth - 10, 10, paletteWidth, paletteHeight);
        geom.parameterPanel = juce::Rectangle<int>(10, 10, 150, 100);
        
        // Rulers only shown when needed (edge-triggered)
        geom.timeRuler = juce::Rectangle<int>();  // Hidden
        geom.freqRuler = juce::Rectangle<int>();  // Hidden
    }
    else
    {
        // Standard mode: Still prioritize canvas but show some chrome
        const int statusBarHeight = 18;
        const int rulerMargin = canvasState.showRulers ? 40 : 8;
        
        geom.statusBar = bounds.removeFromBottom(statusBarHeight);
        
        if (canvasState.showRulers)
        {
            geom.timeRuler = bounds.removeFromBottom(20);
            geom.freqRuler = bounds.removeFromLeft(50);
        }
        
        // Canvas gets the majority of remaining space
        geom.canvasArea = bounds.reduced(rulerMargin / 2);
        
        // Tool palette as sidebar overlay
        const int paletteWidth = 100;
        geom.toolPalette = juce::Rectangle<int>(totalWidth - paletteWidth, 0, paletteWidth, totalHeight / 3);
    }
    
    // Spectral overlay matches canvas area
    geom.spectralOverlay = geom.canvasArea;
    
    // Calculate scaling factors optimized for paint gestures
    geom.pixelsPerSecond = geom.canvasArea.getWidth() / canvasState.timeRange * canvasState.zoomLevel;
    geom.pixelsPerOctave = 80.0f * canvasState.zoomLevel;
    geom.canvasOccupancy = static_cast<float>(geom.canvasArea.getWidth() * geom.canvasArea.getHeight()) / 
                          static_cast<float>(totalWidth * totalHeight);
    
    cachedGeometry = geom;
    geometryNeedsUpdate = false;
    
    return geom;
}

//==============================================================================
// Painting Logic

void RetroCanvasComponent::beginPaintStroke(juce::Point<float> canvasPoint, float pressure)
{
    isPainting = true;
    lastPaintPoint = canvasPoint;
    brushPressure = pressure;
    
    // 🚀 SUB-5MS OPTIMIZATION: Use direct audio thread pathway for maximum responsiveness
    if (processor && processor->getSpectralSynthEngine().isInitialized())
    {
        // DIRECT PAINT-TO-AUDIO: Bypass command queue for <5ms latency
        auto& synthEngine = processor->getSpectralSynthEngine();
        synthEngine.processPaintStrokeDirectToAudio(canvasPoint.x, canvasPoint.y, pressure, brushColor);
        
        // Also send to PaintEngine for visual feedback (non-blocking)
        sendPaintCommand(PaintCommandID::BeginStroke, canvasPoint, pressure);
    }
    else if (currentBrushType == BrushType::SampleBrush && processor)
    {
        // Handle SampleBrush - trigger ForgeVoice samples
        processor->triggerPaintBrush(canvasPoint.y, pressure);
    }
    else
    {
        // Fallback to standard command queue
        sendPaintCommand(PaintCommandID::BeginStroke, canvasPoint, pressure);
    }
}

void RetroCanvasComponent::updatePaintStroke(juce::Point<float> canvasPoint, float pressure)
{
    if (!isPainting)
        return;
    
    lastPaintPoint = canvasPoint;
    brushPressure = pressure;
    
    // 🚀 SUB-5MS OPTIMIZATION: Direct audio updates for continuous painting responsiveness
    if (processor && processor->getSpectralSynthEngine().isInitialized())
    {
        // CONTINUOUS DIRECT UPDATES: Maintain <5ms latency during paint strokes
        auto& synthEngine = processor->getSpectralSynthEngine();
        synthEngine.processPaintStrokeDirectToAudio(canvasPoint.x, canvasPoint.y, pressure, brushColor);
        
        // Visual feedback update (non-blocking)
        sendPaintCommand(PaintCommandID::UpdateStroke, canvasPoint, pressure);
    }
    else if (currentBrushType == BrushType::SampleBrush && processor)
    {
        // Handle SampleBrush - update pitch in real-time
        processor->triggerPaintBrush(canvasPoint.y, pressure);
    }
    else
    {
        // Fallback to standard command queue
        sendPaintCommand(PaintCommandID::UpdateStroke, canvasPoint, pressure);
    }
}

void RetroCanvasComponent::endPaintStroke()
{
    if (!isPainting)
        return;
    
    isPainting = false;
    
    // Handle SampleBrush differently - stop the sample
    if (currentBrushType == BrushType::SampleBrush && processor)
    {
        processor->stopPaintBrush();
    }
    else
    {
        // Use standard PaintEngine for other brush types
        sendPaintCommand(PaintCommandID::EndStroke, {0, 0});
    }
}

void RetroCanvasComponent::sendPaintCommand(PaintCommandID commandID, juce::Point<float> canvasPoint, float pressure)
{
    if (commandTarget)
    {
        Command cmd(commandID, canvasPoint.x, canvasPoint.y, pressure, brushColor);
        commandTarget(cmd);
    }
}

void RetroCanvasComponent::addParticleAt(juce::Point<float> position, juce::Colour color)
{
    Particle particle;
    particle.position = position;
    particle.velocity = {
        (juce::Random::getSystemRandom().nextFloat() - 0.5f) * 20.0f,
        (juce::Random::getSystemRandom().nextFloat() - 0.5f) * 20.0f
    };
    particle.color = color;
    particle.life = 1.0f;
    particle.size = 2.0f + juce::Random::getSystemRandom().nextFloat() * 3.0f;
    
    particles.push_back(particle);
    
    // Limit particle count for performance
    if (particles.size() > 100)
    {
        particles.erase(particles.begin());
    }
}

void RetroCanvasComponent::updateParticles()
{
    const float deltaTime = 1.0f / 60.0f;
    
    for (auto it = particles.begin(); it != particles.end();)
    {
        auto& particle = *it;
        
        // Update position
        particle.position += particle.velocity * deltaTime;
        
        // Update life
        particle.life -= deltaTime * 2.0f; // 0.5 second lifetime
        
        // Apply gravity and friction
        particle.velocity.y += 50.0f * deltaTime; // Gravity
        particle.velocity *= 0.98f; // Friction
        
        // Remove dead particles
        if (particle.life <= 0.0f)
        {
            it = particles.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

//==============================================================================
// Public Interface

void RetroCanvasComponent::setCanvasState(const CanvasState& newState)
{
    canvasState = newState;
    geometryNeedsUpdate = true;
    repaint();
}

void RetroCanvasComponent::clearCanvas()
{
    if (commandTarget)
    {
        Command cmd(PaintCommandID::ClearCanvas);
        commandTarget(cmd);
    }
    
    particles.clear();
    repaint();
}

void RetroCanvasComponent::resetView()
{
    canvasState.zoomLevel = 1.0f;
    canvasState.scrollX = 0.0f;
    canvasState.scrollY = 0.0f;
    geometryNeedsUpdate = true;
    repaint();
}

void RetroCanvasComponent::setBrushType(BrushType type)
{
    currentBrushType = type;
    canvasState.currentBrush = type;
    
    // Set brush-specific colors and paint modes
    switch (type)
    {
        case BrushType::PaintBrush:
            brushColor = CanvasColors::PAINT_BLUE;
            canvasState.paintMode = ColorChannel::Brightness;
            break;
        case BrushType::SineBrush:
            brushColor = CanvasColors::PAINT_BLUE;
            canvasState.paintMode = ColorChannel::Hue;
            break;
        case BrushType::HarmonicBrush:
            brushColor = CanvasColors::BRIGHT_BLUE;
            canvasState.paintMode = ColorChannel::Saturation;
            break;
        case BrushType::NoiseBrush:
            brushColor = CanvasColors::VIBRANT_RED;
            canvasState.paintMode = ColorChannel::Red;
            break;
        case BrushType::SampleBrush:
            brushColor = CanvasColors::ROYAL_BLUE;
            canvasState.paintMode = ColorChannel::Blue;
            break;
        case BrushType::GranularPen:
            brushColor = CanvasColors::PURPLE;
            canvasState.paintMode = ColorChannel::Green;
            break;
        case BrushType::CDPMorph:
            brushColor = CanvasColors::BRIGHT_ORANGE;
            canvasState.paintMode = ColorChannel::Yellow;
            break;
        case BrushType::SmearBrush:
            brushColor = CanvasColors::EMERALD_GREEN;
            canvasState.paintMode = ColorChannel::Hue;
            break;
        case BrushType::SprayBrush:
            brushColor = CanvasColors::BRIGHT_ORANGE.withAlpha(0.7f);
            canvasState.paintMode = ColorChannel::Brightness;
            break;
    }
    
    geometryNeedsUpdate = true;  // Trigger layout update for room change
    repaint();
}

void RetroCanvasComponent::setPerformanceInfo(float cpuLoad, int activeOscillators, float latency)
{
    currentCPULoad = cpuLoad;
    currentActiveOscillators = activeOscillators;
    currentLatency = latency;
    
    // Only repaint status bar area for performance
    auto geom = calculateGeometry();
    repaint(geom.statusBar);
}

//==============================================================================
// New Public Interface for MetaSynth Features
//==============================================================================

void RetroCanvasComponent::setImmersiveMode(bool immersive)
{
    canvasState.immersiveMode = immersive;
    geometryNeedsUpdate = true;
    repaint();
}

void RetroCanvasComponent::setMetaSynthColoring(bool enabled)
{
    canvasState.metaSynthColoring = enabled;
    repaint();
}

void RetroCanvasComponent::setPaintMode(ColorChannel mode)
{
    canvasState.paintMode = mode;
    repaint();
}

void RetroCanvasComponent::toggleSpectralOverlay()
{
    canvasState.showSpectralOverlay = !canvasState.showSpectralOverlay;
    repaint();
}

void RetroCanvasComponent::toggleRulers()
{
    canvasState.showRulers = !canvasState.showRulers;
    geometryNeedsUpdate = true;
    repaint();
}

float RetroCanvasComponent::getCanvasOccupancy() const
{
    return cachedGeometry.canvasOccupancy;
}

void RetroCanvasComponent::setPaintIntensity(float intensity)
{
    canvasState.paintIntensity = juce::jlimit(0.0f, 2.0f, intensity);
}

void RetroCanvasComponent::setHarmonicSpread(float spread)
{
    canvasState.harmonicSpread = juce::jlimit(0.0f, 1.0f, spread);
}

//==============================================================================
// MetaSynth-Style Spectral Room Rendering
//==============================================================================

void RetroCanvasComponent::drawSpectralRoom(juce::Graphics& g, const CanvasGeometry& geom) const
{
    // Get room color based on current brush type (MetaSynth-style rooms)
    juce::Colour roomColor = getRoomColorForBrush(canvasState.currentBrush);
    
    // Breathing animation for room atmosphere
    float roomPulse = std::sin(animationTime * 0.5f) * 0.05f + 0.95f;
    juce::Colour animatedRoomColor = roomColor.withMultipliedBrightness(roomPulse);
    
    // Fill background with spectral room color
    g.fillAll(animatedRoomColor.darker(0.8f));
    
    // Add subtle room texture based on brush type
    drawRoomTexture(g, geom, canvasState.currentBrush);
}

void RetroCanvasComponent::drawCanvasBackground(juce::Graphics& g, const CanvasGeometry& geom) const
{
    // Canvas background adapts to spectral room
    juce::Colour roomColor = getRoomColorForBrush(canvasState.currentBrush);
    juce::Colour canvasColor = roomColor.contrasting(0.9f).withAlpha(0.95f);
    
    g.setColour(canvasColor);
    g.fillRect(geom.canvasArea);
}

void RetroCanvasComponent::drawRoomTexture(juce::Graphics& g, const CanvasGeometry& geom, BrushType brushType) const
{
    juce::Colour roomColor = getRoomColorForBrush(brushType);
    
    switch (brushType)
    {
        case BrushType::SineBrush:
            // Sine waves - smooth flowing lines
            g.setColour(roomColor.withAlpha(0.1f));
            for (int i = 0; i < 5; ++i)
            {
                juce::Path wavePath;
                wavePath.startNewSubPath(0, geom.canvasArea.getCentreY() + std::sin(animationTime + i) * 20);
                for (float x = 0; x < getWidth(); x += 10)
                {
                    float y = geom.canvasArea.getCentreY() + std::sin((x / 100.0f) + animationTime + i) * 30;
                    wavePath.lineTo(x, y);
                }
                g.strokePath(wavePath, juce::PathStrokeType(1.0f));
            }
            break;
            
        case BrushType::HarmonicBrush:
            // Harmonic series - radiating circles
            {
                auto center = geom.canvasArea.getCentre();
                g.setColour(roomColor.withAlpha(0.08f));
                for (int i = 1; i <= 8; ++i)
                {
                    float radius = i * 40.0f + std::sin(animationTime * 2) * 10;
                    g.drawEllipse(center.x - radius, center.y - radius, radius * 2, radius * 2, 1.0f);
                }
            }
            break;
            
        case BrushType::NoiseBrush:
            // Noise - scattered particles
            {
                juce::Random random(static_cast<int>(animationTime * 100));
                g.setColour(roomColor.withAlpha(0.06f));
                for (int i = 0; i < 50; ++i)
                {
                    float x = random.nextFloat() * getWidth();
                    float y = random.nextFloat() * getHeight();
                    g.fillEllipse(x - 1, y - 1, 2, 2);
                }
            }
            break;
            
        case BrushType::SampleBrush:
            // Sample blocks - digital rectangles
            g.setColour(roomColor.withAlpha(0.1f));
            for (int i = 0; i < 20; ++i)
            {
                float x = (i % 5) * (getWidth() / 5.0f);
                float y = (i / 5) * (getHeight() / 4.0f);
                float size = 15 + std::sin(animationTime + i) * 5;
                g.drawRect(x, y, size, size, 1.0f);
            }
            break;
            
        case BrushType::GranularPen:
            // Granular - clustered dots
            {
                auto center = geom.canvasArea.getCentre();
                g.setColour(roomColor.withAlpha(0.12f));
                for (int i = 0; i < 100; ++i)
                {
                    float angle = (i / 100.0f) * juce::MathConstants<float>::twoPi;
                    float radius = 50 + std::sin(animationTime + angle * 3) * 30;
                    float x = center.x + std::cos(angle) * radius;
                    float y = center.y + std::sin(angle) * radius;
                    g.fillEllipse(x - 1, y - 1, 2, 2);
                }
            }
            break;
            
        case BrushType::CDPMorph:
            // CDP Morph - morphing geometric patterns
            {
                auto center = geom.canvasArea.getCentre();
                g.setColour(roomColor.withAlpha(0.15f));
                
                // Morphing between triangle and hexagon
                float morph = (std::sin(animationTime) + 1.0f) * 0.5f; // 0-1
                int numSides = static_cast<int>(3 + morph * 3); // 3-6 sides
                
                juce::Path morphPath;
                for (int i = 0; i < numSides; ++i)
                {
                    float angle = (i / static_cast<float>(numSides)) * juce::MathConstants<float>::twoPi;
                    float x = center.x + std::cos(angle) * 60;
                    float y = center.y + std::sin(angle) * 60;
                    
                    if (i == 0)
                        morphPath.startNewSubPath(x, y);
                    else
                        morphPath.lineTo(x, y);
                }
                morphPath.closeSubPath();
                g.strokePath(morphPath, juce::PathStrokeType(2.0f));
            }
            break;
    }
}

juce::Colour RetroCanvasComponent::getRoomColorForBrush(BrushType brushType) const
{
    switch (brushType)
    {
        case BrushType::SineBrush:      return juce::Colour(0xFF1A2A4A); // Blue room
        case BrushType::HarmonicBrush:  return juce::Colour(0xFF2A1A4A); // Purple room  
        case BrushType::NoiseBrush:     return juce::Colour(0xFF4A2A1A); // Orange room
        case BrushType::SampleBrush:    return juce::Colour(0xFF1A4A2A); // Green room
        case BrushType::GranularPen:    return juce::Colour(0xFF4A1A2A); // Red room
        case BrushType::CDPMorph:       return juce::Colour(0xFF4A2A4A); // Magenta room
        default:                        return juce::Colour(0xFF0A0A0A); // Dark default
    }
}

//==============================================================================
// MetaSynth Color Mapping System
//==============================================================================

juce::Colour RetroCanvasComponent::getMetaSynthColor(ColorChannel channel, float intensity, float frequency) const
{
    // MetaSynth-style color mapping based on channel and audio parameters
    intensity = juce::jlimit(0.0f, 1.0f, intensity);
    
    // Frequency-based hue shift (low = red, high = blue)
    const float freqNorm = (std::log2(frequency) - std::log2(canvasState.minFreq)) / 
                          (std::log2(canvasState.maxFreq) - std::log2(canvasState.minFreq));
    
    switch (channel)
    {
        case ColorChannel::Red:
            // Left channel - Red with intensity as brightness
            return juce::Colour(intensity, 0.0f, 0.0f, 1.0f);
            
        case ColorChannel::Green:
            // Right channel - Green with intensity as brightness  
            return juce::Colour(0.0f, intensity, 0.0f, 1.0f);
            
        case ColorChannel::Blue:
            // Phase/spectral data - Blue with frequency mapping
            return juce::Colour(0.0f, 0.0f, intensity * (0.5f + freqNorm * 0.5f), 1.0f);
            
        case ColorChannel::Yellow:
            // Center channel (L+R) - Yellow with intensity
            return juce::Colour(intensity, intensity, 0.0f, 1.0f);
            
        case ColorChannel::Brightness:
            // Overall amplitude - White with intensity as brightness
            return juce::Colour(intensity, intensity, intensity, 1.0f);
            
        case ColorChannel::Hue:
            // Frequency-based hue cycling
            return juce::Colour::fromHSV(freqNorm, 1.0f, intensity, 1.0f);
            
        case ColorChannel::Saturation:
            // Bandwidth mapping - Saturation based on harmonic content
            return juce::Colour::fromHSV(freqNorm, intensity, 1.0f, 1.0f);
            
        default:
            return CanvasColors::PAINT_BLUE.withBrightness(intensity);
    }
}

void RetroCanvasComponent::drawRealTimeSpectralOverlay(juce::Graphics& g, const CanvasGeometry& geom) const
{
    // MetaSynth-accurate real-time spectral analysis overlay
    g.setOpacity(0.8f);
    
    // Connect to audio processor for actual spectral data
    if (processor && processor->getSpectralSynthEngine())
    {
        drawLiveSpectralData(g, geom);
    }
    else
    {
        // Fallback: Draw animated spectral representation for visual development
        drawAnimatedSpectralPreview(g, geom);
    }
    
    // Additional MetaSynth-style spectral grid
    drawSpectralFrequencyBins(g, geom);
}

void RetroCanvasComponent::drawLiveSpectralData(juce::Graphics& g, const CanvasGeometry& geom) const
{
    // Get real spectral data from SpectralSynthEngine
    auto* spectralEngine = processor->getSpectralSynthEngine();
    const int activeOscillators = spectralEngine->getActiveOscillatorCount();
    
    if (activeOscillators > 0)
    {
        // Draw active oscillators as spectral bars
        const float barWidth = geom.canvasArea.getWidth() / 128.0f;  // 128 frequency bins
        
        for (int i = 0; i < activeOscillators && i < 64; ++i)  // Limit for performance
        {
            // Get oscillator data (this would need to be implemented in SpectralSynthEngine)
            // const auto oscData = spectralEngine->getOscillatorData(i);
            
            // For now, simulate oscillator visualization
            const float freq = 80.0f + (i * 100.0f);  // Spread across frequency range
            const float freqNorm = (freq - canvasState.minFreq) / (canvasState.maxFreq - canvasState.minFreq);
            const float amplitude = 0.3f + (std::sin(animationTime + i) * 0.2f);  // Animated amplitude
            
            if (freqNorm >= 0.0f && freqNorm <= 1.0f)
            {
                const float yPos = geom.canvasArea.getY() + (1.0f - freqNorm) * geom.canvasArea.getHeight();
                const float barHeight = amplitude * 30.0f;  // Scale for visibility
                
                // MetaSynth-style frequency color mapping
                juce::Colour barColor = juce::Colour::fromHSV(freqNorm, 0.8f, 1.0f, amplitude);
                g.setColour(barColor);
                g.fillRoundedRectangle(geom.canvasArea.getX() + (i * barWidth), 
                                     yPos - barHeight * 0.5f, 
                                     barWidth - 1, 
                                     barHeight, 2.0f);
            }
        }
    }
}

void RetroCanvasComponent::drawAnimatedSpectralPreview(juce::Graphics& g, const CanvasGeometry& geom) const
{
    // Animated spectral representation for development/preview
    const int numBins = 64;
    const float binWidth = static_cast<float>(geom.canvasArea.getWidth()) / numBins;
    
    for (int i = 0; i < numBins; ++i)
    {
        // Simulate spectral data with animation
        const float time = animationTime + i * 0.1f;
        const float magnitude = (std::sin(time * 2.0f) + 1.0f) * 0.3f + 0.1f;
        
        const int x = geom.canvasArea.getX() + static_cast<int>(i * binWidth);
        const int height = static_cast<int>(magnitude * geom.canvasArea.getHeight());
        const int y = geom.canvasArea.getBottom() - height;
        
        // Color based on frequency
        const float freqNorm = static_cast<float>(i) / numBins;
        juce::Colour specColor = getMetaSynthColor(ColorChannel::Hue, magnitude, 
                                                 canvasState.minFreq + freqNorm * (canvasState.maxFreq - canvasState.minFreq));
        
        g.setColour(specColor.withAlpha(0.4f));
        g.fillRect(x, y, static_cast<int>(binWidth), height);
    }
}

void RetroCanvasComponent::drawSpectralFrequencyBins(juce::Graphics& g, const CanvasGeometry& geom) const
{
    // MetaSynth-style spectral frequency bin markers
    if (!canvasState.showGrid) return;
    
    g.setColour(CanvasColors::GRID_LIGHT.withAlpha(0.3f));
    
    // Draw key frequency markers (MetaSynth-style)
    const std::vector<float> keyFrequencies = {
        80.0f, 160.0f, 320.0f, 640.0f, 1280.0f, 2560.0f, 5120.0f  // Octaves
    };
    
    for (float freq : keyFrequencies)
    {
        if (freq >= canvasState.minFreq && freq <= canvasState.maxFreq)
        {
            const float freqNorm = (freq - canvasState.minFreq) / (canvasState.maxFreq - canvasState.minFreq);
            const float yPos = geom.canvasArea.getY() + (1.0f - freqNorm) * geom.canvasArea.getHeight();
            
            // Draw subtle frequency line
            g.drawHorizontalLine(static_cast<int>(yPos), 
                                geom.canvasArea.getX(), 
                                geom.canvasArea.getRight());
            
            // Add frequency label
            const juce::String freqLabel = (freq < 1000.0f) ? 
                juce::String(static_cast<int>(freq)) + "Hz" :
                juce::String(freq / 1000.0f, 1) + "kHz";
                
            g.setFont(10.0f);
            g.drawText(freqLabel, 
                      geom.canvasArea.getX() + 5, 
                      static_cast<int>(yPos) - 6, 
                      60, 12, 
                      juce::Justification::left);
        }
    }
    
    // Draw spectral bins for fine-grained frequency reference
    const int numSpectralBins = 32;
    const float binHeight = static_cast<float>(geom.canvasArea.getHeight()) / numSpectralBins;
    
    g.setColour(CanvasColors::BRIGHT_BLUE.withAlpha(0.1f));
    for (int i = 0; i < numSpectralBins; ++i)
    {
        const float yPos = geom.canvasArea.getY() + i * binHeight;
        g.drawHorizontalLine(static_cast<int>(yPos), 
                           geom.canvasArea.getX(), 
                           geom.canvasArea.getRight());
    }
}

void RetroCanvasComponent::drawContextualUI(juce::Graphics& g, const CanvasGeometry& geom) const
{
    // Draw contextual UI elements only when needed
    if (!canvasState.immersiveMode)
    {
        // Show rulers when mouse is near edges
        if (mousePosition.x < geom.canvasArea.getX() + 50 && canvasState.showRulers)
        {
            drawFrequencyGrid(g, geom);
        }
        
        if (mousePosition.y > geom.canvasArea.getBottom() - 50 && canvasState.showRulers)
        {
            drawTimeGrid(g, geom);
        }
    }
}

void RetroCanvasComponent::drawFloatingToolPalette(juce::Graphics& g, const CanvasGeometry& geom) const
{
    // Floating tool palette - non-intrusive, contextual
    g.setColour(CanvasColors::CANVAS_IVORY.withAlpha(0.9f));
    g.fillRoundedRectangle(geom.toolPalette.toFloat(), 4.0f);
    
    g.setColour(CanvasColors::EMERALD_GREEN);
    g.drawRoundedRectangle(geom.toolPalette.toFloat(), 4.0f, 1.0f);
    
    // Draw brush type indicators
    const int buttonSize = 20;
    const int padding = 5;
    int yPos = geom.toolPalette.getY() + padding;
    
    const std::vector<BrushType> brushTypes = {
        BrushType::PaintBrush, BrushType::SineBrush, BrushType::HarmonicBrush,
        BrushType::NoiseBrush, BrushType::GranularPen, BrushType::CDPMorph
    };
    
    for (const auto& brushType : brushTypes)
    {
        juce::Rectangle<int> buttonArea(geom.toolPalette.getX() + padding, yPos, buttonSize, buttonSize);
        
        // Highlight current brush
        if (brushType == canvasState.currentBrush)
        {
            g.setColour(CanvasColors::BRIGHT_ORANGE.withAlpha(0.8f));
            g.fillRoundedRectangle(buttonArea.toFloat(), 2.0f);
        }
        
        // Draw brush icon/color
        g.setColour(getRoomColorForBrush(brushType));
        g.fillRoundedRectangle(buttonArea.reduced(3).toFloat(), 1.0f);
        
        yPos += buttonSize + padding;
    }
    
    // Paint mode indicator
    g.setFont(createTerminalFont(8.0f));
    g.setColour(CanvasColors::DARK_BLUE_GRAY);
    juce::String paintModeText = "MODE: ";
    
    switch (canvasState.paintMode)
    {
        case ColorChannel::Red: paintModeText += "LEFT"; break;
        case ColorChannel::Green: paintModeText += "RIGHT"; break;
        case ColorChannel::Yellow: paintModeText += "CENTER"; break;
        case ColorChannel::Brightness: paintModeText += "BRIGHT"; break;
        default: paintModeText += "SPECTRAL"; break;
    }
    
    g.drawText(paintModeText, geom.toolPalette.getX() + 5, 
               geom.toolPalette.getBottom() - 25, 90, 20, juce::Justification::left);
}

void RetroCanvasComponent::handlePaintGesture(const juce::MouseEvent& e, bool isDrag)
{
    const auto geom = calculateGeometry();
    const float frequency = screenYToFrequency(e.getPosition().y, geom);
    const float time = screenXToTime(e.getPosition().x, geom);
    const float pressure = e.mods.isLeftButtonDown() ? 1.0f : 0.5f;
    
    // Calculate pan position (-1.0 to 1.0) from horizontal position
    const float pan = (time / canvasState.timeRange) * 2.0f - 1.0f;
    
    // Modifier key combinations for advanced gestures
    if (e.mods.isShiftDown())
    {
        // Shift: Horizontal smearing/frequency shifting
        canvasState.paintMode = ColorChannel::Hue;
    }
    else if (e.mods.isCtrlDown())
    {
        // Ctrl: Amplitude painting (brightness)
        canvasState.paintMode = ColorChannel::Brightness;
    }
    else if (e.mods.isAltDown())
    {
        // Alt: Stereo positioning (red/green channels)
        canvasState.paintMode = pan < 0 ? ColorChannel::Red : ColorChannel::Green;
    }
    
    // Apply MetaSynth color mapping to the paint stroke
    juce::Colour gestureColor = getMetaSynthColor(canvasState.paintMode, pressure, frequency);
    
    if (paintEngine)
    {
        // TODO: Send enhanced paint data to engine with MetaSynth parameters
        // paintEngine->applyMetaSynthPaint(frequency, pressure, pan, canvasState.paintMode);
    }
}

void RetroCanvasComponent::processFrequencyGesture(float frequency, float amplitude, float pan)
{
    // Process paint gestures into audio parameters
    // This is where paint becomes sound in real-time
    
    if (processor)
    {
        // Map paint position to audio parameters
        const float normalizedFreq = (frequency - canvasState.minFreq) / (canvasState.maxFreq - canvasState.minFreq);
        const float clampedPan = juce::jlimit(-1.0f, 1.0f, pan);
        
        // TODO: Connect to processor's paint-to-audio engine
        // processor->setPaintParameters(normalizedFreq, amplitude, clampedPan);
    }
}

void RetroCanvasComponent::addMetaSynthParticle(juce::Point<float> position, ColorChannel channel, float intensity)
{
    const auto geom = calculateGeometry();
    const float frequency = screenYToFrequency(static_cast<int>(position.y), geom);
    juce::Colour particleColor = getMetaSynthColor(channel, intensity, frequency);
    
    Particle particle;
    particle.position = position;
    particle.velocity = {
        (juce::Random::getSystemRandom().nextFloat() - 0.5f) * 15.0f,
        (juce::Random::getSystemRandom().nextFloat() - 0.5f) * 15.0f
    };
    particle.color = particleColor;
    particle.life = 1.0f;
    particle.size = 1.0f + intensity * 4.0f;
    
    particles.push_back(particle);
    
    // Limit particle count for performance
    if (particles.size() > 150)
    {
        particles.erase(particles.begin());
    }
}

void RetroCanvasComponent::addMetaSynthParticleFast(juce::Point<float> position, ColorChannel channel, float intensity, float frequency)
{
    // SUB-5MS OPTIMIZATION: Skip coordinate conversion, use provided frequency
    juce::Colour particleColor = getMetaSynthColor(channel, intensity, frequency);
    
    Particle particle;
    particle.position = position;
    // FAST RANDOM: Use simple bit manipulation instead of Random class
    static uint32_t fastRand = 1;
    fastRand = (fastRand * 1664525u) + 1013904223u;  // Linear congruential generator
    const float randX = ((fastRand >> 16) & 0xFFFF) / 65535.0f - 0.5f;
    fastRand = (fastRand * 1664525u) + 1013904223u;
    const float randY = ((fastRand >> 16) & 0xFFFF) / 65535.0f - 0.5f;
    
    particle.velocity = { randX * 15.0f, randY * 15.0f };
    particle.color = particleColor;
    particle.life = 1.0f;
    particle.size = 1.0f + intensity * 4.0f;
    
    particles.push_back(particle);
    
    // FAST CLEANUP: Limit particle count with simple check
    if (particles.size() > 120) {  // Slightly lower limit for better performance
        particles.pop_front();
    }
}

inline float RetroCanvasComponent::freqNormToFrequency(float freqNorm) const noexcept
{
    // FAST FREQUENCY CONVERSION: Pre-calculated for minimal latency
    return canvasState.minFreq * std::pow(canvasState.maxFreq / canvasState.minFreq, freqNorm);
}

void RetroCanvasComponent::updateSpectralVisualization()
{
    // Update real-time spectral analysis
    // TODO: Connect to audio engine's FFT analysis
    // This should update the spectral overlay with current audio content
}

//==============================================================================
// RT-SAFE MASK PAINTING IMPLEMENTATION
//==============================================================================

void RetroCanvasComponent::paintMaskAtPosition(float timeNorm, float freqNorm, float pressure)
{
    if (!processor) return;
    
    // Get mask snapshot from spectral synth engine
    auto& maskSnapshot = processor->getSpectralSynthEngine().getMaskSnapshot();
    auto* workBuffer = maskSnapshot.getWorkBuffer();
    if (!workBuffer) return;
    
    // Determine mask value based on brush type
    float maskValue = 1.0f; // Default no masking
    if (canvasState.currentBrush == BrushType::MaskBrush)
    {
        // Erase mode: attenuate based on pressure (0.0 = full attenuation, 1.0 = no change)
        maskValue = 1.0f - (pressure * 0.8f); // Max 80% attenuation for safety
    }
    else if (canvasState.currentBrush == BrushType::EraseBrush)
    {
        // Restore mode: always restore to full (1.0)
        maskValue = 1.0f;
    }
    
    // Calculate brush size based on current settings
    const float brushRadius = brushSize * 0.02f; // Scale to reasonable size
    
    // Paint circular brush at position
    maskSnapshot.paintCircle(timeNorm, freqNorm, brushRadius, maskValue);
    
    // Visual feedback - add particle at mouse position
    const auto geom = calculateGeometry();
    const int mouseX = static_cast<int>(timeNorm * geom.canvasArea.getWidth()) + geom.canvasArea.getX();
    const int mouseY = static_cast<int>((1.0f - freqNorm) * geom.canvasArea.getHeight()) + geom.canvasArea.getY();
    
    juce::Colour feedbackColor = (canvasState.currentBrush == BrushType::MaskBrush) ? 
                                juce::Colours::red.withAlpha(0.6f) : 
                                juce::Colours::green.withAlpha(0.6f);
    
    addParticleAt(juce::Point<float>(mouseX, mouseY), feedbackColor);
}

void RetroCanvasComponent::commitMaskChanges()
{
    if (!processor) return;
    
    // Atomically commit the work buffer to the audio thread
    auto& maskSnapshot = processor->getSpectralSynthEngine().getMaskSnapshot();
    maskSnapshot.commitWorkBuffer();
    
    DBG("Mask changes committed to audio thread");
    
    // Trigger repaint to show updated visualization
    repaint();
}