# SpectralCanvas Pro Asset Generation Specification
*Systematic approach to prevent asset chaos*

## 🎯 Overview
This document defines EXACTLY what assets to generate, their sizes, placement, and usage to avoid the mess from last time.

## 📐 Asset Specifications

### 1. Knob Face Texture
- **Filename**: `knob_face_64.png`  
- **Size**: 64×64px (EXACTLY)
- **Format**: PNG with alpha channel
- **Style**: 1980s EMU rack synthesizer knob
- **Colors**: Dark aluminum (#2A3140) with subtle radial brushing
- **Usage**: Background texture for all rotary controls
- **Placement**: Centered on knob bounds, scales uniformly

**DALL-E 3 Prompt**:
```
Create a circular knob face for vintage 1980s EMU synthesizer, exactly 64x64 pixels, PNG with transparent background. Surface: brushed dark aluminum (#2A3140) with subtle radial brushing pattern emanating from center. Small raised metal indicator dot at top. Realistic lighting shows brushed metal texture. Perfect circle, no shadows outside boundary. Product photography quality.
```

### 2. LED Bloom Effect
- **Filename**: `led_bloom_32.png`
- **Size**: 32×32px (EXACTLY) 
- **Format**: PNG with alpha channel
- **Style**: Soft amber LED glow for vintage hardware
- **Colors**: Amber core (#FFB366) fading to transparent
- **Usage**: Background glow behind LED components
- **Placement**: Centered on LED position

**DALL-E 3 Prompt**:
```
Create LED bloom glow effect for vintage audio hardware, exactly 32x32 pixels, PNG with transparent background. Center: bright amber core (#FFB366). Soft radial gradient to transparent edges with realistic lens flare streaks. Should look like warm analog LED bleeding through frosted plastic housing. No hard edges, perfect antialiasing.
```

### 3. Panel Background Texture  
- **Filename**: `panel_teal_256.png`
- **Size**: 256×256px (EXACTLY)
- **Format**: PNG (no alpha needed)
- **Style**: EMU rack equipment panel texture
- **Colors**: Dark teal (#1D2A32) with subtle texture
- **Usage**: Tileable background for main panels
- **Placement**: Tiled across panel surfaces

**DALL-E 3 Prompt**:
```
Create seamless tileable metal panel texture for 1980s EMU rack synthesizer, exactly 256x256 pixels. Powder-coated dark teal (#1D2A32) with very subtle brushed metal underneath. Matte finish, minimal reflectivity. Must tile perfectly with no visible seams when repeated. Reference: professional audio rack equipment texture.
```

### 4. Mystery Switch (Your Request!)
- **Filename**: `switch_mystery_48x24.png`
- **Size**: 48×24px (EXACTLY)
- **Format**: PNG with alpha channel  
- **Style**: Unlabeled toggle switch for filter/tube amp
- **Colors**: Graphite housing (#2A3140), silver toggle
- **Usage**: Special control for secret sauce
- **Placement**: Precise positioning in control layout

**DALL-E 3 Prompt**:
```
Create vintage toggle switch for 1980s synthesizer, exactly 48x24 pixels, PNG with transparent background. Dark graphite plastic housing (#2A3140) with subtle texture. Silver metal toggle lever in center position. Unmarked and mysterious - no labels or text. Reference: EMU Emulator series switches. Realistic 3D rendering, soft shadows within boundary.
```

## 🔧 Implementation Steps

### Step 1: Generate Assets
1. Use the exact prompts above in DALL-E 3
2. Save with exact filenames specified
3. Verify dimensions match specifications

### Step 2: Validate Assets
```cpp
// Code will automatically validate:
AssetValidator::validateKnobFace(image);   // Checks 64x64 + alpha
AssetValidator::validateLEDBloom(image);   // Checks 32x32 + alpha  
AssetValidator::validatePanelTexture(image); // Checks 256x256
AssetValidator::validateSwitch(image);     // Checks 48x24 + alpha
```

### Step 3: Add to Project
1. Place assets in `Source/UI/Assets/`
2. Add to CMakeLists.txt as JUCE binary resources
3. Update BinaryData references in AssetManager

### Step 4: Test Systematically
1. Build with fallback assets first (verify layout)
2. Add real assets one by one
3. Validate each component renders correctly

## 🚫 What NOT to Do
- Don't create random asset sizes
- Don't place assets without using AssetPlacement system
- Don't skip validation steps
- Don't generate multiple versions "just in case"

## ✅ Success Criteria
- All assets exactly match specifications
- No placement chaos or overlapping elements  
- Hybrid vector+bitmap rendering works smoothly
- Mystery switch integrates perfectly with existing UI

This systematic approach ensures we get the "dated but satisfying" vintage hardware feel without the mess from last time!