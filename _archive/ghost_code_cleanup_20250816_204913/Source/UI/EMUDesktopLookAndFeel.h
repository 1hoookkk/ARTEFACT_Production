/**
 * EMU Desktop Look and Feel
 * Authentic EMU hardware-inspired styling scaled for desktop software interface
 * Based on EMU Audity 2000, Xtreme Lead-1, and X3 Turbo hardware references
 * 
 * Designed for large desktop interface (1400x900px) with:
 * - Centered 1000x500px paint canvas as focal point
 * - Professional EMU blue metallic panels with orange/yellow accents
 * - Authentic green LCD displays with pixelated EMU fonts
 * - Realistic hardware textures and beveled controls
 * - Vertical software layout with horizontal EMU rack-style sections
 */

#pragma once
#include <JuceHeader.h>
#include "EMUAudityLookAndFeel.h"

class EMUDesktopLookAndFeel : public EMUAudityLookAndFeel
{
public:
    EMUDesktopLookAndFeel();
    ~EMUDesktopLookAndFeel() override = default;
    
    // Desktop-specific EMU color palette (enhanced for larger interface)
    static const juce::Colour EMU_DESKTOP_BLUE_DARK;     // Deep panel backgrounds
    static const juce::Colour EMU_DESKTOP_BLUE_MEDIUM;   // Control panel sections
    static const juce::Colour EMU_DESKTOP_BLUE_LIGHT;    // Highlighted elements
    static const juce::Colour EMU_DESKTOP_ORANGE;        // Interactive accents (Xtreme Lead style)
    static const juce::Colour EMU_DESKTOP_YELLOW;        // Warning/active states
    static const juce::Colour EMU_DESKTOP_GREEN_LCD;     // Authentic LCD green
    static const juce::Colour EMU_DESKTOP_METALLIC;      // Enhanced metallic finish
    static const juce::Colour EMU_DESKTOP_SHADOW_DEEP;   // Professional depth shadows
    static const juce::Colour EMU_DESKTOP_HIGHLIGHT;     // Metallic highlights
    
    // Desktop interface component drawing
    void drawDesktopPanel(juce::Graphics& g, const juce::Rectangle<int>& area,
                          PanelType type, bool isActive = true);
    
    void drawLargeKnob(juce::Graphics& g, const juce::Rectangle<int>& area,
                       float value, bool isActive = true, 
                       const juce::String& label = juce::String());
    
    void drawDesktopButton(juce::Graphics& g, const juce::Rectangle<int>& area,
                          bool isPressed, bool isHighlighted, bool isToggled,
                          const juce::String& text);
    
    void drawLargeLCDDisplay(juce::Graphics& g, const juce::Rectangle<int>& area,
                            const juce::String& text, bool isActive = true,
                            bool showBranding = false);
    
    void drawCanvasFrame(juce::Graphics& g, const juce::Rectangle<int>& canvasArea,
                        bool isPaintActive = false);
    
    void drawMetallicTexture(juce::Graphics& g, const juce::Rectangle<int>& area,
                            float intensity = 1.0f, bool isVertical = false);
    
    void drawSpectralCanvasProBranding(juce::Graphics& g, const juce::Rectangle<int>& area,
                                       const juce::String& modelName = "EMU-INSPIRED");
    
    // Panel type enumeration for different sections
    enum class PanelType
    {
        TopHeader,      // Main EMU branding header
        BottomControl,  // Transport and global controls
        LeftSide,       // Sample and filter controls
        RightSide,      // LFO and effects controls
        CanvasFrame,    // Paint canvas surrounding frame
        LCDSection      // LCD display areas
    };
    
    // Override JUCE components for desktop scale
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override;
    
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;
    
    void drawLabel(juce::Graphics& g, juce::Label& label) override;
    
    void drawGroupComponentOutline(juce::Graphics& g, int width, int height,
                                   const juce::String& text,
                                   const juce::Justification& position,
                                   juce::GroupComponent& group) override;
    
    // Desktop-specific fonts (larger for desktop interface)
    juce::Font getDesktopLCDFont(float height = 24.0f);
    juce::Font getDesktopLabelFont(float height = 14.0f);
    juce::Font getDesktopButtonFont(float height = 16.0f);
    juce::Font getDesktopBrandingFont(float height = 32.0f);
    
private:
    // Enhanced drawing utilities for desktop scale
    void drawEnhancedBevel(juce::Graphics& g, const juce::Rectangle<int>& area,
                          bool isRaised, int bevelWidth = 4);
    
    void drawRealisticMetal(juce::Graphics& g, const juce::Rectangle<int>& area,
                           const juce::Colour& baseColor, bool isVertical = false);
    
    void drawLCDPixelEffect(juce::Graphics& g, const juce::Rectangle<int>& area);
    
    void drawKnobReflection(juce::Graphics& g, const juce::Rectangle<int>& knobArea,
                           float angle);
    
    // Cached fonts for performance
    juce::Font desktopLCDFont;
    juce::Font desktopLabelFont;
    juce::Font desktopButtonFont;
    juce::Font desktopBrandingFont;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EMUDesktopLookAndFeel)
};

/**
 * Desktop Interface Layout Helper
 * Provides utilities for positioning elements in the desktop EMU interface
 */
namespace EMUDesktopLayout
{
    // Calculate panel bounds for desktop interface
    juce::Rectangle<int> getTopPanelBounds(const juce::Rectangle<int>& totalArea);
    juce::Rectangle<int> getBottomPanelBounds(const juce::Rectangle<int>& totalArea);
    juce::Rectangle<int> getLeftPanelBounds(const juce::Rectangle<int>& totalArea);
    juce::Rectangle<int> getRightPanelBounds(const juce::Rectangle<int>& totalArea);
    juce::Rectangle<int> getCanvasBounds(const juce::Rectangle<int>& totalArea);
    
    // Control positioning within panels
    juce::Array<juce::Rectangle<int>> layoutKnobsInGrid(const juce::Rectangle<int>& area,
                                                       int numKnobs, int columns);
    
    juce::Array<juce::Rectangle<int>> layoutButtonsInGrid(const juce::Rectangle<int>& area,
                                                         int numButtons, int columns);
    
    juce::Rectangle<int> getLCDDisplayBounds(const juce::Rectangle<int>& panelArea,
                                           bool isMainDisplay = true);
}

/**
 * Desktop Asset Specifications
 * Defines the larger asset requirements for desktop interface
 */
struct EMUDesktopAssetSpecs
{
    // Desktop interface dimensions
    static constexpr int DESKTOP_WIDTH = 1400;
    static constexpr int DESKTOP_HEIGHT = 900;
    
    // Paint canvas (centerpiece)
    static constexpr int CANVAS_WIDTH = 1000;
    static constexpr int CANVAS_HEIGHT = 500;
    
    // Panel dimensions
    static constexpr int TOP_PANEL_HEIGHT = 120;
    static constexpr int BOTTOM_PANEL_HEIGHT = 80;
    static constexpr int SIDE_PANEL_WIDTH = 200;
    
    // Control sizes (scaled up from hardware)
    static constexpr int DESKTOP_KNOB_SIZE = 80;      // vs 40px in hardware
    static constexpr int DESKTOP_BUTTON_WIDTH = 100;  // vs 50px in hardware  
    static constexpr int DESKTOP_BUTTON_HEIGHT = 50;  // vs 25px in hardware
    static constexpr int DESKTOP_LCD_HEIGHT = 60;     // vs 30px in hardware
    
    // Asset resolution requirements for high-DPI
    static constexpr int ASSET_SCALE_FACTOR = 2;      // 2x for retina displays
    static constexpr int MAX_KNOB_FRAMES = 64;        // Smooth knob animation
};
