#pragma once
#include <JuceHeader.h>

namespace sc::assets {

class AssetProcessor 
{
public:
    // Process assets to fix transparency and sizing issues
    static juce::Image processKnobAsset(const juce::Image& source)
    {
        if (!source.isValid()) return juce::Image();
        
        // Create properly sized output with alpha
        juce::Image processed(juce::Image::ARGB, 64, 64, true);
        juce::Graphics g(processed);
        
        // Scale source to fit 64x64
        g.drawImageWithin(source, 0, 0, 64, 64, juce::RectanglePlacement::centred);
        
        // Remove background by making it transparent
        // Detect background color (usually gray/beige corners)
        auto cornerColor = source.getPixelAt(0, 0);
        
        for (int y = 0; y < processed.getHeight(); ++y) {
            for (int x = 0; x < processed.getWidth(); ++x) {
                auto pixel = processed.getPixelAt(x, y);
                
                // If pixel is close to background color, make transparent
                if (colorDistance(pixel, cornerColor) < 0.3f) {
                    processed.setPixelAt(x, y, juce::Colours::transparentBlack);
                }
            }
        }
        
        return processed;
    }
    
    static juce::Image processSwitchAsset(const juce::Image& source)
    {
        if (!source.isValid()) return juce::Image();
        
        // Create properly sized output with alpha  
        juce::Image processed(juce::Image::ARGB, 48, 24, true);
        juce::Graphics g(processed);
        
        // Scale and center the switch
        g.drawImageWithin(source, 0, 0, 48, 24, juce::RectanglePlacement::centred);
        
        // Remove background (beige/tan color)
        auto bgColor = source.getPixelAt(0, 0);  // Corner should be background
        
        for (int y = 0; y < processed.getHeight(); ++y) {
            for (int x = 0; x < processed.getWidth(); ++x) {
                auto pixel = processed.getPixelAt(x, y);
                
                // More aggressive background removal for switch
                if (colorDistance(pixel, bgColor) < 0.4f) {
                    processed.setPixelAt(x, y, juce::Colours::transparentBlack);
                }
            }
        }
        
        return processed;
    }
    
    static juce::Image processPanelTexture(const juce::Image& source)
    {
        if (!source.isValid()) return juce::Image();
        
        // Panel texture should be solid (no alpha needed)
        juce::Image processed(juce::Image::RGB, 256, 256, false);
        juce::Graphics g(processed);
        
        // Scale to exact tile size
        g.drawImageTransformed(source, 
            juce::AffineTransform::scale(256.0f / source.getWidth(), 256.0f / source.getHeight()));
        
        return processed;
    }

    // Create LED bloom from the knob asset (extract the orange center)
    static juce::Image createLEDFromKnob(const juce::Image& knobSource)
    {
        if (!knobSource.isValid()) return juce::Image();
        
        juce::Image ledBloom(juce::Image::ARGB, 32, 32, true);
        juce::Graphics g(ledBloom);
        
        // Find the orange/amber center area
        auto center = juce::Point<int>(knobSource.getWidth() / 2, knobSource.getHeight() / 2);
        auto orangeColor = knobSource.getPixelAt(center.x, center.y);
        
        // Create radial gradient based on extracted color
        juce::ColourGradient gradient(
            orangeColor.withAlpha(1.0f), 16, 16,        // Center
            orangeColor.withAlpha(0.0f), 16, 32, true   // Edge
        );
        
        g.setGradientFill(gradient);
        g.fillEllipse(0, 0, 32, 32);
        
        return ledBloom;
    }
    
    // Split dual-state button asset (left=off, right=on)
    static std::pair<juce::Image, juce::Image> processDualStateButton(const juce::Image& source)
    {
        if (!source.isValid()) return {juce::Image(), juce::Image()};
        
        // Assume the image contains two states side by side
        int halfWidth = source.getWidth() / 2;
        int height = source.getHeight();
        
        // Extract left half (OFF state) 
        juce::Image offState(juce::Image::ARGB, halfWidth, height, true);
        juce::Graphics gOff(offState);
        gOff.drawImage(source, 0, 0, halfWidth, height, 0, 0, halfWidth, height);
        
        // Extract right half (ON state)
        juce::Image onState(juce::Image::ARGB, halfWidth, height, true); 
        juce::Graphics gOn(onState);
        gOn.drawImage(source, 0, 0, halfWidth, height, halfWidth, 0, halfWidth, height);
        
        // Remove backgrounds from both states
        auto bgColor = source.getPixelAt(5, 5);  // Sample background from corner
        
        for (auto& state : {std::ref(offState), std::ref(onState)}) {
            for (int y = 0; y < state.get().getHeight(); ++y) {
                for (int x = 0; x < state.get().getWidth(); ++x) {
                    auto pixel = state.get().getPixelAt(x, y);
                    if (colorDistance(pixel, bgColor) < 0.35f) {
                        state.get().setPixelAt(x, y, juce::Colours::transparentBlack);
                    }
                }
            }
        }
        
        return {offState, onState};
    }

private:
    // Calculate color distance for background removal
    static float colorDistance(juce::Colour c1, juce::Colour c2)
    {
        auto dr = c1.getRed() - c2.getRed();
        auto dg = c1.getGreen() - c2.getGreen(); 
        auto db = c1.getBlue() - c2.getBlue();
        return std::sqrt(dr*dr + dg*dg + db*db) / 255.0f;
    }
};

} // namespace sc::assets