/**
 * Minimal Paint Engine - Foundation Only
 * Stripped of all complex dependencies
 */

#include "PaintEngineMinimal.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

PaintEngine::PaintEngine()
{
    // Pre-allocate oscillators
    oscillators.reserve(MAX_OSCILLATORS);
    for (int i = 0; i < MAX_OSCILLATORS; ++i)
    {
        oscillators.emplace_back();
    }
}

PaintEngine::~PaintEngine() = default;

void PaintEngine::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    
    // Reset all oscillators
    for (auto& osc : oscillators)
    {
        osc.reset();
    }
}

void PaintEngine::processBlock(juce::AudioBuffer<float>& buffer)
{
    if (!isActive) return;
    
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    
    // Clear buffer first
    buffer.clear();
    
    // Simple test: Generate a sine wave when active
    if (currentStroke != nullptr)
    {
        // Use first oscillator for test
        auto& osc = oscillators[0];
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float sineValue = std::sin(osc.phase);
            
            // Apply to all channels
            for (int channel = 0; channel < numChannels; ++channel)
            {
                buffer.addSample(channel, sample, sineValue * masterGain * 0.1f);
            }
            
            // Update phase
            osc.phase += (2.0 * M_PI * 440.0 / currentSampleRate);
            if (osc.phase >= 2.0 * M_PI)
                osc.phase -= 2.0 * M_PI;
        }
    }
}

void PaintEngine::releaseResources()
{
    // Reset all oscillators
    for (auto& osc : oscillators)
    {
        osc.reset();
    }
}

void PaintEngine::beginStroke(Point position, float pressure, juce::Colour color)
{
    currentStroke = std::make_unique<Stroke>();
    currentStroke->points.push_back(StrokePoint(position, pressure, color));
    
    // Map Y position to frequency (simple mapping for now)
    float frequency = 220.0f + (position.y * 10.0f);
    
    // Activate first oscillator
    auto& osc = oscillators[0];
    osc.frequency = frequency;
    osc.amplitude = pressure * 0.5f;
    osc.isActive = true;
}

void PaintEngine::updateStroke(Point position, float pressure)
{
    if (currentStroke)
    {
        currentStroke->points.push_back(StrokePoint(position, pressure, juce::Colours::white));
        
        // Update frequency based on Y position
        float frequency = 220.0f + (position.y * 10.0f);
        oscillators[0].frequency = frequency;
        oscillators[0].amplitude = pressure * 0.5f;
    }
}

void PaintEngine::endStroke()
{
    if (currentStroke)
    {
        strokes.push_back(std::move(currentStroke));
        currentStroke.reset();
    }
    
    // Deactivate oscillator
    oscillators[0].isActive = false;
}

void PaintEngine::clearCanvas()
{
    strokes.clear();
    currentStroke.reset();
    
    // Reset all oscillators
    for (auto& osc : oscillators)
    {
        osc.reset();
    }
}

// Oscillator implementation
void PaintEngine::Oscillator::reset()
{
    frequency = 440.0f;
    amplitude = 0.0f;
    phase = 0.0f;
    pan = 0.5f;
    isActive = false;
}

// Stroke implementation
PaintEngine::Stroke::Stroke()
{
    points.reserve(100); // Pre-allocate for efficiency
}

// Missing methods needed for linking
void PaintEngine::setCanvasRegion(float left, float top, float right, float bottom)
{
    // Simple implementation for now
    canvasLeft = left;
    canvasTop = top;
    canvasRight = right;
    canvasBottom = bottom;
}

void PaintEngine::setPlayheadPosition(float position)
{
    playheadPosition = position;
}

void PaintEngine::setMasterGain(float gain)
{
    masterGain = gain;
}

void PaintEngine::setFrequencyRange(float minFreq, float maxFreq)
{
    minFrequency = minFreq;
    maxFrequency = maxFreq;
}

float PaintEngine::canvasYToFrequency(float y) const
{
    if (maxFrequency <= minFrequency) return 440.0f;
    
    // Simple linear mapping
    float normalizedY = (y - canvasTop) / (canvasBottom - canvasTop);
    return minFrequency + normalizedY * (maxFrequency - minFrequency);
}