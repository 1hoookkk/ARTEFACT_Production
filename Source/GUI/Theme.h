#pragma once
#include <JuceHeader.h>

namespace Theme {
    // ==============================================================================
    // AUDITY THEME TOKEN SYSTEM
    // Professional E-mu Audity-inspired design tokens for SpectralCanvas Pro
    // ==============================================================================
    
    // Core Colors - Audity 2000 Hardware Aesthetic (Final Consolidated Theme)
    constexpr uint32_t bg = 0xff1c2328;               // Main background (dark blue-gray)
    constexpr uint32_t panelTeal = 0xff345e78;        // Panel background (powder-coat teal)
    constexpr uint32_t bezel = 0xff3d4349;            // Screen bezels and button frames
    constexpr uint32_t textHi = 0xffe6eef3;           // Primary text/labels (high contrast)
    constexpr uint32_t textLo = 0xff8ea3b0;           // Secondary text (dimmed)
    constexpr uint32_t lcdLime = 0xff8dff6a;          // LCD/readout green
    
    // Accent Colors
    constexpr uint32_t accentBlue = 0xff6ad1ff;       // Primary accent (bright cyan-blue)
    constexpr uint32_t ledRed = 0xffff4545;           // Status LED red
    constexpr uint32_t ledGreen = 0xff00ff41;         // Ready/active LED green
    constexpr uint32_t ledOrange = 0xffff8f2a;        // Warning LED orange
    
    // State Colors
    constexpr uint32_t hoverHighlight = 0xff4a7fbb;   // Button hover state
    constexpr uint32_t pressedDark = 0xff2a3e58;      // Button pressed state
    constexpr uint32_t selectedBlue = 0xff5a9fef;     // Selected/active items
    constexpr uint32_t focusOutline = 0xff7fbfff;     // Keyboard focus indicator
    
    // Shadow and Effects
    constexpr uint32_t shadowDark = 0x59000000;       // Drop shadows (35% black)
    constexpr uint32_t rimLight = 0x1fffffff;         // Subtle rim lighting (12% white)
    constexpr uint32_t glowSoft = 0x4d8dff6a;         // LCD glow effect (30% lime)
    
    // Canvas and Grid
    constexpr uint32_t canvasBackground = bg;         // Canvas uses main background
    constexpr uint32_t gridLines = 0x1a8ea3b0;        // Grid lines (~10% opacity of textLo)
    constexpr uint32_t gridMajor = 0x338ea3b0;        // Major grid lines (~20% opacity of textLo)
    
    // ==============================================================================
    // SPACING AND LAYOUT TOKENS
    // 8px grid system for consistent spacing
    // ==============================================================================
    
    constexpr int gridUnit = 8;                       // Base spacing unit (8px)
    constexpr int spacing_xs = gridUnit / 2;          // 4px - minimal spacing
    constexpr int spacing_sm = gridUnit;              // 8px - small spacing
    constexpr int spacing_md = gridUnit * 2;          // 16px - medium spacing
    constexpr int spacing_lg = gridUnit * 3;          // 24px - large spacing
    constexpr int spacing_xl = gridUnit * 4;          // 32px - extra large spacing
    
    // Panel Dimensions (Fixed Layout as per specification)
    constexpr int leftPanelWidth = 220;               // Fixed left navigation width
    constexpr int rightPanelWidth = 340;              // Fixed right inspector width
    constexpr int topBarHeight = 56;                  // Header bar height (56px)
    constexpr int roomsViewWidth = leftPanelWidth;    // Rooms navigation width
    
    // Component Sizing
    constexpr int buttonHeight = 32;                  // Standard button height
    constexpr int buttonHeightLarge = 40;             // Large button height
    constexpr int knobDiameter = 48;                  // Standard knob size
    constexpr int knobDiameterLarge = 64;             // Large knob size
    constexpr int sliderHeight = 24;                  // Horizontal slider height
    constexpr int toggleSize = 20;                    // Toggle switch size
    
    // Content Padding
    constexpr int panelPadding = spacing_md;          // 16px - standard panel padding
    constexpr int panelPaddingLarge = spacing_lg;     // 24px - large panel padding
    constexpr int contentMargin = spacing_sm;         // 8px - content margins
    constexpr int sectionSpacing = spacing_lg;        // 24px - between sections
    
    // ==============================================================================
    // TYPOGRAPHY TOKENS
    // Professional typography scale for hardware interfaces
    // ==============================================================================
    
    constexpr float fontSize_tiny = 9.0f;             // Status text, copyright
    constexpr float fontSize_small = 11.0f;           // Labels, secondary text
    constexpr float fontSize_body = 13.0f;            // Body text, button labels
    constexpr float fontSize_large = 16.0f;           // Headings, emphasis
    constexpr float fontSize_display = 20.0f;         // Display values, titles
    
    // HUD and readouts specifically tuned for hardware aesthetic
    constexpr float hudFontSize = 13.0f;              // HUD readouts (CPU, SR, etc.) - 13-14pt
    constexpr float labelFontSize = 11.0f;            // Parameter labels
    constexpr float valueFontSize = 13.0f;            // Parameter values
    constexpr float logoFontSize = 18.0f;             // Logo and wordmarks
    
    // Font weights (for fonts that support it)
    constexpr float fontWeight_light = 300.0f;
    constexpr float fontWeight_regular = 400.0f;
    constexpr float fontWeight_medium = 500.0f;
    constexpr float fontWeight_bold = 700.0f;
    
    // ==============================================================================
    // VISUAL EFFECTS TOKENS
    // Consistent visual styling across all components
    // ==============================================================================
    
    constexpr float cornerRadius = 6.0f;              // Standard corner radius (6px as per spec)
    constexpr float cornerRadiusLarge = 8.0f;         // Large components
    constexpr float cornerRadiusSmall = 2.0f;         // Small elements
    
    constexpr float borderWidth = 1.0f;               // Standard border thickness
    constexpr float borderWidthThick = 2.0f;          // Emphasis borders
    constexpr float borderWidthThin = 0.5f;           // Subtle borders
    
    constexpr float gridAlpha = 0.2f;                 // Canvas grid transparency
    constexpr float disabledAlpha = 0.5f;             // Disabled component alpha
    constexpr float hoverAlpha = 0.8f;                // Hover state alpha
    constexpr float pressedAlpha = 0.6f;              // Pressed state alpha
    
    // Shadow parameters
    constexpr float shadowRadius = 4.0f;              // Drop shadow blur
    constexpr float shadowOffset = 2.0f;              // Shadow offset distance
    constexpr float glowRadius = 6.0f;                // LED glow effect radius
    
    // ==============================================================================
    // ANIMATION TOKENS
    // Smooth, professional transitions
    // ==============================================================================
    
    constexpr int animationDuration_fast = 150;       // Fast transitions (ms)
    constexpr int animationDuration_medium = 250;     // Medium transitions (ms)
    constexpr int animationDuration_slow = 400;       // Slow transitions (ms)
    
    constexpr float animationEasing = 0.4f;           // Ease-out timing function
    
    // ==============================================================================
    // COMPONENT-SPECIFIC TOKENS
    // Specialized measurements for specific UI components
    // ==============================================================================
    
    // TopBar Layout
    constexpr int topBarLogoWidth = 160;
    constexpr int topBarButtonGroupSpacing = spacing_md;
    constexpr int topBarReadoutWidth = 80;
    constexpr int topBarLEDSize = 12;
    
    // RoomsView Layout
    constexpr int roomButtonHeight = 36;
    constexpr int roomButtonSpacing = spacing_xs;
    constexpr int roomLEDSize = 8;
    constexpr int roomIconSize = 16;
    
    // Inspector Panels
    constexpr int inspectorSectionHeight = 120;
    constexpr int parameterRowHeight = 28;
    constexpr int parameterLabelWidth = 80;
    constexpr int parameterControlWidth = 200;
    
    // Canvas Controls
    constexpr int toolButtonSize = 32;
    constexpr int toolGroupSpacing = spacing_sm;
    constexpr int zoomControlWidth = 120;
    
    // ==============================================================================
    // HELPER FUNCTIONS
    // Utility functions for consistent color manipulation
    // ==============================================================================
    
    inline juce::Colour withAlpha(uint32_t color, float alpha) {
        return juce::Colour(color).withAlpha(alpha);
    }
    
    inline juce::Colour darker(uint32_t color, float amount = 0.2f) {
        return juce::Colour(color).darker(amount);
    }
    
    inline juce::Colour lighter(uint32_t color, float amount = 0.2f) {
        return juce::Colour(color).brighter(amount);
    }
    
    inline juce::Colour interpolate(uint32_t color1, uint32_t color2, float ratio) {
        return juce::Colour(color1).interpolatedWith(juce::Colour(color2), ratio);
    }
    
    // Convert theme tokens to JUCE colours for convenience
    inline juce::Colour background() { return juce::Colour(bg); }
    inline juce::Colour panel() { return juce::Colour(panelTeal); }
    inline juce::Colour bezelColor() { return juce::Colour(bezel); }
    inline juce::Colour text() { return juce::Colour(textHi); }
    inline juce::Colour textDim() { return juce::Colour(textLo); }
    inline juce::Colour accent() { return juce::Colour(accentBlue); }
    inline juce::Colour lcd() { return juce::Colour(lcdLime); }
    inline juce::Colour led() { return juce::Colour(ledRed); }
    inline juce::Colour grid() { return juce::Colour(gridLines); }
    inline juce::Colour gridMajorColor() { return juce::Colour(gridMajor); }
    
    // Layout helper functions
    inline juce::Rectangle<int> withPadding(const juce::Rectangle<int>& bounds, int padding = panelPadding) {
        return bounds.reduced(padding);
    }
    
    inline juce::Rectangle<int> withMargin(const juce::Rectangle<int>& bounds, int margin = contentMargin) {
        return bounds.reduced(margin);
    }
}