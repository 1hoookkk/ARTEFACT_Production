#pragma once
#include <JuceHeader.h>

// Lightweight sprite sheet helper for icons/decals
class SpriteSheet {
public:
    SpriteSheet() = default;
    explicit SpriteSheet(juce::Image img, int tileW, int tileH, int margin = 0, int spacing = 0)
        : image(std::move(img)), w(tileW), h(tileH), pad(margin), gap(spacing) {}

    bool isValid() const { return image.isValid(); }

    juce::Image get(int index) const {
        if (!isValid() || w <= 0 || h <= 0) return {};
        const int cols = juce::jmax(1, (image.getWidth()  - 2*pad + gap) / (w + gap));
        const int x = index % cols;
        const int y = index / cols;
        const int px = pad + x * (w + gap);
        const int py = pad + y * (h + gap);
        if (px + w <= image.getWidth() && py + h <= image.getHeight())
            return image.getClippedImage({ px, py, w, h });
        return {};
    }

    void draw(juce::Graphics& g, int index, juce::Rectangle<int> area, float alpha = 1.0f) const {
        if (auto s = get(index); s.isValid()) {
            g.setOpacity(alpha);
            g.drawImageWithin(s, area.getX(), area.getY(), area.getWidth(), area.getHeight(),
                              juce::RectanglePlacement::centred, false);
        }
    }

private:
    juce::Image image;
    int w{0}, h{0}, pad{0}, gap{0};
};