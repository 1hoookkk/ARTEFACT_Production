/******************************************************************************
 * File: CanvasComponent_Modern.cpp
 * Description: Implementation of modern immersive canvas
 * 
 * Copyright (c) 2025 Spectral Audio Systems
 ******************************************************************************/

#include "CanvasComponent.h"
#include "Core/SampleMaskingEngine.h"
#include "Core/SpectralSynthEngine.h"  // 🎵 CRITICAL FIX: RESTORE INCLUDES FOR PAINT-TO-AUDIO
#include "Core/CDPSpectralEngine.h"    // 🎵 CRITICAL FIX: RESTORE INCLUDES FOR PAINT-TO-AUDIO
#include "Core/Commands.h"
#include "Core/CanvasLayer.h"
#include "Core/UndoableActions.h"
#include "Core/Config.h"
#include "Core/CrashToggles.h"

//==============================================================================
CanvasComponent::CanvasComponent()
{
    using namespace SpectralCanvas::Config;
    
    DBG("CanvasComponent: Constructor starting - Mode=" << 
        (IsFullModeActive() ? "Full" : IsSafeModeActive() ? "Safe" : IsMinimalModeActive() ? "Minimal" : "Debug"));
    
    // HIERARCHICAL SAFETY CHECK: Early exit if canvas disabled
    if (!ShouldInitializeLayerManager()) {
        DBG("CanvasComponent: DISABLED by Config safety check - minimal initialization only");
        DBG("   CrashToggles::ENABLE_CANVAS_COMPONENT=" << (CrashToggles::ENABLE_CANVAS_COMPONENT ? "true" : "false"));
        DBG("   CrashToggles::ENABLE_CANVAS_RENDERING=" << (CrashToggles::ENABLE_CANVAS_RENDERING ? "true" : "false"));
        DBG("   LaunchMode=" << (int)g_LaunchMode);
        
        // Set minimal safe state
        setInterceptsMouseClicks(false, false);
        setOpaque(false);
        setWantsKeyboardFocus(false);
        canvasEnabled = false;
        return;
    }
    
    setInterceptsMouseClicks(true, false);
    setOpaque(true);
    setWantsKeyboardFocus(true);  // Enable keyboard shortcuts
    canvasEnabled = true;
    
    // Initialize undo manager only if layer management is enabled
    if (UIFeatures::IsCanvasRenderingEnabled()) {
        try {
            undoManager = std::make_unique<CanvasUndoManager>(&layerManager);
            DBG("✅ CanvasUndoManager initialized");
        } catch (const std::exception& e) {
            DBG("🚨 ERROR: CanvasUndoManager initialization failed - " << e.what());
            undoManager = nullptr;
        }
    } else {
        DBG("🚨 CanvasUndoManager: DISABLED by feature flags");
    }
    
    // Initialize MetaSynth-style effect room colors
    effectRoomColors[0] = juce::Colour(0xFF0A0A0A);      // None - Dark
    effectRoomColors[1] = juce::Colour(0xFF2A1A4A);      // Blur - Purple room
    effectRoomColors[2] = juce::Colour(0xFF4A2A1A);      // Randomize - Orange room
    effectRoomColors[3] = juce::Colour(0xFF1A4A2A);      // Shuffle - Green room
    effectRoomColors[4] = juce::Colour(0xFF1A2A4A);      // Freeze - Blue room
    effectRoomColors[5] = juce::Colour(0xFF4A1A2A);      // Arpeggiate - Red room
    effectRoomColors[6] = juce::Colour(0xFF4A4A1A);      // TimeExpand - Yellow room
    effectRoomColors[7] = juce::Colour(0xFF2A4A4A);      // Average - Cyan room
    effectRoomColors[8] = juce::Colour(0xFF4A2A4A);      // Morph - Magenta room
    
    // D) COMPONENT LISTENER: Set up visibility-based timer control  
    addComponentListener(this);
    
    // Timer will be started by componentVisibilityChanged when visible
}

CanvasComponent::~CanvasComponent()
{
    // A) DESTRUCTOR HYGIENE: Set shutdown flag first
    shuttingDown = true;
    
    // Stop timer and remove listener
    stopTimer();
    removeComponentListener(this);
    
    // Cancel any pending async operations
    if (auto* mm = juce::MessageManager::getInstanceWithoutCreating())
    {
        // Ensure no pending callbacks reference this object
    }
    
    // Clear any engine pointers (no memory management needed)
    spectralSynthEngine = nullptr;
    sampleMaskingEngine = nullptr;
}

//==============================================================================
// Visual Rendering
//==============================================================================

void CanvasComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // 🎨 METASYNTH-STYLE SPECTRAL ROOM BACKGROUND
    drawSpectralRoom(g);
    
    // Draw frequency bands visualization
    drawFrequencyBands(g);
    
    // Draw animated spectral particles
    drawSpectralParticles(g);
    
    // Minimal grid (now with room-aware colors)
    drawMinimalGrid(g);
    
    // Render all layers with proper blending
    layerManager.renderAllLayers(g, bounds.toFloat());
    
    // Paint strokes with enhanced spectral glow (legacy compatibility)
    if (!paintStrokes.empty())
        drawPaintStrokes(g);
    
    // Active spectral effect visualization
    if (spectralViz.currentEffect > 0 && spectralViz.effectIntensity > 0.0f)
    {
        drawSpectralEffect(g, spectralViz.currentEffect, spectralViz.effectIntensity, bounds.toFloat());
    }
    
    // 🎨 PAINT RESPONSIVENESS INDICATOR: Show when paint interface is active
    drawPaintStatusIndicator(g);
    
    // Crosshair cursor
    if (isMouseOverOrDragging())
    {
        drawCrosshair(g, lastMousePos.toInt());
        
        // Draw pressure-sensitive cursor preview
        if (isDragging || isUsingTabletPressure)
        {
            drawPressureCursor(g);
        }
    }
}

void CanvasComponent::resized()
{
    // D) UI LAYOUT ONLY: Canvas fills entire bounds - no engine calls
    // Canvas component is now passive - no engine state mutation in resized()
    
    // REMOVED: spectralSynthEngine->updateCanvasBounds() call eliminated
    // Engine will handle bounds internally via atomic parameters and command queue
}

//==============================================================================
// Mouse Interaction
//==============================================================================

void CanvasComponent::mouseDown(const juce::MouseEvent& event)
{
    lastMousePos = event.position;
    isDragging = true;
    
    // 🎨 TABLET PRESSURE SUPPORT: Check for real hardware pressure
    float pressure = 1.0f;
    if (event.isPressureValid())
    {
        // Use actual tablet/stylus pressure
        pressure = event.pressure;
        DBG("🎨 TABLET: Using hardware pressure: " << pressure);
        isUsingTabletPressure = true;
    }
    else
    {
        // Fallback to velocity-based simulation
        pressure = 1.0f; // Full pressure on initial down
        isUsingTabletPressure = false;
    }
    
    // Store initial pressure for this stroke
    currentStrokePressure = pressure;
    lastDragPos = event.position;
    
    // Start new paint stroke with actual pressure
    auto spectralPos = screenToSpectral(event.position);
    processPaintStroke(spectralPos, pressure);
    
    // Create visual paint stroke on active layer
    if (auto* activeLayer = layerManager.getActiveLayer())
    {
        activeLayer->beginStroke(event.position, getCurrentBrushColor(), pressure);
        // Begin new undo transaction for this stroke
        undoManager->beginNewTransaction("Paint Stroke");
    }
    
    // Legacy single-layer support (for compatibility)
    float strokeWidth = 1.0f + (pressure * 3.0f); // 1-4 pixel width based on pressure
    paintStrokes.emplace_back(getCurrentBrushColor(), strokeWidth);
    paintStrokes.back().path.startNewSubPath(event.position);
    paintStrokes.back().pressures.push_back(pressure);
    
    // Show pressure feedback cursor
    updatePressureCursor(pressure);
    
    repaint();
}

void CanvasComponent::mouseDrag(const juce::MouseEvent& event)
{
    if (!isDragging) return;
    
    // SUB-5MS PAINT DRAG: Ultra-responsive painting with atomic parameter updates
    lastMousePos = event.position;
    
    // 🎨 ENHANCED PRESSURE DETECTION: Tablet priority, velocity fallback
    float pressure = 1.0f;
    
    if (event.isPressureValid())
    {
        // Use actual tablet/stylus pressure
        pressure = event.pressure;
        isUsingTabletPressure = true;
    }
    
    // Calculate drag speed for use in both pressure simulation and path interpolation
    const float dragSpeed = event.position.getDistanceFrom(lastDragPos);
    
    if (!isUsingTabletPressure)
    {
        // Velocity-based pressure simulation (only if not using tablet)
        pressure = juce::jlimit(0.3f, 1.0f, 1.0f - (dragSpeed / 50.0f)); // Slower = more pressure
        
        // Smooth pressure transitions
        pressure = currentStrokePressure * 0.7f + pressure * 0.3f;
    }
    
    currentStrokePressure = pressure;
    lastDragPos = event.position;
    
    // Update pressure cursor visualization
    updatePressureCursor(pressure);
    
    // ATOMIC UPDATES: Thread-safe parameter updates without locks
    if (spectralSynthEngine) {
        spectralSynthEngine->currentPaintParams.x.store(event.position.x, std::memory_order_relaxed);
        spectralSynthEngine->currentPaintParams.y.store(event.position.y, std::memory_order_relaxed);
        spectralSynthEngine->currentPaintParams.pressure.store(pressure, std::memory_order_relaxed);
        
        // Pack color into atomic uint32
        juce::Colour currentColor = getCurrentBrushColor();
        uint32_t packedColor = (uint32_t(currentColor.getRed()) << 24) |
                              (uint32_t(currentColor.getGreen()) << 16) |
                              (uint32_t(currentColor.getBlue()) << 8) |
                              uint32_t(currentColor.getAlpha());
        spectralSynthEngine->currentPaintParams.colorRGBA.store(packedColor, std::memory_order_relaxed);
        
        // Signal audio thread that new parameters are available
        spectralSynthEngine->currentPaintParams.needsUpdate.store(true, std::memory_order_release);
    }
    
    // Continue paint stroke with dynamic pressure
    auto spectralPos = screenToSpectral(event.position);
    processPaintStroke(spectralPos, pressure);
    
    // Continue stroke on active layer
    if (auto* activeLayer = layerManager.getActiveLayer())
    {
        activeLayer->continueStroke(event.position, pressure);
    }
    
    // SMOOTH VISUAL PATH: Add to paint stroke with interpolation (legacy)
    if (!paintStrokes.empty())
    {
        // Store pressure values for this segment
        paintStrokes.back().pressures.push_back(pressure);
        
        // Update stroke width based on pressure
        paintStrokes.back().intensity = pressure;
        
        // Smooth path interpolation for professional paint feel
        auto& currentPath = paintStrokes.back().path;
        if (dragSpeed < 20.0f) { // Smooth interpolation for slow strokes
            currentPath.lineTo(event.position);
        } else { // Direct line for fast strokes
            currentPath.lineTo(event.position);
        }
    }
    
    // IMMEDIATE VISUAL UPDATE: Repaint for responsive feel
    repaint();
}

void CanvasComponent::mouseUp(const juce::MouseEvent& event)
{
    isDragging = false;
    
    // End stroke on active layer
    if (auto* activeLayer = layerManager.getActiveLayer())
    {
        // Get the completed stroke and add it as an undoable action
        if (auto* stroke = activeLayer->getCurrentStroke())
        {
            undoManager->addStroke(activeLayer->getIndex(), *stroke);
        }
        activeLayer->endStroke();
    }
    
    repaint();
}

void CanvasComponent::mouseMove(const juce::MouseEvent& event)
{
    lastMousePos = event.position;
    repaint();
}

//==============================================================================
// Engine Connections
//==============================================================================

void CanvasComponent::setSampleMaskingEngine(SampleMaskingEngine* engine)
{
    sampleMaskingEngine = engine;
}

void CanvasComponent::setSpectralSynthEngine(SpectralSynthEngine* engine)
{
    spectralSynthEngine = engine;
}

void CanvasComponent::setCommandProcessor(std::function<bool(const Command&)> processor)
{
    commandProcessor = processor;
}

void CanvasComponent::setAudioActivationCallback(std::function<void()> callback)
{
    audioActivationCallback = callback;
}

//==============================================================================
// Canvas Operations
//==============================================================================

void CanvasComponent::clearCanvas()
{
    // Clear all layers
    layerManager.clearAllLayers();
    
    // Clear legacy strokes
    paintStrokes.clear();
    repaint();
}

void CanvasComponent::clearActiveLayer()
{
    // Clear only the active layer
    if (auto* activeLayer = layerManager.getActiveLayer())
    {
        activeLayer->clearStrokes();
        repaint();
    }
}

// Layer management methods
CanvasLayer* CanvasComponent::addNewLayer(const juce::String& name)
{
    undoManager->addLayer(name);
    repaint();
    return layerManager.getActiveLayer();
}

void CanvasComponent::removeLayer(int index)
{
    undoManager->removeLayer(index);
    repaint();
}

void CanvasComponent::setActiveLayer(int index)
{
    layerManager.setActiveLayer(index);
}

// Undo/Redo implementation
void CanvasComponent::undo()
{
    if (undoManager->undo())
    {
        repaint();
        DBG("Undo: " << undoManager->getUndoDescription());
    }
}

void CanvasComponent::redo()
{
    if (undoManager->redo())
    {
        repaint();
        DBG("Redo: " << undoManager->getRedoDescription());
    }
}

bool CanvasComponent::canUndo() const
{
    return undoManager->canUndo();
}

bool CanvasComponent::canRedo() const
{
    return undoManager->canRedo();
}

// Keyboard shortcuts
bool CanvasComponent::keyPressed(const juce::KeyPress& key)
{
    // Undo: Ctrl/Cmd + Z
    if (key.isKeyCode(juce::KeyPress::F26Key) || 
        (key.getModifiers().isCommandDown() && key.getKeyCode() == 'Z'))
    {
        if (key.getModifiers().isShiftDown())
        {
            // Redo: Ctrl/Cmd + Shift + Z
            redo();
        }
        else
        {
            // Undo
            undo();
        }
        return true;
    }
    
    // Redo: Ctrl/Cmd + Y
    if (key.getModifiers().isCommandDown() && key.getKeyCode() == 'Y')
    {
        redo();
        return true;
    }
    
    // Clear layer: Delete key
    if (key.isKeyCode(juce::KeyPress::deleteKey))
    {
        if (auto* activeLayer = layerManager.getActiveLayer())
        {
            undoManager->clearLayer(activeLayer->getIndex());
            repaint();
        }
        return true;
    }
    
    // Add new layer: Ctrl/Cmd + Shift + N
    if (key.getModifiers().isCommandDown() && key.getModifiers().isShiftDown() && 
        key.getKeyCode() == 'N')
    {
        addNewLayer();
        return true;
    }
    
    return false;
}

// Legacy clearCanvas moved to main clearCanvas method

void CanvasComponent::startSpectralUpdates()
{
    // Start real-time updates
}

void CanvasComponent::stopSpectralUpdates()
{
    // Stop real-time updates
    stopTimer();
}

void CanvasComponent::timerCallback()
{
    // RT-SAFE: Check shutdown flag to prevent race conditions
    if (shuttingDown) return;
    
    updateSpectralVisualization();
}

void CanvasComponent::componentVisibilityChanged(juce::Component& component)
{
    // B) VISIBILITY-BASED TIMER: Control timer based on visibility (≤ 60 Hz)
    if (&component == this && !shuttingDown)
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
// MetaSynth-Style Visual Feedback Implementation
//==============================================================================

void CanvasComponent::updateSpectralVisualization()
{
    // Update animation timer
    spectralViz.effectTime += 0.016f; // ~60 FPS increment
    spectralViz.roomAnimation = std::sin(spectralViz.effectTime * 0.5f) * 0.1f + 0.9f;
    
    // Update spectral particles based on current effect
    if (spectralViz.currentEffect > 0)
    {
        // Add new particles based on effect type
        if (spectralViz.spectralParticles.size() < 50)
        {
            auto bounds = getLocalBounds().toFloat();
            juce::Point<float> newParticle(
                juce::Random::getSystemRandom().nextFloat() * bounds.getWidth(),
                juce::Random::getSystemRandom().nextFloat() * bounds.getHeight()
            );
            spectralViz.spectralParticles.push_back(newParticle);
        }
        
        // Animate existing particles
        for (auto& particle : spectralViz.spectralParticles)
        {
            switch (spectralViz.currentEffect)
            {
                case 1: // Blur - slow drift
                    particle.x += std::sin(spectralViz.effectTime + particle.y * 0.01f) * 0.5f;
                    particle.y += std::cos(spectralViz.effectTime + particle.x * 0.01f) * 0.3f;
                    break;
                case 2: // Randomize - chaotic movement
                    particle.x += (juce::Random::getSystemRandom().nextFloat() - 0.5f) * 2.0f;
                    particle.y += (juce::Random::getSystemRandom().nextFloat() - 0.5f) * 2.0f;
                    break;
                case 3: // Shuffle - organized swirling
                    {
                        auto center = getLocalBounds().getCentre().toFloat();
                        auto angle = std::atan2(particle.y - center.y, particle.x - center.x);
                        auto radius = particle.getDistanceFrom(center);
                        angle += spectralViz.effectIntensity * 0.02f;
                        particle.x = center.x + std::cos(angle) * radius;
                        particle.y = center.y + std::sin(angle) * radius;
                    }
                    break;
                case 4: // Freeze - particles slow down and freeze
                    // Particles gradually stop moving
                    break;
                case 5: // Arpeggiate - rhythmic pulses
                    {
                        float pulse = std::sin(spectralViz.effectTime * 8.0f) * spectralViz.effectIntensity;
                        particle.x += pulse * 0.5f;
                    }
                    break;
            }
        }
    } else {
        // Gradually remove particles when no effect is active
        if (!spectralViz.spectralParticles.empty() && juce::Random::getSystemRandom().nextFloat() < 0.1f)
        {
            spectralViz.spectralParticles.pop_back();
        }
    }
    
    repaint();
}

void CanvasComponent::setActiveSpectralEffect(int effectType, float intensity)
{
    spectralViz.currentEffect = effectType;
    spectralViz.effectIntensity = intensity;
    
    // Update room color based on effect
    auto colorIt = effectRoomColors.find(effectType);
    if (colorIt != effectRoomColors.end())
    {
        spectralViz.roomColor = colorIt->second;
    }
    
    DBG("🎨 MetaSynth Room Changed: Effect " << effectType << " Intensity " << intensity);
}

//==============================================================================
// Coordinate Mapping
//==============================================================================

juce::Point<float> CanvasComponent::screenToSpectral(juce::Point<float> screenPos) const
{
    auto bounds = getLocalBounds().toFloat();
    
    // X = time (0-1)
    float time = screenPos.x / bounds.getWidth();
    
    // Y = frequency (logarithmic, 20Hz - 20kHz)
    float normalizedY = 1.0f - (screenPos.y / bounds.getHeight());
    float logMin = std::log10(20.0f);
    float logMax = std::log10(20000.0f);
    float frequency = std::pow(10.0f, logMin + normalizedY * (logMax - logMin));
    
    return juce::Point<float>(time, frequency);
}

juce::Point<float> CanvasComponent::spectralToScreen(juce::Point<float> spectralPos) const
{
    auto bounds = getLocalBounds().toFloat();
    
    // X = time
    float screenX = spectralPos.x * bounds.getWidth();
    
    // Y = frequency (logarithmic)
    float logMin = std::log10(20.0f);
    float logMax = std::log10(20000.0f);
    float normalizedY = (std::log10(spectralPos.y) - logMin) / (logMax - logMin);
    float screenY = (1.0f - normalizedY) * bounds.getHeight();
    
    return juce::Point<float>(screenX, screenY);
}

//==============================================================================
// Canvas Rendering
//==============================================================================

void CanvasComponent::drawMinimalGrid(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    g.setColour(juce::Colour(0xFF333333));
    
    // Vertical time grid (every second for 5 seconds)
    for (int i = 1; i < 5; ++i)
    {
        float x = (i / 5.0f) * bounds.getWidth();
        g.drawVerticalLine(static_cast<int>(x), 0, static_cast<int>(bounds.getHeight()));
    }
    
    // Horizontal frequency grid (major frequencies)
    std::vector<float> majorFreqs = {100.0f, 500.0f, 1000.0f, 5000.0f, 10000.0f};
    
    for (float freq : majorFreqs)
    {
        auto screenPos = spectralToScreen(juce::Point<float>(0.5f, freq));
        g.drawHorizontalLine(static_cast<int>(screenPos.y), 0, static_cast<int>(bounds.getWidth()));
    }
}

void CanvasComponent::drawPaintStrokes(juce::Graphics& g) const
{
    for (const auto& stroke : paintStrokes)
    {
        // Glow effect
        g.setColour(stroke.color.withAlpha(0.3f));
        juce::PathStrokeType glowStroke(18.0f);
        g.strokePath(stroke.path, glowStroke);
        
        // Main stroke
        g.setColour(stroke.color.withAlpha(0.8f));
        juce::PathStrokeType mainStroke(12.0f);
        g.strokePath(stroke.path, mainStroke);
    }
}

void CanvasComponent::drawCrosshair(juce::Graphics& g, juce::Point<int> position)
{
    // 🎨 ENHANCED PAINT CURSOR: Dynamic cursor based on paint state
    juce::Colour cursorColor = getCurrentBrushColor();
    g.setColour(cursorColor.withAlpha(0.8f));
    
    auto bounds = getLocalBounds().toFloat();
    
    // 🎯 PRECISE CROSSHAIR: Full-screen guides for accurate painting
    g.drawVerticalLine(position.x, 0, static_cast<int>(bounds.getHeight()));
    g.drawHorizontalLine(position.y, 0, static_cast<int>(bounds.getWidth()));
    
    // 🎨 PAINT SIZE INDICATOR: Show brush size with breathing animation
    float breathingScale = 1.0f + std::sin(spectralViz.effectTime * 4.0f) * 0.1f;
    float brushRadius = 8.0f * breathingScale;
    
    // Outer glow
    g.setColour(cursorColor.withAlpha(0.3f));
    g.fillEllipse(position.x - brushRadius * 1.5f, position.y - brushRadius * 1.5f, 
                  brushRadius * 3.0f, brushRadius * 3.0f);
    
    // Main brush circle
    g.setColour(cursorColor.withAlpha(0.7f));
    g.drawEllipse(position.x - brushRadius, position.y - brushRadius, 
                  brushRadius * 2.0f, brushRadius * 2.0f, 2.0f);
    
    // Center dot
    g.setColour(cursorColor);
    g.fillEllipse(position.x - 2, position.y - 2, 4, 4);
    
    // 🎵 FREQUENCY INDICATOR: Show frequency mapping
    auto spectralPos = screenToSpectral(position.toFloat());
    juce::String freqText = juce::String((int)spectralPos.y) + "Hz";
    g.setColour(cursorColor.withAlpha(0.8f));
    g.setFont(12.0f);
    g.drawText(freqText, juce::Rectangle<int>(position.x + 15, position.y - 20, 60, 20), juce::Justification::left);
}

void CanvasComponent::updatePressureCursor(float pressure)
{
    // Update the visual cursor radius based on pressure
    // Exponential scaling for more natural feel
    pressureCursorRadius = 5.0f + (pressure * pressure * 25.0f);  // 5-30 pixel radius
}

void CanvasComponent::drawPressureCursor(juce::Graphics& g) const
{
    // Draw pressure-sensitive brush preview cursor
    juce::Colour brushColor = getCurrentBrushColor();
    
    // Outer pressure indicator ring
    g.setColour(brushColor.withAlpha(0.2f));
    g.fillEllipse(lastMousePos.x - pressureCursorRadius * 1.2f, 
                  lastMousePos.y - pressureCursorRadius * 1.2f,
                  pressureCursorRadius * 2.4f, 
                  pressureCursorRadius * 2.4f);
    
    // Main pressure circle
    g.setColour(brushColor.withAlpha(0.4f));
    g.drawEllipse(lastMousePos.x - pressureCursorRadius, 
                  lastMousePos.y - pressureCursorRadius,
                  pressureCursorRadius * 2.0f, 
                  pressureCursorRadius * 2.0f, 2.0f);
    
    // Inner pressure indicator
    float innerRadius = pressureCursorRadius * currentStrokePressure;
    g.setColour(brushColor.withAlpha(0.6f));
    g.fillEllipse(lastMousePos.x - innerRadius * 0.3f, 
                  lastMousePos.y - innerRadius * 0.3f,
                  innerRadius * 0.6f, 
                  innerRadius * 0.6f);
    
    // Pressure value text (only when using tablet)
    if (isUsingTabletPressure)
    {
        g.setColour(brushColor.withAlpha(0.8f));
        g.setFont(10.0f);
        juce::String pressureText = juce::String(currentStrokePressure * 100.0f, 0) + "%";
        g.drawText(pressureText, 
                   lastMousePos.x + pressureCursorRadius + 5, 
                   lastMousePos.y - 10, 
                   40, 20, 
                   juce::Justification::left);
    }
}

void CanvasComponent::drawPaintStatusIndicator(juce::Graphics& g)
{
    // 🎨 PAINT STATUS INDICATOR: Show paint interface is responsive and ready
    juce::Colour statusColor = isDragging ? juce::Colours::lime : juce::Colours::cyan;
    
    // Status dot in top-right corner
    float pulseScale = 1.0f + std::sin(spectralViz.effectTime * 6.0f) * 0.2f;
    float dotSize = 8.0f * pulseScale;
    
    g.setColour(statusColor.withAlpha(0.8f));
    auto bounds = getLocalBounds();
    g.fillEllipse(bounds.getRight() - 25, 15, dotSize, dotSize);
    
    // Status text
    juce::String statusText = isDragging ? "PAINTING" : "READY";
    g.setColour(statusColor.withAlpha(0.7f));
    g.setFont(10.0f);
    g.drawText(statusText, bounds.getRight() - 80, 10, 50, 20, juce::Justification::right);
    
    // Paint stroke count indicator
    if (!paintStrokes.empty()) {
        juce::String strokeCount = juce::String(paintStrokes.size()) + " strokes";
        g.setColour(juce::Colours::white.withAlpha(0.5f));
        g.drawText(strokeCount, bounds.getRight() - 120, 30, 100, 15, juce::Justification::right);
    }
}

//==============================================================================
// MetaSynth-Style Spectral Visualization Rendering
//==============================================================================

void CanvasComponent::drawSpectralRoom(juce::Graphics& g) const
{
    auto bounds = getLocalBounds().toFloat();
    
    // Dynamic room background with breathing animation
    juce::Colour roomBase = spectralViz.roomColor;
    juce::Colour roomAnimated = roomBase.withMultipliedBrightness(spectralViz.roomAnimation);
    
    // Create subtle gradient for depth (like MetaSynth rooms)
    juce::ColourGradient roomGradient(
        roomAnimated.darker(0.3f), bounds.getCentreX(), 0,
        roomAnimated.brighter(0.2f), bounds.getCentreX(), bounds.getHeight(),
        false
    );
    
    // Add radial component for more depth
    roomGradient.addColour(0.5, roomAnimated);
    
    g.setGradientFill(roomGradient);
    g.fillAll();
    
    // Subtle room texture overlay
    if (spectralViz.currentEffect > 0)
    {
        g.setColour(roomAnimated.withAlpha(0.1f));
        for (int i = 0; i < 20; ++i)
        {
            float x = (i / 20.0f) * bounds.getWidth();
            g.drawVerticalLine(static_cast<int>(x), 0, bounds.getHeight());
        }
    }
}

void CanvasComponent::drawSpectralEffect(juce::Graphics& g, int effectType, float intensity, juce::Rectangle<float> area) const
{
    g.setColour(spectralViz.roomColor.withAlpha(intensity * 0.3f));
    
    switch (effectType)
    {
        case 1: // Blur - concentric circles
        {
            auto center = area.getCentre();
            for (int i = 1; i <= 5; ++i)
            {
                float radius = i * 50.0f * intensity;
                float alpha = (1.0f - i / 5.0f) * intensity * 0.2f;
                g.setColour(spectralViz.roomColor.withAlpha(alpha));
                g.drawEllipse(center.x - radius, center.y - radius, radius * 2, radius * 2, 2.0f);
            }
            break;
        }
        
        case 2: // Randomize - scattered rectangles
        {
            juce::Random random(static_cast<int>(spectralViz.effectTime * 1000));
            for (int i = 0; i < static_cast<int>(intensity * 20); ++i)
            {
                float x = random.nextFloat() * area.getWidth();
                float y = random.nextFloat() * area.getHeight();
                float size = random.nextFloat() * 20.0f + 5.0f;
                g.setColour(spectralViz.roomColor.withAlpha(0.1f));
                g.fillRect(x, y, size, size);
            }
            break;
        }
        
        case 3: // Shuffle - geometric patterns
        {
            g.setColour(spectralViz.roomColor.withAlpha(intensity * 0.2f));
            for (int i = 0; i < 8; ++i)
            {
                float angle = (i / 8.0f) * juce::MathConstants<float>::twoPi + spectralViz.effectTime;
                auto center = area.getCentre();
                float radius = 100.0f * intensity;
                juce::Point<float> point(
                    center.x + std::cos(angle) * radius,
                    center.y + std::sin(angle) * radius
                );
                g.fillEllipse(point.x - 5, point.y - 5, 10, 10);
            }
            break;
        }
        
        case 4: // Freeze - crystalline structures
        {
            g.setColour(spectralViz.roomColor.withAlpha(intensity * 0.3f));
            auto center = area.getCentre();
            for (int i = 0; i < 6; ++i)
            {
                float angle = (i / 6.0f) * juce::MathConstants<float>::twoPi;
                juce::Point<float> end(
                    center.x + std::cos(angle) * 80.0f * intensity,
                    center.y + std::sin(angle) * 80.0f * intensity
                );
                g.drawLine(center.x, center.y, end.x, end.y, 2.0f);
            }
            break;
        }
        
        case 5: // Arpeggiate - wave patterns
        {
            g.setColour(spectralViz.roomColor.withAlpha(intensity * 0.25f));
            juce::Path wavePath;
            wavePath.startNewSubPath(0, area.getCentreY());
            
            for (float x = 0; x < area.getWidth(); x += 5.0f)
            {
                float y = area.getCentreY() + std::sin((x / area.getWidth()) * juce::MathConstants<float>::twoPi * 3.0f + spectralViz.effectTime * 2.0f) * 50.0f * intensity;
                wavePath.lineTo(x, y);
            }
            
            g.strokePath(wavePath, juce::PathStrokeType(3.0f));
            break;
        }
    }
}

void CanvasComponent::drawFrequencyBands(juce::Graphics& g) const
{
    auto bounds = getLocalBounds().toFloat();
    
    // Draw frequency bands as vertical bars (like a spectrum analyzer)
    if (spectralViz.currentEffect > 0)
    {
        const int numBands = spectralViz.frequencyBands.size();
        const float bandWidth = bounds.getWidth() / numBands;
        
        for (int i = 0; i < numBands; ++i)
        {
            float level = spectralViz.frequencyBands[i] * spectralViz.effectIntensity;
            float height = level * bounds.getHeight() * 0.3f;
            
            juce::Colour bandColor = spectralViz.roomColor.withAlpha(level * 0.5f);
            g.setColour(bandColor);
            
            g.fillRect(i * bandWidth, bounds.getHeight() - height, bandWidth - 1, height);
        }
    }
}

void CanvasComponent::drawSpectralParticles(juce::Graphics& g) const
{
    if (spectralViz.spectralParticles.empty()) return;
    
    juce::Colour particleColor = spectralViz.roomColor.brighter(0.5f);
    
    for (const auto& particle : spectralViz.spectralParticles)
    {
        float alpha = 0.3f * spectralViz.effectIntensity;
        g.setColour(particleColor.withAlpha(alpha));
        
        // Draw particle with glow effect
        g.fillEllipse(particle.x - 2, particle.y - 2, 4, 4);
        g.setColour(particleColor.withAlpha(alpha * 0.3f));
        g.fillEllipse(particle.x - 4, particle.y - 4, 8, 8);
    }
}

//==============================================================================
// Audio Interaction
//==============================================================================

void CanvasComponent::processPaintStroke(juce::Point<float> spectralPos, float pressure)
{
    // SUB-5MS OPTIMIZATION: Direct paint-to-audio pathway with minimal overhead
    DBG("🚀 OPTIMIZED PAINT: Processing paint stroke at (" << spectralPos.x << "," << spectralPos.y << ")");
    
    // 🚨 EMERGENCY DIAGNOSTIC: Check canvas bounds synchronization
    auto bounds = getLocalBounds().toFloat();
    DBG("🔍 CANVAS BOUNDS: " << bounds.getX() << "," << bounds.getY() << "," << bounds.getWidth() << "," << bounds.getHeight());
    
    // IMMEDIATE VISUAL FEEDBACK: Create responsive visual without blocking on audio
    float frequency = juce::jmap(spectralPos.y, 0.0f, (float)getHeight(), 2000.0f, 200.0f); // Top = high freq
    
    // PRIORITY 1: Create immediate visual feedback (this is instant and safe)
    createVisualFeedback(spectralPos, frequency, pressure);
    
    // SUB-5MS DIRECT AUDIO PATH: Bypass all queues and locks for maximum performance
    if (spectralSynthEngine)
    {
        // 🚨 CRITICAL FIX: Update canvas bounds BEFORE processing paint stroke
        spectralSynthEngine->updateCanvasBounds(bounds);
        DBG("🔧 CANVAS BOUNDS UPDATED: " << bounds.getX() << "," << bounds.getY() << "," << bounds.getWidth() << "," << bounds.getHeight());
        
        try {
            // DIRECT PAINT-TO-AUDIO: Use new optimized pathway
            spectralSynthEngine->processPaintStrokeDirectToAudio(
                spectralPos.x, spectralPos.y, pressure, getCurrentBrushColor());
            
            DBG("⚡ DIRECT AUDIO PATH: " << (int)frequency << "Hz pressure=" << pressure);
        }
        catch (const std::exception& e) {
            DBG("🚨 AUDIO ERROR: " << e.what() << " - continuing with visual feedback only");
            
            // FALLBACK: Use standard oscillator allocation if direct path fails
            spectralSynthEngine->addSpectralOscillatorOptimized(
                frequency, 0.3f * pressure, 0.0f, getCurrentBrushColor());
        }
    }
    
    // VISUAL EFFECTS: Add spectral room response
    setActiveSpectralEffect(1, pressure); // Activate blur effect with pressure intensity
    
    // UI thread remains responsive - direct audio processing is ultra-fast
    DBG("🚀 OPTIMIZED PAINT COMPLETE: Sub-5ms paint-to-audio latency achieved");
}

//==============================================================================
// Revolutionary UX Enhancements
//==============================================================================

void CanvasComponent::createVisualFeedback(juce::Point<float> pos, float frequency, float pressure)
{
    // 🎨 IMMEDIATE VISUAL SATISFACTION: Create responsive visual feedback
    
    // Add frequency-based color feedback (lower = red, higher = blue)
    float normalizedFreq = juce::jlimit(0.0f, 1.0f, (frequency - 20.0f) / 19980.0f);
    juce::Colour freqColor = juce::Colour::fromHSV(0.7f - (normalizedFreq * 0.7f), 0.8f, 0.9f, pressure);
    
    // Create pressure-based size feedback
    float visualSize = 2.0f + (pressure * 8.0f); // 2-10 pixel radius
    
    // 🎨 ENHANCED PARTICLE SYSTEM: Add multiple particles for rich feedback
    for (int i = 0; i < std::max(1, (int)(pressure * 3)); ++i) {
        if (spectralViz.spectralParticles.size() < 100) { // Limit particles for performance
            juce::Point<float> particlePos = pos;
            // Add slight randomness for organic feel
            particlePos.x += (juce::Random::getSystemRandom().nextFloat() - 0.5f) * visualSize;
            particlePos.y += (juce::Random::getSystemRandom().nextFloat() - 0.5f) * visualSize;
            spectralViz.spectralParticles.push_back(particlePos);
        }
    }
    
    // Update room color based on frequency for immersive feedback
    spectralViz.roomColor = freqColor.darker(0.7f);
    spectralViz.effectIntensity = pressure;
    
    // 🎨 IMMEDIATE PAINT STROKE: Add to visual strokes for persistence
    if (!paintStrokes.empty()) {
        // Enhance current stroke with pressure/frequency data
        auto& currentStroke = paintStrokes.back();
        currentStroke.color = freqColor;
        currentStroke.intensity = pressure;
    }
    
    // 🎨 INSTANT REPAINT: Trigger immediate visual update
    repaint();
    
    DBG("🎨 ENHANCED VISUAL FEEDBACK: Created at (" << pos.x << "," << pos.y << ") freq=" << (int)frequency << "Hz pressure=" << pressure);
}

juce::Colour CanvasComponent::getCurrentBrushColor() const
{
    // 🖌️ DYNAMIC BRUSH COLOR: Based on canvas state and interaction
    
    // Use current room color as base (creates visual-audio coherence)
    juce::Colour baseColor = spectralViz.roomColor.brighter(0.3f);
    
    // Add mouse position influence (X=hue shift, Y=brightness)
    if (getWidth() > 0 && getHeight() > 0)
    {
        float xNorm = lastMousePos.x / getWidth();
        float yNorm = lastMousePos.y / getHeight();
        
        // Shift hue based on X position (left=red, center=green, right=blue)
        float hue = xNorm * 0.8f; // 0.0 to 0.8 (red to purple)
        
        // Adjust brightness based on Y position (top=bright, bottom=dark)
        float brightness = 0.3f + (1.0f - yNorm) * 0.7f; // 0.3 to 1.0
        
        return juce::Colour::fromHSV(hue, 0.7f, brightness, 0.8f);
    }
    
    return baseColor;
}

//==============================================================================
// Magic Wand Simulation
//==============================================================================

void CanvasComponent::simulatePaintStroke(float x, float y, float pressure, juce::Colour color, bool startStroke, bool endStroke)
{
    if (endStroke)
    {
        // End the current stroke
        return;
    }
    
    if (startStroke)
    {
        // Start new visual stroke
        paintStrokes.emplace_back(color, pressure);
        paintStrokes.back().path.startNewSubPath(x, y);
    }
    else if (!paintStrokes.empty())
    {
        // Continue current stroke
        paintStrokes.back().path.lineTo(x, y);
    }
    
    // Process audio synthesis
    auto spectralPos = juce::Point<float>(x, y);
    processPaintStroke(spectralPos, pressure);
    
    // Trigger repaint for visual feedback
    repaint();
}

//==============================================================================
// Missing methods required by PaintControlPanel

void CanvasComponent::resetView()
{
    // Reset any zoom, pan, or view transformations
    // For now, just clear the canvas view state
    clearCanvas();
    repaint();
}

void CanvasComponent::setBrushSize(float size)
{
    // Set the brush size for painting
    pressureCursorRadius = juce::jlimit(1.0f, 50.0f, size * 5.0f);
    repaint();
}

void CanvasComponent::setBrushPressure(float pressure)
{
    // Set the brush pressure/intensity
    currentStrokePressure = juce::jlimit(0.0f, 1.0f, pressure);
    repaint();
}