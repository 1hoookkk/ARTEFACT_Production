# SpectralCanvas Pro UI Design Specification
## E-mu Audity 2000 / Xtreme Lead Inspired Interface

## Visual Themes

### Theme 1: Audity (Default)
- **Panel**: #345E78 (Medium blue powder-coat)
- **Bezel**: #3D4349 (Dark metallic gray)
- **Label Text**: #E8F1F4 (Off-white)
- **LCD**: #8DFF6A (Lime green glow)
- **LED Active**: #FF4545 (Bright red)
- **LED Inactive**: #4A1010 (Dark red)
- **Shadow**: rgba(0,0,0,0.35)
- **Highlight**: rgba(255,255,255,0.12)

### Theme 2: Xtreme Lead (Alternative)
- **Panel**: #F0A517 (Safety yellow/orange)
- **Bezel**: #101214 (Deep black)
- **Label Text**: #181A1B (Near black)
- **LCD**: #7FE86A (Green-yellow)
- **LED Active**: #FF7A2F (Orange)
- **LED Inactive**: #4A2010 (Dark orange)
- **Shadow**: rgba(0,0,0,0.45)
- **Highlight**: rgba(255,255,255,0.10)

## Layout Architecture

### Responsive Grid System
```
┌─────────────────────────────────────────────────────┐
│                    TOP BAR (60px)                    │
│  [Logo] [Preset A/B] [Theme] [SR|BUF|CPU] [READY●]  │
├─────────────────────────────────────────────────────┤
│ ROOMS │                                   │ RIGHT   │
│ ───── │          CANVAS VIEW              │ STACK   │
│Canvas │         (Log-freq grid)           │ ─────   │
│Filter │      800x400px paint area         │ Params  │
│ Tube  │    Zoom/Pan/Brush controls        │ Panel   │
│Music  │                                   │         │
│ Forge │                                   │ Controls│
│Rompler│                                   │         │
│(80px) │            (flex)                 │ (280px) │
├─────────────────────────────────────────────────────┤
│                   FOOTER (40px)                      │
│         [Status] [Version] [Performance]             │
└─────────────────────────────────────────────────────┘
```

### Minimum/Maximum Dimensions
- **Minimum**: 900 × 600 pixels
- **Default**: 1200 × 700 pixels  
- **Maximum**: 1600 × 1000 pixels
- **Scaling**: Maintains aspect ratio, components scale proportionally

## Control Specifications

### Detented Rotary Knobs
- **Size**: 48×48px (standard), 32×32px (fine control)
- **Detents**: 41 positions (-20 to +20)
- **Visual**: Brushed metal cap with white indicator line
- **Interaction**: 
  - Click+drag for coarse adjustment
  - Shift+drag for fine adjustment (0.1× speed)
  - Double-click to reset to default
- **Animation**: 120ms ease-out on value changes

### Beveled Buttons
- **Size**: 80×32px (standard), 60×24px (compact)
- **States**: Up (default), Down (pressed), Hover (+5% brightness)
- **LED Pip**: 6×6px indicator in top-right corner
- **Visual**: 2px bevel with highlight/shadow
- **Click feedback**: 50ms down state before release

### LED Meters (12-18 segments)
- **Segment size**: 4×12px with 2px gap
- **Colors**: 
  - -inf to -12dB: Green (#00FF41)
  - -12dB to -6dB: Yellow (#FFD700)
  - -6dB to 0dB: Orange (#FF8C00)
  - Above 0dB: Red (#FF0000)
- **Update rate**: 30Hz
- **Peak hold**: 1500ms with fadeout

### LCD Readouts
- **Font**: Monospace, pixelated (resembling 7-segment)
- **Size**: 14px standard, 18px for main values
- **Glow**: 2px blur radius in theme LCD color
- **Background**: #0A0C0B (near black) with slight inset
- **Update rate**: 15Hz for smooth appearance

## Motion & Animation

### Timing Guidelines
- **Control response**: 120-180ms ease-out curve
- **Panel transitions**: 250ms ease-in-out
- **Hover effects**: 80ms ease-out
- **LED updates**: Instant (no animation)
- **Repaint throttle**: 30Hz minimum, 60Hz maximum

### Easing Functions
```cpp
// Standard ease-out for controls
float easeOut(float t) {
    return 1.0f - std::pow(1.0f - t, 3.0f);
}

// Smooth ease-in-out for panels
float easeInOut(float t) {
    return t < 0.5f 
        ? 4.0f * t * t * t
        : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
}
```

## Texture Requirements

### Base Textures (1× and 2× resolutions)
1. **panel_powdercoat.png**: 256×256px tileable metal texture
2. **bezel_9slice.png**: 48×48px with 12px insets
3. **button_up.png**: 80×32px beveled button
4. **button_down.png**: 80×32px pressed state
5. **knob_cap.png**: 64×64px brushed metal knob
6. **led_on.png**: 8×8px bright LED
7. **led_off.png**: 8×8px dim LED
8. **lcd_strip.png**: 512×32px LCD background
9. **meter_on.png**: 6×14px lit segment
10. **meter_off.png**: 6×14px unlit segment

### Texture Loading Strategy
- All textures loaded once at startup via BinaryData
- Cached in TextureBank singleton
- No runtime file I/O
- Automatic 2× selection for HiDPI displays

## Performance Requirements

### RT-Safety Guarantees
- **Zero allocations** in paint() methods
- **No locks** in UI render path
- **Cached geometry** for complex shapes
- **Pre-calculated** gradient stops
- **Atomic reads** for parameter values

### Memory Budget
- **Texture cache**: Max 8MB total
- **Geometry cache**: Max 512KB
- **Per-component**: Max 64KB
- **Total UI memory**: Target < 16MB

### CPU Budget (per frame at 60Hz)
- **Paint calls**: < 2ms total
- **Parameter updates**: < 0.5ms
- **Animation calculations**: < 0.3ms
- **Event handling**: < 0.2ms
- **Total frame time**: < 3ms (leaves headroom)

## Accessibility

### Keyboard Navigation
- Tab/Shift+Tab: Navigate controls
- Arrow keys: Adjust focused control
- Space/Enter: Activate buttons
- Escape: Cancel operations
- Ctrl+Z/Y: Undo/Redo (where applicable)

### Visual Accessibility
- High contrast between text and backgrounds
- Clear focus indicators (2px colored outline)
- Sufficient control spacing (min 8px gaps)
- Alternative text for all controls
- Colorblind-friendly LED colors option

## Room-Specific Panels

### Canvas Room
- Brush size, pressure, color intensity
- Frequency range (f_min, f_max)
- Paint decay time
- Grid snap options

### Filters Room
- Multimode filter (LP/HP/BP/Notch)
- Cutoff, resonance, drive
- Envelope amount, key tracking
- Morph A↔B with smoothing

### Tube Room
- Drive, bias, mix controls
- Oversampling (2×/4×/8×)
- Tube type selector
- Output compensation

### Musical Room
- Formant shifter
- Comb filter controls
- Parametric EQ (3-band)
- Harmonic enhancer

### Forge Room
- Pattern sequencer
- Step velocity/gate
- Swing amount
- Pattern length

### Rompler Room
- Sample slot matrix (4×4)
- Layer volume/pan
- Key range splits
- EMU filter routing

## Implementation Notes

### Component Hierarchy
```
ARTEFACTAudioProcessorEditor
├── SpectralLookAndFeel (manages themes)
├── TextureBank (singleton, holds all images)
├── TopBar
│   ├── LogoComponent
│   ├── PresetManager
│   ├── ThemeToggle
│   └── StatusReadouts
├── MainContent
│   ├── RoomsView (navigation rail)
│   ├── CanvasView (center paint area)
│   └── RightStack (control panels)
└── Footer
    └── PerformanceMeters
```

### Update Cycle
1. Timer callback at 30-60Hz
2. Check atomic parameter snapshots
3. Update only changed components
4. Coalesce repaints to minimize overhead
5. Skip updates when editor not visible

## Quality Metrics

### Visual Fidelity
- ✓ Consistent theme colors across all elements
- ✓ Proper depth perception via shadows/highlights
- ✓ Smooth gradients without banding
- ✓ Crisp text rendering at all sizes
- ✓ Authentic hardware aesthetic

### Performance Targets
- ✓ 60 FPS minimum on modern hardware
- ✓ < 5% CPU usage when idle
- ✓ < 10% CPU during active painting
- ✓ Instant response to control changes
- ✓ No audio dropouts during UI operations

---
*Revision 1.0 - SpectralCanvas Pro E-mu Inspired UI*