#pragma once
#include <JuceHeader.h>
#include "LayoutSpec.h"

// ==============================================================================
// ENHANCED 9-SLICE RENDERER
// Pixel-perfect 9-slice image rendering with non-distorting scaling
// ==============================================================================

class NineSliceRenderer
{
public:
    // Draw 9-slice with pixel-perfect snapping
    static void draw(juce::Graphics& g, 
                    const juce::Image& image,
                    juce::Rectangle<float> destBounds,
                    const Layout::Nine& margins,
                    float opacity = 1.0f)
    {
        if (image.isNull() || destBounds.isEmpty())
            return;
        
        // Snap destination to pixel boundaries
        auto dest = Layout::snap(destBounds);
        
        // Source image dimensions
        const int srcW = image.getWidth();
        const int srcH = image.getHeight();
        
        // Validate margins don't exceed image size
        if (margins.l + margins.r >= srcW || margins.t + margins.b >= srcH)
        {
            // Fallback to simple stretch if margins are invalid
            g.setOpacity(opacity);
            g.drawImage(image, dest.toFloat());
            return;
        }
        
        // Calculate slice positions in source image
        const int srcLeft = margins.l;
        const int srcRight = srcW - margins.r;
        const int srcTop = margins.t;
        const int srcBottom = srcH - margins.b;
        
        // Calculate destination positions
        const int destLeft = dest.getX() + margins.l;
        const int destRight = dest.getRight() - margins.r;
        const int destTop = dest.getY() + margins.t;
        const int destBottom = dest.getBottom() - margins.b;
        
        // Ensure center area has positive size
        if (destLeft >= destRight || destTop >= destBottom)
        {
            // Destination too small, just draw corners
            g.setOpacity(opacity);
            drawCorners(g, image, dest, margins);
            return;
        }
        
        g.setOpacity(opacity);
        
        // Draw 9 slices
        // Top-left corner (no scaling)
        drawSlice(g, image,
                 juce::Rectangle<int>(0, 0, srcLeft, srcTop),
                 juce::Rectangle<int>(dest.getX(), dest.getY(), margins.l, margins.t),
                 false);
        
        // Top-center (horizontal stretch only)
        drawSlice(g, image,
                 juce::Rectangle<int>(srcLeft, 0, srcRight - srcLeft, srcTop),
                 juce::Rectangle<int>(destLeft, dest.getY(), destRight - destLeft, margins.t),
                 true, false);
        
        // Top-right corner (no scaling)
        drawSlice(g, image,
                 juce::Rectangle<int>(srcRight, 0, margins.r, srcTop),
                 juce::Rectangle<int>(destRight, dest.getY(), margins.r, margins.t),
                 false);
        
        // Middle-left (vertical stretch only)
        drawSlice(g, image,
                 juce::Rectangle<int>(0, srcTop, srcLeft, srcBottom - srcTop),
                 juce::Rectangle<int>(dest.getX(), destTop, margins.l, destBottom - destTop),
                 false, true);
        
        // Center (both directions stretch)
        drawSlice(g, image,
                 juce::Rectangle<int>(srcLeft, srcTop, srcRight - srcLeft, srcBottom - srcTop),
                 juce::Rectangle<int>(destLeft, destTop, destRight - destLeft, destBottom - destTop),
                 true, true);
        
        // Middle-right (vertical stretch only)
        drawSlice(g, image,
                 juce::Rectangle<int>(srcRight, srcTop, margins.r, srcBottom - srcTop),
                 juce::Rectangle<int>(destRight, destTop, margins.r, destBottom - destTop),
                 false, true);
        
        // Bottom-left corner (no scaling)
        drawSlice(g, image,
                 juce::Rectangle<int>(0, srcBottom, srcLeft, margins.b),
                 juce::Rectangle<int>(dest.getX(), destBottom, margins.l, margins.b),
                 false);
        
        // Bottom-center (horizontal stretch only)
        drawSlice(g, image,
                 juce::Rectangle<int>(srcLeft, srcBottom, srcRight - srcLeft, margins.b),
                 juce::Rectangle<int>(destLeft, destBottom, destRight - destLeft, margins.b),
                 true, false);
        
        // Bottom-right corner (no scaling)
        drawSlice(g, image,
                 juce::Rectangle<int>(srcRight, srcBottom, margins.r, margins.b),
                 juce::Rectangle<int>(destRight, destBottom, margins.r, margins.b),
                 false);
        
        g.setOpacity(1.0f);
    }
    
    // Draw with tiling instead of stretching for center areas
    static void drawTiled(juce::Graphics& g,
                         const juce::Image& image,
                         juce::Rectangle<float> destBounds,
                         const Layout::Nine& margins,
                         float opacity = 1.0f)
    {
        if (image.isNull() || destBounds.isEmpty())
            return;
        
        // Snap to pixels
        auto dest = Layout::snap(destBounds);
        
        // Draw corners and edges normally
        draw(g, image, dest.toFloat(), margins, opacity);
        
        // Override center with tiled version
        const int srcW = image.getWidth();
        const int srcH = image.getHeight();
        
        const int srcLeft = margins.l;
        const int srcRight = srcW - margins.r;
        const int srcTop = margins.t;
        const int srcBottom = srcH - margins.b;
        
        const int destLeft = dest.getX() + margins.l;
        const int destRight = dest.getRight() - margins.r;
        const int destTop = dest.getY() + margins.t;
        const int destBottom = dest.getBottom() - margins.b;
        
        if (destLeft < destRight && destTop < destBottom)
        {
            // Get center slice
            auto centerSlice = image.getClippedImage(
                juce::Rectangle<int>(srcLeft, srcTop, 
                                   srcRight - srcLeft, 
                                   srcBottom - srcTop));
            
            // Tile it in the center area
            g.setTiledImageFill(centerSlice, destLeft, destTop, opacity);
            g.fillRect(destLeft, destTop, destRight - destLeft, destBottom - destTop);
        }
    }
    
private:
    static void drawSlice(juce::Graphics& g,
                         const juce::Image& source,
                         juce::Rectangle<int> srcRect,
                         juce::Rectangle<int> destRect,
                         bool stretchH = true,
                         bool stretchV = true)
    {
        if (srcRect.isEmpty() || destRect.isEmpty())
            return;
        
        auto slice = source.getClippedImage(srcRect);
        
        if (!stretchH && !stretchV)
        {
            // Draw without scaling (corners)
            g.drawImageAt(slice, destRect.getX(), destRect.getY());
        }
        else
        {
            // Draw with appropriate stretching
            auto placement = stretchH && stretchV ? 
                           juce::RectanglePlacement::stretchToFit :
                           stretchH ? juce::RectanglePlacement::xLeft | 
                                    juce::RectanglePlacement::yTop | 
                                    juce::RectanglePlacement::doNotResize :
                                    juce::RectanglePlacement::xLeft | 
                                    juce::RectanglePlacement::yTop | 
                                    juce::RectanglePlacement::doNotResize;
            
            g.drawImage(slice, destRect.toFloat(), placement);
        }
    }
    
    static void drawCorners(juce::Graphics& g,
                          const juce::Image& image,
                          juce::Rectangle<int> dest,
                          const Layout::Nine& margins)
    {
        const int srcW = image.getWidth();
        const int srcH = image.getHeight();
        
        // Just draw the four corners
        // Top-left
        auto tl = image.getClippedImage({0, 0, margins.l, margins.t});
        g.drawImageAt(tl, dest.getX(), dest.getY());
        
        // Top-right
        auto tr = image.getClippedImage({srcW - margins.r, 0, margins.r, margins.t});
        g.drawImageAt(tr, dest.getRight() - margins.r, dest.getY());
        
        // Bottom-left
        auto bl = image.getClippedImage({0, srcH - margins.b, margins.l, margins.b});
        g.drawImageAt(bl, dest.getX(), dest.getBottom() - margins.b);
        
        // Bottom-right
        auto br = image.getClippedImage({srcW - margins.r, srcH - margins.b, margins.r, margins.b});
        g.drawImageAt(br, dest.getRight() - margins.r, dest.getBottom() - margins.b);
    }
};

// ==============================================================================
// Convenience function for backward compatibility
// ==============================================================================
inline void drawNineSliceEnhanced(juce::Graphics& g, 
                                 const juce::Image& image,
                                 juce::Rectangle<int> bounds,
                                 const Layout::Nine& margins,
                                 float opacity = 1.0f)
{
    NineSliceRenderer::draw(g, image, bounds.toFloat(), margins, opacity);
}