#pragma once
#include <JuceHeader.h>
#include "Theme.h"

// ==============================================================================
// LAYOUT SPECIFICATION SYSTEM
// Centralized layout constants and helper functions for SpectralCanvas Pro
// Enforces strict 8px grid alignment for pixel-perfect Audity-2000 aesthetic
// ==============================================================================

namespace Layout {
    // ==============================================================================
    // BASE CANVAS DIMENSIONS (Required by PluginEditor.cpp)
    // ==============================================================================
    constexpr int baseW = 900;     // Base canvas width (minimum)
    constexpr int baseH = 600;     // Base canvas height (minimum)
    constexpr int minW = 900;      // Minimum resizable width
    constexpr int minH = 600;      // Minimum resizable height
    
    // ==============================================================================
    // GRID SYSTEM (8px rhythm from Theme.h)
    // ==============================================================================
    constexpr int unit = Theme::gridUnit;     // 8px base unit
    constexpr int halfUnit = unit / 2;        // 4px for fine adjustments
    
    // ==============================================================================
    // LAYOUT AREAS (Fixed dimensions for major UI sections)
    // ==============================================================================
    constexpr int topBarH = Theme::topBarHeight;     // 56px header
    constexpr int leftNavW = Theme::leftPanelWidth;  // 220px left nav
    constexpr int inspectorW = Theme::rightPanelWidth; // 340px inspector
    
    // ==============================================================================
    // SPACING CONSTANTS (From Theme.h)
    // ==============================================================================
    constexpr int padSm = Theme::spacing_sm;   // 8px - small padding
    constexpr int padMd = Theme::spacing_md;   // 16px - medium padding  
    constexpr int padLg = Theme::spacing_lg;   // 24px - large padding
    constexpr int padXl = Theme::spacing_xl;   // 32px - extra large padding
    
    // ==============================================================================
    // LAYOUT HELPER FUNCTIONS
    // Functions to extract layout areas from bounds
    // ==============================================================================
    
    // Extract top bar area (56px height)
    inline juce::Rectangle<int> takeTopBar(juce::Rectangle<int>& bounds) {
        return bounds.removeFromTop(topBarH);
    }
    
    // Extract left navigation area (220px width) 
    inline juce::Rectangle<int> takeNav(juce::Rectangle<int>& bounds) {
        return bounds.removeFromLeft(leftNavW);
    }
    
    // Extract right inspector area (340px width)
    inline juce::Rectangle<int> takeInspector(juce::Rectangle<int>& bounds) {
        return bounds.removeFromRight(inspectorW);
    }
    
    // Apply padding to bounds using theme constants
    inline juce::Rectangle<int> withPadding(const juce::Rectangle<int>& bounds, 
                                           int padding = padMd) {
        return bounds.reduced(padding);
    }
    
    // ==============================================================================
    // PIXEL SNAPPING UTILITIES
    // Ensure crisp, pixel-perfect rendering
    // ==============================================================================
    
    // Snap rectangle to pixel boundaries
    inline juce::Rectangle<int> snap(const juce::Rectangle<float>& r) {
        return juce::Rectangle<int>(
            juce::roundToInt(r.getX()),
            juce::roundToInt(r.getY()), 
            juce::roundToInt(r.getWidth()),
            juce::roundToInt(r.getHeight())
        );
    }
    
    // Align value to grid
    inline int alignToGrid(int value, int gridSize = unit) {
        return (value / gridSize) * gridSize;
    }
    
    // Round up to next grid unit  
    inline int ceilToGrid(int value, int gridSize = unit) {
        return ((value + gridSize - 1) / gridSize) * gridSize;
    }
    
    // ==============================================================================
    // GRID CALCULATION HELPERS
    // Convert grid units to pixels
    // ==============================================================================
    
    inline int grid(int units) { 
        return units * unit; 
    }
    
    inline juce::Rectangle<int> gridRect(int xUnits, int yUnits, 
                                        int widthUnits, int heightUnits) {
        return juce::Rectangle<int>(
            grid(xUnits), 
            grid(yUnits), 
            grid(widthUnits), 
            grid(heightUnits)
        );
    }
    
    // ==============================================================================
    // RESPONSIVE BREAKPOINTS
    // For future responsive behavior
    // ==============================================================================
    enum class Breakpoint {
        Compact,    // < 1024px width  
        Regular,    // 1024-1440px
        Large,      // 1440-1920px
        XLarge      // > 1920px
    };
    
    inline Breakpoint getBreakpoint(int width) {
        if (width < 1024) return Breakpoint::Compact;
        if (width < 1440) return Breakpoint::Regular; 
        if (width < 1920) return Breakpoint::Large;
        return Breakpoint::XLarge;
    }
    
    inline juce::String getBreakpointName(Breakpoint bp) {
        switch (bp) {
            case Breakpoint::Compact: return "Compact";
            case Breakpoint::Regular: return "Regular";
            case Breakpoint::Large: return "Large"; 
            case Breakpoint::XLarge: return "XLarge";
            default: return "Unknown";
        }
    }
    
    // ==============================================================================
    // LAYOUT VALIDATION (Debug helpers)
    // ==============================================================================
    
    // Check if rectangle is aligned to grid
    inline bool isGridAligned(const juce::Rectangle<int>& r, int gridSize = unit) {
        return (r.getX() % gridSize == 0) &&
               (r.getY() % gridSize == 0) &&
               (r.getWidth() % gridSize == 0) &&
               (r.getHeight() % gridSize == 0);
    }
    
    // Get grid alignment errors as string (for debugging)
    inline juce::String getGridAlignmentErrors(const juce::Rectangle<int>& r, 
                                             int gridSize = unit) {
        juce::StringArray errors;
        if (r.getX() % gridSize != 0) errors.add("X not aligned");
        if (r.getY() % gridSize != 0) errors.add("Y not aligned");
        if (r.getWidth() % gridSize != 0) errors.add("Width not aligned");
        if (r.getHeight() % gridSize != 0) errors.add("Height not aligned");
        return errors.joinIntoString(", ");
    }
}