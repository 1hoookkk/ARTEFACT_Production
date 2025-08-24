// Tests/TestCanvasRender.cpp
// Phase 1: Visual Rendering Validation

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Source/CanvasComponent.h"
#include "../Source/Core/PaintQueue.h"

// Test fixture for offscreen canvas rendering
class TestCanvasRender
{
public:
    TestCanvasRender()
        : paintQueue()
        , canvas(paintQueue)
    {
        // Set up offscreen rendering context
        canvas.setSize(800, 600);
    }
    
    void testRenderEventLogging()
    {
        // Initial state
        const int initialCount = canvas.getRenderEventCount();
        
        // Create graphics context for offscreen rendering
        juce::Image testImage(juce::Image::ARGB, 800, 600, true);
        juce::Graphics g(testImage);
        
        // Trigger paint event
        canvas.paint(g);
        
        // Verify counter incremented
        const int finalCount = canvas.getRenderEventCount();
        assert(finalCount > initialCount);
        
        printf("✓ Render event counter: %d -> %d\n", initialCount, finalCount);
    }
    
    void testStrokeCountValidation()
    {
        // Initial empty canvas
        assert(canvas.getStrokeCount() == 0);
        
        // Simulate mouse down/drag/up sequence
        juce::MouseEvent downEvent(
            juce::MouseInputSource{},
            juce::Point<float>(100.0f, 200.0f),
            juce::ModifierKeys::noModifiers,
            0.7f, // pressure
            juce::MouseInputSource::defaultOrientation,
            juce::MouseInputSource::defaultOrientation,
            juce::MouseInputSource::defaultRotation,
            juce::MouseInputSource::defaultTiltX,
            juce::MouseInputSource::defaultTiltY,
            &canvas,
            &canvas,
            juce::Time::getCurrentTime(),
            juce::Point<float>(100.0f, 200.0f),
            juce::Time::getCurrentTime(),
            1,
            false
        );
        
        canvas.mouseDown(downEvent);
        assert(canvas.getStrokeCount() == 0); // Still in progress
        
        canvas.mouseUp(downEvent);
        assert(canvas.getStrokeCount() == 1); // Stroke completed
        
        printf("✓ Stroke count validation passed\n");
    }
    
    void testBoundsValidation()
    {
        // Clear canvas first
        canvas.clearCanvas();
        
        auto bounds = canvas.getLastStrokeBounds();
        assert(bounds.isEmpty());
        
        printf("✓ Bounds validation for empty canvas passed\n");
    }
    
private:
    PaintGestureQueue paintQueue;
    CanvasComponent canvas;
};

// Test runner entry point
int main() { TestCanvasRender test; test.testRenderEventLogging(); test.testStrokeCountValidation(); test.testBoundsValidation(); printf("All canvas render tests passed!\n"); return 0; }