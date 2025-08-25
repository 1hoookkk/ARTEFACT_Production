# Phase 2 UI Implementation Guide

**Version:** Phase 2 Scoped  
**Date:** August 24, 2025  
**Status:** Implementation Guide  
**Target:** SpectralCanvas Pro Integration

---

## Overview

This guide provides step-by-step instructions for integrating the Phase 2 UI components into the existing SpectralCanvas Pro codebase. The implementation follows the "Minimal but Musical" design philosophy and provides sophisticated musical intelligence while maintaining simplicity.

---

## New Components Created

### 1. Musical Grid Overlay (`MusicalGridOverlay.h/cpp`)
- **Purpose:** Replaces basic grid with piano key frequency guides
- **Features:** Scale highlighting, ghost snapping preview, overtone guides
- **Integration:** Overlay on top of existing canvas

### 2. Enhanced Piano Roll (`EnhancedPianoRollComponent.h/cpp`)
- **Purpose:** Collapsible 88-key piano roll with scale highlighting
- **Features:** Interactive key selection, frequency focus, scale visualization
- **Integration:** Left sidebar component

### 3. Chord Recognition (`ChordRecognitionOverlay.h/cpp`)
- **Purpose:** Detects and displays chord names when ≥3 tones align
- **Features:** Non-intrusive display, configurable tolerance
- **Integration:** Overlay component

### 4. Enhanced Export Pod (`EnhancedExportPod.h/cpp`)
- **Purpose:** Real-time waveform preview with improved visual feedback
- **Features:** Drag-to-DAW functionality, audio level metering
- **Integration:** Bottom panel component

### 5. Scale Selector (`ScaleSelectorComponent.h/cpp`)
- **Purpose:** Root note and mode selection with auto-detect
- **Features:** Dropdown selectors, confidence indicator
- **Integration:** Control panel component

---

## Integration Steps

### Step 1: Update Color Palette

The color palette has been expanded in `Source/GUI/Colors.h` to include all Phase 2 colors:

```cpp
// HPSS Color System (Drum Focus)
inline const juce::Colour PercColor     = juce::Colour::fromRGB (0, 212, 255);  // Electric Cyan #00D4FF
inline const juce::Colour HarmColor     = juce::Colour::fromRGB (255, 107, 53);  // Burnt Orange #FF6B35

// Musical Context Colors
inline const juce::Colour NoteLineC     = juce::Colour::fromRGB (255, 107, 53); // #FF6B35 @ 40%
inline const juce::Colour ScaleInKey    = juce::Colour::fromRGB (255, 165, 0);  // #FFA500 @ 15%
inline const juce::Colour GhostSnap     = juce::Colour::fromRGB (245, 242, 237); // #F5F2ED @ 60%
```

### Step 2: Integrate Musical Grid Overlay

Add to `CanvasComponent.h`:

```cpp
#include "GUI/MusicalGridOverlay.h"

class CanvasComponent : public juce::Component
{
private:
    std::unique_ptr<MusicalGridOverlay> musicalGridOverlay;
    
public:
    // Add these methods
    void setScale(int rootNote, int scaleMode);
    void setGhostSnapTarget(float frequency);
    void setOvertoneGuides(float fundamentalFreq);
};
```

Add to `CanvasComponent.cpp`:

```cpp
CanvasComponent::CanvasComponent()
{
    // ... existing code ...
    
    // Create musical grid overlay
    musicalGridOverlay = std::make_unique<MusicalGridOverlay>();
    addAndMakeVisible(musicalGridOverlay.get());
}

void CanvasComponent::resized()
{
    // ... existing code ...
    
    // Position musical grid overlay
    if (musicalGridOverlay)
        musicalGridOverlay->setBounds(getLocalBounds());
}

void CanvasComponent::paint(juce::Graphics& g)
{
    // ... existing canvas painting code ...
    
    // Musical grid overlay will paint on top automatically
}
```

### Step 3: Integrate Enhanced Piano Roll

Add to main component (e.g., `MainComponent.h`):

```cpp
#include "GUI/EnhancedPianoRollComponent.h"

class MainComponent : public juce::Component
{
private:
    std::unique_ptr<EnhancedPianoRollComponent> pianoRoll;
    
public:
    void setScale(int rootNote, int scaleMode);
    void setActiveNotes(const std::vector<int>& notes);
};
```

Add to main component implementation:

```cpp
MainComponent::MainComponent()
{
    // ... existing code ...
    
    // Create piano roll
    pianoRoll = std::make_unique<EnhancedPianoRollComponent>();
    pianoRoll->onKeySelected = [this](float frequency) {
        // Handle key selection - focus canvas on this frequency
        if (canvasComponent)
        {
            // Implement frequency focus logic
        }
    };
    addAndMakeVisible(pianoRoll.get());
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    
    // Position piano roll on left
    if (pianoRoll)
    {
        int pianoWidth = pianoRoll->isCollapsed() ? 120 : 200;
        pianoRoll->setBounds(bounds.removeFromLeft(pianoWidth));
    }
    
    // Position canvas in remaining space
    if (canvasComponent)
        canvasComponent->setBounds(bounds);
}
```

### Step 4: Integrate Chord Recognition

Add to `CanvasComponent.h`:

```cpp
#include "GUI/ChordRecognitionOverlay.h"

class CanvasComponent : public juce::Component
{
private:
    std::unique_ptr<ChordRecognitionOverlay> chordRecognition;
    
public:
    void updateActiveTones(const std::vector<float>& frequencies);
};
```

Add to `CanvasComponent.cpp`:

```cpp
CanvasComponent::CanvasComponent()
{
    // ... existing code ...
    
    // Create chord recognition overlay
    chordRecognition = std::make_unique<ChordRecognitionOverlay>();
    addAndMakeVisible(chordRecognition.get());
}

void CanvasComponent::resized()
{
    // ... existing code ...
    
    // Position chord recognition at top
    if (chordRecognition)
        chordRecognition->setBounds(getLocalBounds().removeFromTop(30));
}
```

### Step 5: Integrate Enhanced Export Pod

Replace existing export component with enhanced version:

```cpp
#include "GUI/EnhancedExportPod.h"

class MainComponent : public juce::Component
{
private:
    std::unique_ptr<EnhancedExportPod> exportPod;
    
public:
    void updateExportState(bool ready, bool rendering, float levelDb);
    void updateWaveformPreview(const juce::AudioBuffer<float>& buffer);
};
```

Implementation:

```cpp
MainComponent::MainComponent()
{
    // ... existing code ...
    
    // Create enhanced export pod
    exportPod = std::make_unique<EnhancedExportPod>();
    exportPod->onCaptureRequested = [this]() {
        // Handle capture request
        triggerAudioCapture();
    };
    addAndMakeVisible(exportPod.get());
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    
    // Position export pod at bottom
    if (exportPod)
    {
        int exportHeight = 60;
        exportPod->setBounds(bounds.removeFromBottom(exportHeight));
    }
    
    // ... rest of layout ...
}
```

### Step 6: Integrate Scale Selector

Add to control panel or header:

```cpp
#include "GUI/ScaleSelectorComponent.h"

class MainComponent : public juce::Component
{
private:
    std::unique_ptr<ScaleSelectorComponent> scaleSelector;
    
public:
    void setScale(int rootNote, int scaleMode);
    void setAutoDetectConfidence(float confidence);
};
```

Implementation:

```cpp
MainComponent::MainComponent()
{
    // ... existing code ...
    
    // Create scale selector
    scaleSelector = std::make_unique<ScaleSelectorComponent>();
    scaleSelector->onScaleChanged = [this](int root, int mode) {
        // Update all components with new scale
        setScale(root, mode);
    };
    addAndMakeVisible(scaleSelector.get());
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    
    // Position scale selector in header/control area
    if (scaleSelector)
    {
        int selectorHeight = 35;
        scaleSelector->setBounds(bounds.removeFromTop(selectorHeight));
    }
    
    // ... rest of layout ...
}
```

---

## HPSS Color Integration

### Update Paint Rendering

Modify the paint stroke rendering in `CanvasComponent.cpp` to use HPSS colors:

```cpp
void CanvasComponent::drawPaintStroke(juce::Graphics& g, const PaintStroke& stroke)
{
    // Determine if stroke is more transient or harmonic
    float transientRatio = calculateTransientRatio(stroke);
    
    // Choose color based on content
    juce::Colour strokeColor;
    if (transientRatio > 0.7f)
    {
        strokeColor = UIColour::PercColor; // Electric Cyan for transient
    }
    else if (transientRatio < 0.3f)
    {
        strokeColor = UIColour::HarmColor; // Burnt Orange for harmonic
    }
    else
    {
        // Blend colors for mixed content
        strokeColor = UIColour::PercColor.interpolatedWith(UIColour::HarmColor, 1.0f - transientRatio);
    }
    
    // Apply stroke rendering with color
    g.setColour(strokeColor);
    // ... existing stroke rendering code ...
}
```

### Add Transient/Harmonic Balance Control

Add to control panel:

```cpp
void MainComponent::setPercHarmBalance(float balance)
{
    // Update canvas rendering emphasis
    if (canvasComponent)
    {
        // Implement balance-based rendering emphasis
        // 0.0 = more transient (cyan), 1.0 = more harmonic (orange)
    }
}
```

---

## Real-time Waveform Preview

### Connect Audio Engine

Add to audio processing chain:

```cpp
void MainComponent::audioBufferUpdated(const juce::AudioBuffer<float>& buffer)
{
    // Update waveform preview
    if (exportPod)
        exportPod->updateWaveformPreview(buffer);
    
    // Update audio level
    float rmsLevel = calculateRMSLevel(buffer);
    float levelDb = 20.0f * std::log10(rmsLevel);
    
    if (exportPod)
        exportPod->setAudioLevel(levelDb);
}
```

---

## Performance Considerations

### Optimization Strategies

1. **Lazy Rendering:** Only update musical grid when scale changes
2. **Cached Calculations:** Pre-calculate piano key frequencies
3. **Efficient Overlays:** Use non-opaque components for overlays
4. **Frame Rate Control:** Limit waveform updates to 30fps

### Memory Management

```cpp
// Use RAII for component management
std::unique_ptr<MusicalGridOverlay> musicalGridOverlay;
std::unique_ptr<EnhancedPianoRollComponent> pianoRoll;
std::unique_ptr<ChordRecognitionOverlay> chordRecognition;
std::unique_ptr<EnhancedExportPod> exportPod;
std::unique_ptr<ScaleSelectorComponent> scaleSelector;
```

---

## Testing and Validation

### Visual Testing Checklist

- [ ] Piano key frequency guides align with audio
- [ ] Scale highlighting updates correctly
- [ ] Ghost snapping preview appears near scale notes
- [ ] Overtone guides show during painting
- [ ] Chord recognition displays accurate names
- [ ] HPSS colors reflect transient/harmonic content
- [ ] Real-time waveform preview updates smoothly
- [ ] Export pod shows correct status and levels

### Performance Testing

- [ ] 60fps painting performance maintained
- [ ] Memory usage remains stable
- [ ] Audio latency unaffected
- [ ] UI responsiveness preserved

---

## Future Enhancements

### Phase 2B Features (Week 2)
- Advanced scale detection algorithms
- More sophisticated chord recognition
- Enhanced overtone visualization
- Keyboard shortcuts for all features

### Phase 2C Features (Week 3)
- Smooth animations and transitions
- Advanced performance optimizations
- Additional scale modes
- Custom color themes

---

**This implementation provides the foundation for the Phase 2 UI while maintaining the existing codebase structure and performance characteristics.**
