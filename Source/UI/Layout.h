#pragma once
#include <juce_graphics/juce_graphics.h>

namespace ui {

// Normalized layout helpers for responsive, resolution-independent UI
// All functions work with normalized 0-1 coordinates on a square artboard

inline juce::Rectangle<int> squareBox(juce::Rectangle<int> bounds) {
    int side = juce::jmin(bounds.getWidth(), bounds.getHeight());
    return juce::Rectangle<int>(0, 0, side, side).withCentre(bounds.getCentre());
}

inline juce::Rectangle<float> nrect(juce::Rectangle<int> box, float x, float y, float w, float h) {
    return {
        box.getX() + x * box.getWidth(),
        box.getY() + y * box.getHeight(),
        w * box.getWidth(),
        h * box.getHeight()
    };
}

inline juce::Rectangle<float> ncircle(juce::Rectangle<int> box, float cx, float cy, float r) {
    float radius = r * (float)juce::jmin(box.getWidth(), box.getHeight());
    return juce::Rectangle<float>(-radius, -radius, 2 * radius, 2 * radius)
        .withCentre({box.getX() + cx * box.getWidth(), 
                    box.getY() + cy * box.getHeight()});
}

inline juce::Point<float> npoint(juce::Rectangle<int> box, float x, float y) {
    return {box.getX() + x * box.getWidth(), 
            box.getY() + y * box.getHeight()};
}

// Helper for creating multiple circles in a row (e.g., realm buttons)
inline std::array<juce::Rectangle<float>, 3> nrow3(juce::Rectangle<int> box, 
                                                   float y, float diameter,
                                                   float c1, float c2, float c3) {
    float d = diameter * (float)juce::jmin(box.getWidth(), box.getHeight());
    return {
        juce::Rectangle<float>(-d/2, -d/2, d, d).withCentre({box.getX() + c1 * box.getWidth(), box.getY() + y * box.getHeight()}),
        juce::Rectangle<float>(-d/2, -d/2, d, d).withCentre({box.getX() + c2 * box.getWidth(), box.getY() + y * box.getHeight()}),
        juce::Rectangle<float>(-d/2, -d/2, d, d).withCentre({box.getX() + c3 * box.getWidth(), box.getY() + y * box.getHeight()})
    };
}

// Helper for creating multiple rectangles in a row (e.g., action buttons)
inline std::array<juce::Rectangle<float>, 4> nrow4(juce::Rectangle<int> box,
                                                   float y, float w, float h,
                                                   float c1, float c2, float c3, float c4) {
    return {
        nrect(box, c1 - w/2, y - h/2, w, h),
        nrect(box, c2 - w/2, y - h/2, w, h),
        nrect(box, c3 - w/2, y - h/2, w, h),
        nrect(box, c4 - w/2, y - h/2, w, h)
    };
}

} // namespace ui