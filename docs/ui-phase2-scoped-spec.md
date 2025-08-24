# SpectralCanvas Pro: Phase 2 UI Specification (Minimal but Musical)

**Version:** Phase 2 Scoped  
**Date:** August 24, 2025  
**Status:** Production-Ready Specification  
**Target:** Post-MVP, Pre-3D Complex Features

---

## Design Philosophy: "Minimal but Musical"

### Core Principle
Provide **just enough visual intelligence** to make harmonic relationships obvious and the drum workflow intuitive, without overwhelming first-time users or looking like a spaceship cockpit.

### Visual Identity
- **Clean & Approachable:** Flat 2D interface, not intimidating
- **Musically Intelligent:** Every stroke has clear pitch/scale context
- **Drum-Focused:** Transient vs harmonic visually separated
- **Instantly Memorable:** "Cyan = punch, orange = body" visual language

---

## Canvas Core (Flat 2D Spectrogram)

### Base Visualization
```
Layout:         Standard 2D spectrogram
X-axis:         Time (scrollable, zoomable)
Y-axis:         Frequency (20Hz to 20kHz, logarithmic scale)
Intensity:      Brightness/opacity (not 3D height)
Background:     Deep black (#0A0908) for maximum contrast
```

### Paint Stroke Rendering
```
Blend Mode:     Additive glow
Effect:         2px gaussian blur + subtle particle trail
Lifetime:       Configurable fade (1-10 seconds)
Visual:         Connected lines, not individual points
Color:          Based on harmonic vs transient content
```

### Key Grid Overlay (Toggleable)
```
Note Lines:     Horizontal guides at piano key frequencies
Style:          
- C notes:      Thick lines (#FF6B35 @ 40%)
- Other notes:  Thin lines (#FF6B35 @ 20%)
Labels:         Note names at left edge (C4, D4, E4...)
Density:        Auto-adjusts based on zoom level
```

---

## Musical Context Overlays

### Piano Roll Sidebar (Collapsible)
```
Position:       Left edge, 88-key range (A0 to C8)
Width:          120px (collapsed), 200px (expanded)
Visual:         Semi-transparent piano keys over canvas
Active Notes:   Painted frequencies cause keys to glow
Color:          Warm amber (#FFA500) for active notes
Interaction:    Click key to focus that frequency
```

### Scale Highlighting System
```
Scale Lanes:    Chosen scale degrees get background glow
- In-scale:     Subtle amber glow (#FFA500 @ 15%)
- Root note:    Pulsing emphasis (#FFA500 @ 25%)
- Out-of-scale: Dimmed appearance (50% opacity)

Scale Selector:
- Root dropdown: C, C#, D... (all 12 notes)
- Mode dropdown: Major, Minor, Dorian, Mixolydian
- Auto-detect:   Checkbox with confidence percentage
```

### Ghost Snapping Preview
```
Trigger:        When painting near a scale note
Visual:         Thin vertical line showing snap target
Color:          Ghost white (#F5F2ED @ 60%)
Behavior:       Appears 20px before snap, guides stroke
Animation:      Subtle pulse to draw attention
```

---

## Transient/Tonal Visual Separation (Drum Focus)

### HPSS Color System
```
Transient Stream (Perc):
- Color:        Electric Cyan (#00D4FF)
- Visual:       Sharp, high-contrast edges
- Rendering:    Crisp lines, no blur

Harmonic Stream (Tonal):
- Color:        Burnt Orange (#FF6B35) 
- Visual:       Smooth, warm gradients
- Rendering:    Soft edges, subtle glow

Mixed Content:
- Color:        Gradient blend of both
- Visual:       Smooth transition between sharp/soft
```

### Perc/Harm Balance Visualization
```
Canvas Response:   Emphasis shifts based on balance knob
- More Perc (0.0): Canvas shows more cyan, sharper details
- Balanced (0.5):  Equal emphasis on both streams
- More Harm (1.0): Canvas shows more orange, smoother forms

Real-time Update: Canvas recolors smoothly during knob adjustment
No Artifacts:     Smooth interpolation, no visual glitches
```

---

## Simple Harmonic Feedback

### Overtone Guide Lines
```
Trigger:        When painting a clear fundamental frequency
Visual:         Thin, faint vertical guides at 2f, 3f, 4f, 5f
Color:          Desaturated orange (#FF6B35 @ 30%)
Labels:         Small text: "2nd", "3rd", "4th", "5th" 
Behavior:       Appear during painting, fade after 2 seconds
```

### Chord Recognition Overlay
```
Detection:      When ≥3 tones align within tolerance
Display:        Small text label at top center of canvas
Format:         "Gm", "Cmaj7", "F#dim", etc.
Color:          Amber (#FFA500)
Position:       Non-intrusive, easily ignored if not needed
Update:         Real-time as painting evolves
```

---

## Export Pod Integration (Enhanced)

### Real-Time Waveform Preview
```
Position:       Bottom panel, always visible
Waveform:       Mini visualization of current audio state
Update:         Near real-time as user paints (30fps target)
Color:          Matches canvas color system (cyan/orange mix)
Interaction:    Visual reinforcement of "what you hear is what you get"
```

### Export Interface
```
Layout:
┌─────────────────────────────────────────────────┐
│  [CAPTURE]  ♫ snare_Gm_vintage.wav             │
│   ◉ Ready   ████████▒▒ -2.1dB                  │
│   Preview:  ▁▂▃▆█▆▃▂▁  ═══════ DRAG →          │
└─────────────────────────────────────────────────┘

Elements:
- CAPTURE: Prominent button with LED ring
- Status: Clear indication of ready/rendering state
- Waveform: Real-time preview that updates with painting
- Drag Handle: Obvious visual cue for drag-to-DAW
```

---

## Implementation Roadmap

### Phase 2A: Core Visual Upgrade (Week 1)
- Replace basic grid with musical note lines
- Implement cyan/orange HPSS color separation
- Add piano roll sidebar with basic functionality
- Real-time waveform preview in Export Pod

### Phase 2B: Musical Intelligence (Week 2)
- Scale highlighting system
- Ghost snapping preview
- Basic chord recognition display
- Overtone guide lines

### Phase 2C: Polish & Optimization (Week 3)
- Smooth animations and transitions
- Keyboard shortcuts implementation  
- Responsive zoom behavior
- Performance optimization (target 60fps painting)

---

**This Phase 2 UI provides the perfect balance: sophisticated enough to showcase our musical intelligence, simple enough for immediate user adoption.**