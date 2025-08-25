#pragma once
#include <JuceHeader.h>
#include "../Core/PaintQueue.h"

// Minimal stub for PixelCanvasComponent to get build working
class PixelCanvasComponent : public juce::Component
{
public:
    PixelCanvasComponent() 
    {
        setSize(400, 400);
        setMouseClickGrabsKeyboardFocus(false);
    }
    
    void setPaintQueue(SpectralPaintQueue* queue)
    {
        paintQueue_ = queue;
    }
    
    void clearCanvas()
    {
        repaint();
    }
    
    void panicDisableVisuals()
    {
        // Stub implementation
    }
    
    // Additional stub methods needed by PluginEditorY2K
    void setReduceMotion(bool reduce)
    {
        // Stub implementation
    }
    
    void setScanlinesEnabled(bool enabled)
    {
        // Stub implementation  
    }
    
    void setBloomEnabled(bool enabled)
    {
        // Stub implementation
    }
    
    void paint(juce::Graphics& g) override
    {
        // Simple stub - just fill with black background
        g.fillAll(juce::Colours::black);
        
        // Draw a simple grid to indicate this is the canvas area
        g.setColour(juce::Colour(0xFF333333));
        for (int i = 0; i < getWidth(); i += 32) {
            g.drawVerticalLine(i, 0.0f, static_cast<float>(getHeight()));
        }
        for (int i = 0; i < getHeight(); i += 32) {
            g.drawHorizontalLine(i, 0.0f, static_cast<float>(getWidth()));
        }
        
        // Draw paint points as visual feedback
        g.setColour(juce::Colours::cyan);
        for (const auto& point : paintPoints_) {
            g.fillEllipse(point.x - 3.0f, point.y - 3.0f, 6.0f, 6.0f);
        }
    }
    
    void mouseDown(const juce::MouseEvent& e) override
    {
        if (paintQueue_) {
            // Add visual feedback
            paintPoints_.add(e.position);
            
            // Create paint event
            PaintEvent paintEvent;
            paintEvent.nx = e.position.x / getWidth();  // Normalized 0..1
            paintEvent.ny = e.position.y / getHeight(); // Normalized 0..1
            paintEvent.pressure = 1.0f;                 // Full pressure for mouse
            paintEvent.flags = kStrokeStart;
            paintEvent.color = 0xFFFFFFFF;              // White color
            
            paintQueue_->push(paintEvent);
            repaint();
        }
    }
    
    void mouseDrag(const juce::MouseEvent& e) override
    {
        if (paintQueue_) {
            // Add visual feedback
            paintPoints_.add(e.position);
            
            // Create paint event
            PaintEvent paintEvent;
            paintEvent.nx = e.position.x / getWidth();  // Normalized 0..1
            paintEvent.ny = e.position.y / getHeight(); // Normalized 0..1
            paintEvent.pressure = 1.0f;                 // Full pressure for mouse
            paintEvent.flags = kStrokeMove;
            paintEvent.color = 0xFFFFFFFF;              // White color
            
            paintQueue_->push(paintEvent);
            repaint();
        }
    }
    
    void mouseUp(const juce::MouseEvent& e) override
    {
        if (paintQueue_) {
            // Create paint event
            PaintEvent paintEvent;
            paintEvent.nx = e.position.x / getWidth();  // Normalized 0..1
            paintEvent.ny = e.position.y / getHeight(); // Normalized 0..1
            paintEvent.pressure = 1.0f;                 // Full pressure for mouse
            paintEvent.flags = kStrokeEnd;
            paintEvent.color = 0xFFFFFFFF;              // White color
            
            paintQueue_->push(paintEvent);
        }
    }
    
private:
    SpectralPaintQueue* paintQueue_ = nullptr;
    juce::Array<juce::Point<float>> paintPoints_;
};