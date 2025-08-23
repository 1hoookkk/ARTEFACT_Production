#pragma once
#include <JuceHeader.h>

namespace scp {

/**
 * @brief Theme tokens for SpectralCanvas Pro Y2K/Winamp aesthetic
 * 
 * Defines all color, spacing, and visual parameters for the single
 * polished Y2K theme. Vector-first approach ensures crisp rendering
 * at any DPI while maintaining authentic retro character.
 */
struct ThemeTokens
{
    // Base colors - deep navy/charcoal foundation
    juce::Colour panelBase;        // Primary background color
    juce::Colour panelAccent;      // Secondary/accent background
    juce::Colour panelStroke;      // Panel outline/separator color
    
    // Chrome/metallic elements
    juce::Colour knobCream;        // Knob face - vintage cream
    juce::Colour knobFaceMetal;    // Brushed metal center
    juce::Colour chromeHighlight;  // Specular highlights
    juce::Colour chromeShadow;     // Bevel shadows
    
    // LED/indicator colors
    juce::Colour ledAmber;         // Primary LED color (amber)
    juce::Colour ledAccent;        // Secondary LED color (cyan)
    juce::Colour ledOff;           // LED off state
    
    // Canvas/CRT colors
    juce::Colour gridGreen;        // CRT phosphor green
    juce::Colour scanlineTint;     // Scanline overlay
    juce::Colour bloomTint;        // Phosphor bloom
    
    // Typography colors
    juce::Colour hudText;          // HUD/readout text
    juce::Colour labelText;        // Control labels
    juce::Colour valueText;        // Parameter values
    
    // Realm/mode colors (for controls)
    juce::Colour realmBlue;        // Blue realm/mode
    juce::Colour realmPurple;      // Purple realm/mode  
    juce::Colour realmYellow;      // Yellow realm/mode
    
    // Visual parameters
    float chromeBevelDepth = 7.0f;        // Chrome bevel depth in pixels
    float specularAlpha = 0.9f;           // Specular highlight intensity
    float scanlineOpacity = 0.15f;        // Scanline overlay opacity
    float bloomRadius = 0.6f;             // Phosphor bloom radius multiplier
    bool useSpeckleTile = true;           // Enable speckle texture
    
    // Spacing/layout (for consistent sizing)
    float knobSize = 64.0f;               // Standard knob diameter
    float buttonHeight = 28.0f;           // Standard button height
    float panelPadding = 12.0f;           // Panel internal padding
    float controlSpacing = 8.0f;          // Spacing between controls
};

/**
 * @brief Y2K/Winamp theme - the single polished default theme
 * 
 * Authentic late-90s hardware aesthetic with neon accents,
 * chrome bevels, and CRT-inspired canvas effects.
 */
inline ThemeTokens y2kWinmapTokens()
{
    ThemeTokens t;
    
    // Base colors - dark navy foundation
    t.panelBase        = juce::Colour::fromString("#07121A");   // Very dark blue-black
    t.panelAccent      = juce::Colour::fromString("#0E1B24");   // Slightly lighter accent
    t.panelStroke      = juce::Colour::fromString("#1A2832");   // Panel separators
    
    // Chrome/metallic elements
    t.knobCream        = juce::Colour::fromString("#F3E9D4");   // Vintage cream knob face
    t.knobFaceMetal    = juce::Colour::fromString("#313743");   // Brushed metal center
    t.chromeHighlight  = juce::Colour::fromString("#E8F4F8");   // Bright chrome highlight
    t.chromeShadow     = juce::Colour::fromString("#0A0F14");   // Deep bevel shadow
    
    // LED colors - amber/cyan theme
    t.ledAmber         = juce::Colour::fromString("#FFB84C");   // Warm amber LEDs
    t.ledAccent        = juce::Colour::fromString("#00E5FF");   // Cyan accent LEDs
    t.ledOff           = juce::Colour::fromString("#2A2A2A");   // LED off state
    
    // Canvas/CRT colors
    t.gridGreen        = juce::Colour::fromString("#00C870");   // CRT phosphor green
    t.scanlineTint     = juce::Colour::fromRGBA(0,0,0,0x28);    // Subtle scanline overlay
    t.bloomTint        = juce::Colour::fromString("#00C870").withAlpha(0.12f); // Phosphor bloom
    
    // Typography colors
    t.hudText          = juce::Colour::fromString("#E7F6EE");   // High-contrast HUD text
    t.labelText        = juce::Colour::fromString("#B8D4D8");   // Control labels
    t.valueText        = juce::Colour::fromString("#FFB84C");   // Parameter values (amber)
    
    // Realm colors - bright neon accents
    t.realmBlue        = juce::Colour::fromString("#1A9CFF");   // Electric blue
    t.realmPurple      = juce::Colour::fromString("#B77BFF");   // Neon purple
    t.realmYellow      = juce::Colour::fromString("#E3C250");   // Electric yellow
    
    // Visual parameters - authentic Y2K feel
    t.chromeBevelDepth = 7.0f;         // Thick chrome bevels
    t.specularAlpha = 0.9f;            // Bright specular highlights
    t.scanlineOpacity = 0.15f;         // Subtle but visible scanlines
    t.bloomRadius = 0.6f;              // Moderate phosphor bloom
    t.useSpeckleTile = true;           // Enable chrome speckle texture
    
    // Layout parameters
    t.knobSize = 64.0f;                // Chunky knobs
    t.buttonHeight = 28.0f;            // Substantial buttons
    t.panelPadding = 12.0f;            // Generous padding
    t.controlSpacing = 8.0f;           // Clean spacing
    
    return t;
}

/**
 * @brief Corporate/professional fallback theme (accessibility)
 * 
 * Clean, high-contrast theme for accessibility compliance
 * and users preferring traditional professional appearance.
 */
inline ThemeTokens corporateTokens()
{
    ThemeTokens t;
    
    // Neutral grays for professional appearance
    t.panelBase        = juce::Colour::fromString("#2A2D32");
    t.panelAccent      = juce::Colour::fromString("#3A3E44");
    t.panelStroke      = juce::Colour::fromString("#4A4E54");
    
    // Subtle chrome without excessive styling
    t.knobCream        = juce::Colour::fromString("#E8E8E8");
    t.knobFaceMetal    = juce::Colour::fromString("#CCCCCC");
    t.chromeHighlight  = juce::Colour::fromString("#F0F0F0");
    t.chromeShadow     = juce::Colour::fromString("#1A1D22");
    
    // Subtle indicators
    t.ledAmber         = juce::Colour::fromString("#FF9500");
    t.ledAccent        = juce::Colour::fromString("#00A4E6");
    t.ledOff           = juce::Colour::fromString("#4A4A4A");
    
    // Clean canvas
    t.gridGreen        = juce::Colour::fromString("#00AA55");
    t.scanlineTint     = juce::Colour::fromRGBA(0,0,0,0x10);
    t.bloomTint        = juce::Colour::fromString("#00AA55").withAlpha(0.06f);
    
    // High-contrast text
    t.hudText          = juce::Colour::fromString("#FFFFFF");
    t.labelText        = juce::Colour::fromString("#D0D0D0");
    t.valueText        = juce::Colour::fromString("#FF9500");
    
    // Muted realm colors
    t.realmBlue        = juce::Colour::fromString("#0080FF");
    t.realmPurple      = juce::Colour::fromString("#8060FF");
    t.realmYellow      = juce::Colour::fromString("#FFB000");
    
    // Reduced visual styling
    t.chromeBevelDepth = 3.0f;
    t.specularAlpha = 0.4f;
    t.scanlineOpacity = 0.05f;
    t.bloomRadius = 0.2f;
    t.useSpeckleTile = false;
    
    return t;
}

} // namespace scp