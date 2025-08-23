#pragma once
#include <JuceHeader.h>
#include "LayoutSpec.h"
#include "Theme.h"

// ==============================================================================
// LAYOUT OVERLAY COMPONENT
// Visual debugging tool that shows the 8px grid and layout areas
// Toggle with F1 key to visualize alignment and spacing
// ==============================================================================

class LayoutOverlay : public juce::Component
{
public:
    LayoutOverlay()
    {
        // Pass through mouse events - this overlay shouldn't interfere with UI interaction
        setInterceptsMouseClicks(false, false);
        setAlwaysOnTop(true);
        setVisible(false); // Initially hidden
    }
    
    ~LayoutOverlay() override = default;
    
    // Toggle visibility of the grid overlay
    void setGridVisible(bool shouldBeVisible)
    {
        isGridVisible = shouldBeVisible;
        setVisible(shouldBeVisible);
        if (shouldBeVisible)
        {
            toFront(false); // Bring to front but don't grab focus
            repaint();
        }
    }
    
    bool isGridShowing() const { return isGridVisible; }
    
    void paint(juce::Graphics& g) override
    {
        if (!isGridVisible) return;
        
        using namespace Layout;
        using namespace Theme;
        
        auto bounds = getLocalBounds();
        
        // ==============================================================================
        // DRAW 8PX GRID LINES
        // ==============================================================================
        
        // Minor grid lines (8px intervals)
        g.setColour(juce::Colour(Theme::gridLines));
        for (int x = 0; x < bounds.getWidth(); x += unit)
        {
            g.drawVerticalLine(x, 0.0f, static_cast<float>(bounds.getHeight()));
        }
        for (int y = 0; y < bounds.getHeight(); y += unit)
        {
            g.drawHorizontalLine(y, 0.0f, static_cast<float>(bounds.getWidth()));
        }
        
        // Major grid lines (64px intervals = 8 units)
        g.setColour(juce::Colour(Theme::gridMajor));
        for (int x = 0; x < bounds.getWidth(); x += (unit * 8))
        {
            g.fillRect(x, 0, 1, bounds.getHeight());
        }
        for (int y = 0; y < bounds.getHeight(); y += (unit * 8))
        {
            g.fillRect(0, y, bounds.getWidth(), 1);
        }
        
        // ==============================================================================
        // DRAW LAYOUT AREA BOUNDARIES
        // ==============================================================================
        
        g.setFont(Theme::fontSize_small);
        
        // Top bar area
        juce::Rectangle<int> topBarArea(0, 0, bounds.getWidth(), topBarH);
        g.setColour(juce::Colour(Theme::accentBlue).withAlpha(0.3f));
        g.drawRect(topBarArea, 2);
        g.setColour(juce::Colour(Theme::accentBlue).withAlpha(0.8f));
        g.drawText("TopBar (56px)", topBarArea.reduced(4), juce::Justification::topLeft);
        
        // Left navigation area
        juce::Rectangle<int> leftNavArea(0, topBarH, leftNavW, bounds.getHeight() - topBarH);
        g.setColour(juce::Colour(Theme::lcdLime).withAlpha(0.3f));
        g.drawRect(leftNavArea, 2);
        g.setColour(juce::Colour(Theme::lcdLime).withAlpha(0.8f));
        g.drawText("LeftNav (220px)", leftNavArea.reduced(4), juce::Justification::topLeft);
        
        // Main content area
        juce::Rectangle<int> mainArea(leftNavW, topBarH,
                                     bounds.getWidth() - leftNavW - inspectorW,
                                     bounds.getHeight() - topBarH);
        g.setColour(juce::Colour(Theme::textHi).withAlpha(0.2f));
        g.drawRect(mainArea, 2);
        g.setColour(juce::Colour(Theme::textHi));
        g.drawText("Main Content", mainArea.reduced(4), juce::Justification::topLeft);
        
        // Inspector area (if window is wide enough)
        if (bounds.getWidth() >= (leftNavW + 400 + inspectorW))
        {
            juce::Rectangle<int> inspectorArea(bounds.getWidth() - inspectorW, topBarH,
                                              inspectorW, bounds.getHeight() - topBarH);
            g.setColour(juce::Colour(Theme::ledOrange).withAlpha(0.3f));
            g.drawRect(inspectorArea, 2);
            g.setColour(juce::Colour(Theme::ledOrange).withAlpha(0.8f));
            g.drawText("Inspector (340px)", inspectorArea.reduced(4), juce::Justification::topLeft);
        }
        
        // ==============================================================================
        // DRAW PADDING GUIDES
        // ==============================================================================
        
        // Small padding guide (8px)
        g.setColour(juce::Colour(Theme::ledGreen).withAlpha(0.15f));
        g.drawRect(bounds.reduced(padSm), 1);
        
        // Medium padding guide (16px)
        g.setColour(juce::Colour(Theme::ledRed).withAlpha(0.1f));
        g.drawRect(bounds.reduced(padMd), 1);
        
        // ==============================================================================
        // DRAW INFO PANEL
        // ==============================================================================
        
        // Info panel background
        auto infoBounds = juce::Rectangle<int>(bounds.getWidth() - 420, bounds.getHeight() - 80, 415, 75);
        g.setColour(juce::Colour(Theme::bg).withAlpha(0.85f));
        g.fillRoundedRectangle(infoBounds.toFloat(), Theme::cornerRadius);
        g.setColour(juce::Colour(Theme::bezel));
        g.drawRoundedRectangle(infoBounds.toFloat(), Theme::cornerRadius, Theme::borderWidth);
        
        // Info text
        g.setColour(juce::Colour(Theme::textHi));
        g.setFont(Theme::fontSize_small);
        
        auto textArea = infoBounds.reduced(8);
        auto lineHeight = 12;
        
        // Title
        g.setFont(juce::Font(Theme::fontSize_small, juce::Font::bold));
        g.drawText("Layout Grid Overlay (F1 to toggle)", textArea.removeFromTop(lineHeight), 
                   juce::Justification::left);
        
        textArea.removeFromTop(4); // spacing
        g.setFont(Theme::fontSize_small);
        
        // Window info
        juce::String windowInfo = "Window: " + juce::String(bounds.getWidth()) + "x" + juce::String(bounds.getHeight());
        g.drawText(windowInfo, textArea.removeFromTop(lineHeight), juce::Justification::left);
        
        // Grid info
        juce::String gridInfo = "Grid: " + juce::String(unit) + "px base unit";
        g.drawText(gridInfo, textArea.removeFromTop(lineHeight), juce::Justification::left);
        
        // Breakpoint info
        juce::String breakpointInfo = "Layout: " + juce::String(bounds.getWidth() >= (leftNavW + 400 + inspectorW) ? "3-column" : "2-column");
        g.drawText(breakpointInfo, textArea.removeFromTop(lineHeight), juce::Justification::left);
        
        // Alignment status
        juce::String alignmentInfo = "Inspector: " + 
            juce::String(bounds.getWidth() >= (leftNavW + 400 + inspectorW) ? "Visible" : "Hidden");
        g.drawText(alignmentInfo, textArea.removeFromTop(lineHeight), juce::Justification::left);
        
        // ==============================================================================
        // DRAW GRID LEGENDS
        // ==============================================================================
        
        // Legend in top-left corner
        auto legendBounds = juce::Rectangle<int>(8, 8, 200, 60);
        g.setColour(juce::Colour(Theme::bg).withAlpha(0.8f));
        g.fillRoundedRectangle(legendBounds.toFloat(), Theme::cornerRadius);
        g.setColour(juce::Colour(Theme::bezel));
        g.drawRoundedRectangle(legendBounds.toFloat(), Theme::cornerRadius, Theme::borderWidth);
        
        auto legendText = legendBounds.reduced(6);
        g.setColour(juce::Colour(Theme::textHi));
        g.setFont(Theme::fontSize_tiny);
        
        g.drawText("Grid Legend:", legendText.removeFromTop(10), juce::Justification::left);
        
        // Grid line samples
        g.setColour(juce::Colour(Theme::gridLines));
        g.drawText("— 8px grid lines", legendText.removeFromTop(10), juce::Justification::left);
        
        g.setColour(juce::Colour(Theme::gridMajor));
        g.drawText("— 64px major lines", legendText.removeFromTop(10), juce::Justification::left);
        
        g.setColour(juce::Colour(Theme::accentBlue).withAlpha(0.6f));
        g.drawText("□ Layout areas", legendText.removeFromTop(10), juce::Justification::left);
    }
    
    // Override to ensure overlay covers entire parent
    void parentSizeChanged() override
    {
        if (auto* parent = getParentComponent())
        {
            setBounds(parent->getLocalBounds());
        }
    }
    
private:
    bool isGridVisible = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LayoutOverlay)
};