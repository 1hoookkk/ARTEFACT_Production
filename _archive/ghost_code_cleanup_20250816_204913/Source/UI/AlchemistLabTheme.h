/******************************************************************************
 * File: AlchemistLabTheme.h
 * Description: Enhanced professional theme system for SpectralCanvas Pro
 * 
 * Combines E-mu Audity heritage with modern spectral visualization.
 * Professional dark theme with vintage hardware elements.
 * 
 * Copyright (c) 2025 Spectral Audio Systems
 ******************************************************************************/

#pragma once
#include <JuceHeader.h>

/**
 * Professional theme system for SpectralCanvas Pro
 * Inspired by classic hardware samplers and modern DAWs
 */
namespace AlchemistLabTheme
{
    //==============================================================================
    // Professional Color Palette
    //==============================================================================
    
    namespace Colors
    {
        // Main Background Colors
        static constexpr uint32_t BackgroundMain      = 0xFF0A0A0A;  // Deep black
        static constexpr uint32_t BackgroundPanel     = 0xFF1A1A1A;  // Panel gray
        static constexpr uint32_t BackgroundControl   = 0xFF2A2A2A;  // Control surface
        static constexpr uint32_t BackgroundHighlight = 0xFF3A3A3A;  // Hover state
        
        // E-mu Heritage Colors
        static constexpr uint32_t EmuLcdGreen         = 0xFF00FF41;  // Classic LCD green
        static constexpr uint32_t EmuAmber            = 0xFFFFB000;  // Hardware amber
        static constexpr uint32_t EmuRed              = 0xFFFF2040;  // Error/warning red
        
        // Spectral Visualization Colors
        static constexpr uint32_t SpectralCyan        = 0xFF00AAFF;  // Primary spectral
        static constexpr uint32_t SpectralBlue        = 0xFF0066CC;  // Secondary spectral
        static constexpr uint32_t SpectralPurple      = 0xFF8800FF;  // Accent spectral
        static constexpr uint32_t SpectralGlow        = 0x4000AAFF;  // Glow effect
        
        // Professional UI Elements
        static constexpr uint32_t ButtonNormal        = 0xFF404040;  // Raised button
        static constexpr uint32_t ButtonPressed       = 0xFF606060;  // Pressed state
        static constexpr uint32_t ButtonActive        = 0xFF00AAFF;  // Active/selected
        
        static constexpr uint32_t BorderNormal        = 0xFF555555;  // Standard border
        static constexpr uint32_t BorderActive        = 0xFF00AAFF;  // Active border
        static constexpr uint32_t BorderInset         = 0xFF222222;  // Inset shadow
        
        // Text Colors
        static constexpr uint32_t TextPrimary         = 0xFFEEEEEE;  // Main text
        static constexpr uint32_t TextSecondary       = 0xFFBBBBBB;  // Secondary text
        static constexpr uint32_t TextDisabled        = 0xFF666666;  // Disabled text
        static constexpr uint32_t TextHighlight       = 0xFF00AAFF;  // Highlighted text
        static constexpr uint32_t TextTerminal        = 0xFF00FF41;  // Terminal/debug text
        
        // Status Indicators
        static constexpr uint32_t StatusOk            = 0xFF00FF41;  // Success/ready
        static constexpr uint32_t StatusWarning       = 0xFFFFB000;  // Warning
        static constexpr uint32_t StatusError         = 0xFFFF2040;  // Error/critical
        static constexpr uint32_t StatusProcessing    = 0xFF00AAFF;  // Processing/active
        
        // Sample Slot Colors
        static constexpr uint32_t SlotEmpty           = 0xFF2A2A2A;  // Empty slot
        static constexpr uint32_t SlotLoaded          = 0xFF404040;  // Sample loaded
        static constexpr uint32_t SlotActive          = 0xFF00AAFF;  // Currently playing
        static constexpr uint32_t SlotDragOver        = 0xFF8800FF;  // Drag over state
        
        // Waveform Visualization
        static constexpr uint32_t WaveformLine        = 0xFF00AAFF;  // Waveform outline
        static constexpr uint32_t WaveformFill        = 0x4000AAFF;  // Waveform fill
        static constexpr uint32_t WaveformGrid        = 0xFF333333;  // Grid lines
        static constexpr uint32_t WaveformMarker      = 0xFFFFB000;  // Position markers
    }
    
    //==============================================================================
    // Typography System
    //==============================================================================
    
    namespace Fonts
    {
        // Font sizes for different UI elements
        static constexpr float HeaderSize     = 14.0f;
        static constexpr float ButtonSize     = 11.0f;
        static constexpr float LabelSize      = 10.0f;
        static constexpr float SmallSize      = 9.0f;
        static constexpr float TinySize       = 8.0f;
        
        // Get professional fonts with fallbacks
        static juce::Font getHeaderFont()   { return juce::Font("Tahoma", HeaderSize, juce::Font::bold); }
        static juce::Font getButtonFont()   { return juce::Font("Tahoma", ButtonSize, juce::Font::plain); }
        static juce::Font getLabelFont()    { return juce::Font("Tahoma", LabelSize, juce::Font::plain); }
        static juce::Font getSmallFont()    { return juce::Font("Tahoma", SmallSize, juce::Font::plain); }
        static juce::Font getMonospaceFont(){ return juce::Font(juce::Font::getDefaultMonospacedFontName(), LabelSize, juce::Font::plain); }
        static juce::Font getLcdFont()      { return juce::Font("Courier New", LabelSize, juce::Font::bold); }
    }
    
    //==============================================================================
    // Component Styling Functions
    //==============================================================================
    
    // Style sample slot with hardware-inspired look
    static void styleSampleSlot(juce::Component& slot, bool isEmpty = true, bool isActive = false)
    {
        // Note: Component doesn't have backgroundColourId - use custom painting instead
        // The slot should handle its own background painting based on state
        if (isActive)
        {
            // slot.currentBackgroundColor = Colors::SlotActive;
        }
        else if (isEmpty)
        {
            // slot.currentBackgroundColor = Colors::SlotEmpty;
        }
        else
        {
            // slot.currentBackgroundColor = Colors::SlotLoaded;
        }
    }
    
    // Style transport buttons with professional look
    static void styleTransportButton(juce::TextButton& button)
    {
        button.setColour(juce::TextButton::buttonColourId, juce::Colour(Colors::ButtonNormal));
        button.setColour(juce::TextButton::buttonOnColourId, juce::Colour(Colors::ButtonPressed));
        button.setColour(juce::TextButton::textColourOffId, juce::Colour(Colors::TextPrimary));
        button.setColour(juce::TextButton::textColourOnId, juce::Colour(Colors::TextPrimary));
        // Note: TextButton uses LookAndFeel font, not setFont()
    }
    
    // Style magic wand button with special highlight
    static void styleMagicWandButton(juce::TextButton& button)
    {
        button.setColour(juce::TextButton::buttonColourId, juce::Colour(Colors::SpectralPurple));
        button.setColour(juce::TextButton::buttonOnColourId, juce::Colour(Colors::ButtonActive));
        button.setColour(juce::TextButton::textColourOffId, juce::Colour(Colors::TextPrimary));
        button.setColour(juce::TextButton::textColourOnId, juce::Colour(Colors::TextPrimary));
        // Note: TextButton uses LookAndFeel font, not setFont()
    }
    
    // Style control buttons (SYNC, QUANTIZE, LOOP)
    static void styleControlButton(juce::TextButton& button)
    {
        button.setColour(juce::TextButton::buttonColourId, juce::Colour(Colors::BackgroundControl));
        button.setColour(juce::TextButton::buttonOnColourId, juce::Colour(Colors::ButtonActive));
        button.setColour(juce::TextButton::textColourOffId, juce::Colour(Colors::TextSecondary));
        button.setColour(juce::TextButton::textColourOnId, juce::Colour(Colors::TextPrimary));
        // Note: TextButton uses LookAndFeel font, not setFont()
    }
    
    // Specific hardware-style button functions for compatibility
    static void styleSyncButton(juce::TextButton& button) { styleControlButton(button); }
    static void styleQuantizeButton(juce::TextButton& button) { styleControlButton(button); }
    static void styleLoopButton(juce::TextButton& button) { styleControlButton(button); }
    
    // Style professional sliders
    static void styleSlider(juce::Slider& slider)
    {
        slider.setColour(juce::Slider::backgroundColourId, juce::Colour(Colors::BackgroundControl));
        slider.setColour(juce::Slider::trackColourId, juce::Colour(Colors::SpectralCyan));
        slider.setColour(juce::Slider::thumbColourId, juce::Colour(Colors::TextPrimary));
        slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(Colors::TextPrimary));
        slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(Colors::BackgroundPanel));
        slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(Colors::BorderNormal));
    }
    
    // Hardware-style BPM slider
    static void styleBPMSlider(juce::Slider& slider)
    {
        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
        styleSlider(slider);
    }
    
    // Style status labels
    static void styleStatusLabel(juce::Label& label, bool isActive = false)
    {
        label.setColour(juce::Label::backgroundColourId, juce::Colour(Colors::BackgroundPanel));
        label.setColour(juce::Label::textColourId, isActive ? 
                       juce::Colour(Colors::StatusProcessing) : 
                       juce::Colour(Colors::TextSecondary));
        label.setFont(Fonts::getMonospaceFont());
        label.setJustificationType(juce::Justification::centred);
    }
    
    //==============================================================================
    // Visual Effects
    //==============================================================================
    
    // Draw hardware-style button with beveled edges
    static void drawHardwareButton(juce::Graphics& g, juce::Rectangle<int> bounds, 
                                  bool isPressed, bool isMouseOver, juce::Colour baseColour)
    {
        auto area = bounds.toFloat();
        
        // Base color with slight variation for state
        auto bgColour = baseColour;
        if (isMouseOver && !isPressed)
            bgColour = bgColour.brighter(0.1f);
        else if (isPressed)
            bgColour = bgColour.darker(0.2f);
            
        // Fill base
        g.setColour(bgColour);
        g.fillRoundedRectangle(area, 2.0f);
        
        // Highlight edge (top-left)
        if (!isPressed)
        {
            g.setColour(bgColour.brighter(0.3f));
            g.drawLine(area.getX() + 1, area.getY() + 1, 
                      area.getRight() - 2, area.getY() + 1, 1.0f);
            g.drawLine(area.getX() + 1, area.getY() + 1, 
                      area.getX() + 1, area.getBottom() - 2, 1.0f);
        }
        
        // Shadow edge (bottom-right)
        g.setColour(bgColour.darker(0.4f));
        g.drawLine(area.getX() + 2, area.getBottom() - 1, 
                  area.getRight() - 1, area.getBottom() - 1, 1.0f);
        g.drawLine(area.getRight() - 1, area.getY() + 2, 
                  area.getRight() - 1, area.getBottom() - 1, 1.0f);
        
        // Border
        g.setColour(juce::Colour(Colors::BorderNormal));
        g.drawRoundedRectangle(area, 2.0f, 1.0f);
    }
    
    // Draw LCD-style display with authentic glow
    static void drawLcdDisplay(juce::Graphics& g, juce::Rectangle<int> bounds, 
                              const juce::String& text, bool isActive = true)
    {
        auto area = bounds.toFloat().reduced(1.0f);
        
        // Background
        g.setColour(juce::Colour(0xFF000000));
        g.fillRoundedRectangle(area, 1.0f);
        
        // Inset border
        g.setColour(juce::Colour(Colors::BorderInset));
        g.drawRoundedRectangle(area, 1.0f, 1.0f);
        
        // Text with glow effect if active
        if (isActive)
        {
            // Glow effect
            g.setColour(juce::Colour(Colors::EmuLcdGreen).withAlpha(0.3f));
            for (int i = 0; i < 3; ++i)
            {
                auto glowArea = area.expanded(i);
                g.drawText(text, glowArea.toNearestInt(), 
                          juce::Justification::centred, false);
            }
            
            // Main text
            g.setColour(juce::Colour(Colors::EmuLcdGreen));
        }
        else
        {
            g.setColour(juce::Colour(Colors::TextDisabled));
        }
        
        g.setFont(Fonts::getLcdFont());
        g.drawText(text, area.toNearestInt(), 
                  juce::Justification::centred, false);
    }
    
    // Draw spectral visualization grid
    static void drawSpectralGrid(juce::Graphics& g, juce::Rectangle<int> bounds, 
                                int divisions = 8)
    {
        auto area = bounds.toFloat();
        
        g.setColour(juce::Colour(Colors::WaveformGrid));
        
        // Vertical lines (time)
        for (int i = 1; i < divisions; ++i)
        {
            float x = area.getX() + (area.getWidth() / divisions) * i;
            g.drawVerticalLine(static_cast<int>(x), area.getY(), area.getBottom());
        }
        
        // Horizontal lines (frequency)
        for (int i = 1; i < divisions; ++i)
        {
            float y = area.getY() + (area.getHeight() / divisions) * i;
            g.drawHorizontalLine(static_cast<int>(y), area.getX(), area.getRight());
        }
    }
}