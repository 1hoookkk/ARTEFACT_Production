# EMU Hardware-Inspired Style Guide for SpectralCanvas Pro
*AI Asset Generation Reference - EMU Audity Aesthetic for SpectralCanvas Pro Software*

## Core Visual Identity

### Color Palette (EMU Audity 2000-Inspired for SpectralCanvas Pro)
- **Primary Blue**: #1E3A5F (Medium Blue - main panels)
- **Dark Blue**: #0A1628 (Background and recessed areas)
- **Light Blue**: #4A7FBB (Active elements and highlights)
- **Metallic Gray**: #6B7280 (Brushed metal surfaces)
- **LCD Green**: #00FF41 (Display text)
- **Amber Orange**: #FFB000 (EMU signature accent)
- **Deep Shadow**: #000814 (Depth and shadows)

### Material Characteristics
- **Brushed Metal**: Subtle horizontal grain texture
- **Metallic Blue**: Slightly reflective with brushed finish
- **LCD Surfaces**: Matte black with slight recessed appearance
- **Raised Elements**: Subtle beveled edges with highlight/shadow
- **Panel Texture**: Fine brushed aluminum with directional grain

## AI Generation Prompt Vocabulary

### Core Keywords (Use in all prompts)
- "EMU Audity-inspired style"
- "metallic blue"
- "brushed metal" 
- "analog synthesizer"
- "high-resolution"
- "photorealistic"
- "software plugin UI"
- "professional audio equipment"
- "vintage synthesizer"

### Specific Material Descriptors
- "brushed aluminum texture"
- "subtle reflections"
- "beveled edges"
- "recessed screws"
- "directional metal grain"
- "matte black LCD"
- "retro pixelated font"
- "LED segments"

### Quality Modifiers
- "high detail"
- "clean design" 
- "professional grade"
- "studio equipment"
- "precision engineering"

## Component-Specific Prompts

### Knobs & Rotary Controls
**Base Prompt**: "Photorealistic brushed metal knob, metallic blue ring, analog synthesizer, high-resolution, software plugin UI"

**Variations**:
- **Filter Knob**: Add "white indicator line, slightly angled from above"
- **Volume Knob**: Add "knurled edge, silver highlight"
- **Fine Control**: Add "smaller size, precise markings"

**Angles to Generate**:
- Straight on (0°)
- Slightly angled (15° from above)
- Three-quarter view (30° from above-right)

### Metallic Panels
**Base Prompt**: "Metallic blue brushed metal panel, EMU Audity style, high-resolution, software plugin UI, subtle reflections, clean design"

**Size Variations**:
- Small: "approximately 200x50 pixels"
- Medium: "approximately 400x100 pixels" 
- Large: "approximately 600x200 pixels"

**Edge Treatments**:
- "with subtle beveled edges"
- "with recessed corner screws"
- "with raised border detail"

### LCD Displays
**Base Prompt**: "LCD display screen, blue backlight, pixelated retro font, analog synthesizer, high-resolution, software plugin UI"

**Content Variations**:
- Parameter: "showing 'FILTER 1.2kHz'"
- Value: "showing '127'"
- Status: "showing 'READY'"
- Preset: "showing 'LEAD 001'"

**States**:
- Active: "bright blue backlight"
- Inactive: "dim backlight, low contrast"

### VU Meters
**Base Prompt**: "VU meter, analog style, metallic blue background, green and red LED segments, high-resolution, software plugin UI"

**Level Variations**:
- Low: "showing -12dB, mostly green segments"
- Medium: "showing -6dB, green to yellow"
- High: "showing 0dB, red segments active"
- Peak: "showing +3dB, peak red indicators"

**Orientations**:
- Horizontal bar
- Vertical column
- Curved meter (classic VU style)

## Technical Specifications

### Resolution Requirements
- **Minimum**: 512x512 for square elements
- **Recommended**: 1024x1024 for primary assets
- **Panels**: 2048x512 for large background panels
- **Detail Elements**: 256x256 for small indicators

### File Organization
```
Source/UI/Assets/
├── Knobs/
│   ├── filter_knob_001.png
│   ├── volume_knob_001.png
│   └── fine_knob_001.png
├── Panels/
│   ├── main_panel_large.png
│   ├── side_panel_medium.png
│   └── corner_detail.png
├── Displays/
│   ├── lcd_active_001.png
│   ├── lcd_inactive_001.png
│   └── led_indicator_001.png
└── Meters/
    ├── vu_horizontal_001.png
    ├── vu_vertical_001.png
    └── spectrum_background.png
```

## Magnific AI Upscaling Settings

### Recommended Settings
- **Creativity**: 0.2-0.3 (preserve design integrity)
- **Resemblance**: 0.8-0.9 (maintain style consistency) 
- **Detail**: 0.7-0.8 (enhance metallic textures)
- **Upscale Factor**: 2x initially, then 2x again if needed

### Prompt Guidance for Upscaling
- "enhance brushed metal texture"
- "sharpen metallic edges" 
- "preserve blue metallic color"
- "maintain LED clarity"
- "improve surface detail"

## Integration Notes

### LookAndFeel Class Integration
- Replace procedural drawing with image-based rendering
- Maintain interactive behavior (hover, click states)
- Scale assets appropriately for different DPI
- Cache images for performance

### Paint Canvas Considerations
- 800x400px main canvas area
- Seamless integration with EMU-inspired control panels
- Subtle texture overlay for paint interaction feedback
- Maintain visual hierarchy between paint area and controls

## Quality Checkpoints

### Before Magnific Upscaling
- [ ] Consistent color palette across all elements
- [ ] Proper perspective and lighting
- [ ] Clear material definition (metal vs plastic vs LCD)
- [ ] Appropriate level of detail for final use size

### After Integration  
- [ ] Visual coherence across entire interface
- [ ] Proper scaling at different display resolutions
- [ ] Smooth integration with existing LookAndFeel code
- [ ] Performance impact within acceptable limits

---

*This style guide ensures consistent, professional-quality EMU Audity-inspired visuals for SpectralCanvas Pro using AI generation + Magnific upscaling workflow.*