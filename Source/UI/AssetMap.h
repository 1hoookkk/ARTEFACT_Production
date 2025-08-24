#pragma once
#include <JuceHeader.h>

namespace sc::assets {

// Central asset registry - defines EXACTLY where each asset goes
struct AssetSpecs {
    // Knob assets - size and usage clearly defined
    struct Knob {
        static constexpr int FACE_SIZE = 64;      // Always 64x64px
        static constexpr int MIN_RENDER = 48;     // Minimum render size
        static constexpr int MAX_RENDER = 128;    // Maximum render size
        static constexpr const char* FACE_KEY = "knob_face_64";
    };

    // LED assets - standardized sizes
    struct LED {
        static constexpr int BLOOM_SIZE = 32;     // Always 32x32px
        static constexpr int CORE_SIZE = 12;      // Core LED size
        static constexpr const char* BLOOM_KEY = "led_bloom_32";
        static constexpr const char* CORE_KEY = "led_core_12";
    };

    // Panel textures - tileable patterns
    struct Panel {
        static constexpr int TILE_SIZE = 256;     // Always 256x256px tileable
        static constexpr const char* MAIN_KEY = "panel_teal_256";
        static constexpr const char* DARK_KEY = "panel_dark_256";
    };

    // Switch assets - specific sizes for specific functions
    struct Switch {
        static constexpr int TOGGLE_W = 48;
        static constexpr int TOGGLE_H = 24;
        static constexpr const char* MYSTERY_KEY = "switch_mystery_48x24";
    };
};

// Asset coordinate system - prevents placement chaos
struct AssetPlacement {
    // Component-specific placement rules
    enum class Component {
        ROTARY_KNOB,        // Uses Knob::FACE_SIZE, centered on control
        PANEL_BACKGROUND,   // Uses Panel::TILE_SIZE, tiled fill
        LED_INDICATOR,      // Uses LED::BLOOM_SIZE, centered on LED position
        TOGGLE_SWITCH       // Uses Switch dimensions, precise positioning
    };

    // Render strategy for each component
    struct RenderStrategy {
        Component component;
        juce::Rectangle<int> bounds;
        float scale = 1.0f;
        juce::RectanglePlacement placement = juce::RectanglePlacement::centred;
    };

    // Generate render strategy based on component type and bounds
    static RenderStrategy forKnob(juce::Rectangle<int> knobBounds) {
        return { Component::ROTARY_KNOB, knobBounds, 1.0f, juce::RectanglePlacement::centred };
    }

    static RenderStrategy forLED(juce::Rectangle<int> ledBounds) {
        return { Component::LED_INDICATOR, ledBounds, 1.0f, juce::RectanglePlacement::centred };
    }

    static RenderStrategy forPanel(juce::Rectangle<int> panelBounds) {
        return { Component::PANEL_BACKGROUND, panelBounds, 1.0f, juce::RectanglePlacement::fillDestination };
    }

    static RenderStrategy forSwitch(juce::Rectangle<int> switchBounds) {
        return { Component::TOGGLE_SWITCH, switchBounds, 1.0f, juce::RectanglePlacement::centred };
    }
};

// Asset validation - ensures assets meet specs before using
class AssetValidator {
public:
    static bool validateKnobFace(const juce::Image& img) {
        return img.isValid() 
            && img.getWidth() == AssetSpecs::Knob::FACE_SIZE
            && img.getHeight() == AssetSpecs::Knob::FACE_SIZE
            && img.hasAlphaChannel();
    }

    static bool validateLEDBloom(const juce::Image& img) {
        return img.isValid()
            && img.getWidth() == AssetSpecs::LED::BLOOM_SIZE
            && img.getHeight() == AssetSpecs::LED::BLOOM_SIZE
            && img.hasAlphaChannel();
    }

    static bool validatePanelTexture(const juce::Image& img) {
        return img.isValid()
            && img.getWidth() == AssetSpecs::Panel::TILE_SIZE
            && img.getHeight() == AssetSpecs::Panel::TILE_SIZE;
    }

    static bool validateSwitch(const juce::Image& img) {
        return img.isValid()
            && img.getWidth() == AssetSpecs::Switch::TOGGLE_W
            && img.getHeight() == AssetSpecs::Switch::TOGGLE_H
            && img.hasAlphaChannel();
    }
};

} // namespace sc::assets