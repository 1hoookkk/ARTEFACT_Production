# SpectralCanvas Pro - UI Layout Guide

## Window Layout (800x600)

```
┌─────────────────────────────────────────────────────────────────────────────┐
│ Spectral Canvas Pro                                                         │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  [○] Transform      ┌─────────────────────┐                                │
│                     │                     │                                │
│                     │    BLACK CANVAS     │                                │
│                     │                     │                                │
│                     │   Paint here to     │                                │
│                     │   create sound      │                                │
│                     │                     │                                │
│                     │   128x128 pixels    │                                │
│                     │                     │                                │
│                     └─────────────────────┘                                │
│                                                                             │
│                                                                             │
│ Status: MC: ON | fallbacks: 0                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

## UI Components

### 1. Character Switch [○]
- **Location**: Top left
- **Appearance**: Circle indicator (○ = OFF, ● = ON)
- **Function**: Toggles vintage machine character
- **Behavior**: 150ms smooth morph when toggled
- **NO LABELS**: Mysterious design as specified

### 2. Transform Button
- **Location**: Next to character switch
- **Text**: "Transform"  
- **Function**: Triggers 2-second vintage hihat demo
- **Behavior**: 
  - Posts synthetic harsh hihat stroke
  - Enables character switch for 2 seconds
  - Auto-disables after demo

### 3. Paint Canvas
- **Location**: Center area
- **Size**: 128x128 pixels (or larger based on layout)
- **Background**: Black
- **Grid**: 8x8 reference grid (dark gray)
- **Function**: Paint-to-audio synthesis
- **Behavior**:
  - Mouse down: Clear previous strokes, start new stroke
  - Mouse drag: Add stroke points, create sound in real-time
  - Mouse up: Finish stroke
  - Cursor: Crosshair when over canvas

### 4. Status Display
- **Location**: Bottom of window
- **Content**: "MC: ON/OFF | fallbacks: N"
- **Function**: Shows multicore processing status
- **Color**: Lime green text

## Audio Behavior

### Normal Mode (Character Switch OFF)
- Paint strokes create clean spectral oscillators
- No tape speed effects
- Normal stereo width
- Pure synthesis sound

### Vintage Mode (Character Switch ON)
- Paint strokes create spectral oscillators + vintage character
- 15% faster tape speed (1.15x playback)
- 40% wider stereo field (1.4x width)  
- Vintage machine coloration
- 150ms smooth transition when toggling

### Transform Demo
- Loads harsh hihat characteristics
- Triggers at high frequency (x=0.75, y=0.85)
- High pressure (0.9) for aggressive attack
- Full brightness (1.0) for hihat shimmer
- Temporary character enable (2 seconds)

## Technical Notes
- All paint strokes flow through RT-safe SPSC ring buffer
- UI updates at 30Hz via timer callback
- Parameter smoothing prevents audio clicks
- Emergency hard limiting prevents speaker damage
- Built as both VST3 and Standalone applications