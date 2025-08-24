#!/usr/bin/env python3
"""
SpectralCanvas Pro E-mu Texture Generator
Generates procedural hardware-style textures for the UI.
"""

from PIL import Image, ImageDraw, ImageFilter
import os
import random
import math

# E-mu Audity color scheme
AUDITY_PANEL = (52, 94, 120)      # #345E78
AUDITY_BEZEL = (61, 67, 73)       # #3D4349  
AUDITY_LCD = (141, 255, 106)      # #8DFF6A

# Create output directory
os.makedirs("Assets/UI", exist_ok=True)

def create_noise_texture(width, height, base_color, variation=0.1):
    """Create a subtle noise texture."""
    img = Image.new('RGBA', (width, height))
    pixels = []
    
    for y in range(height):
        for x in range(width):
            # Add random variation to base color
            r = int(base_color[0] * (1 + random.uniform(-variation, variation)))
            g = int(base_color[1] * (1 + random.uniform(-variation, variation)))
            b = int(base_color[2] * (1 + random.uniform(-variation, variation)))
            
            # Clamp values
            r = max(0, min(255, r))
            g = max(0, min(255, g))
            b = max(0, min(255, b))
            
            pixels.append((r, g, b, 255))
    
    img.putdata(pixels)
    return img

def create_gradient(width, height, color1, color2, direction='vertical'):
    """Create a gradient image."""
    img = Image.new('RGBA', (width, height))
    draw = ImageDraw.Draw(img)
    
    if direction == 'vertical':
        for y in range(height):
            ratio = y / height
            r = int(color1[0] * (1 - ratio) + color2[0] * ratio)
            g = int(color1[1] * (1 - ratio) + color2[1] * ratio)
            b = int(color1[2] * (1 - ratio) + color2[2] * ratio)
            draw.line([(0, y), (width, y)], fill=(r, g, b, 255))
    else:  # horizontal
        for x in range(width):
            ratio = x / width
            r = int(color1[0] * (1 - ratio) + color2[0] * ratio)
            g = int(color1[1] * (1 - ratio) + color2[1] * ratio)
            b = int(color1[2] * (1 - ratio) + color2[2] * ratio)
            draw.line([(x, 0), (x, height)], fill=(r, g, b, 255))
    
    return img

def create_panel_powdercoat(size=256):
    """Create tileable powdercoat panel texture."""
    # Start with base color
    img = create_noise_texture(size, size, AUDITY_PANEL, 0.05)
    
    # Add subtle gradient
    gradient = create_gradient(size, size, 
                              tuple(int(c * 1.1) for c in AUDITY_PANEL),
                              tuple(int(c * 0.9) for c in AUDITY_PANEL))
    
    # Blend gradient with noise
    img = Image.blend(img, gradient, 0.3)
    
    # Add fine speckle pattern
    draw = ImageDraw.Draw(img)
    random.seed(42)  # Consistent pattern
    for _ in range(size * 2):
        x = random.randint(0, size - 1)
        y = random.randint(0, size - 1)
        brightness = random.uniform(0.8, 1.2)
        color = tuple(int(c * brightness) for c in AUDITY_PANEL) + (255,)
        draw.point((x, y), fill=color)
    
    return img

def create_bezel_9slice(size=48):
    """Create 9-slice bezel texture."""
    img = Image.new('RGBA', (size, size), AUDITY_BEZEL + (255,))
    draw = ImageDraw.Draw(img)
    
    # Outer highlight
    lighter = tuple(int(c * 1.3) for c in AUDITY_BEZEL)
    draw.rectangle([0, 0, size-1, size-1], outline=lighter + (255,), width=1)
    
    # Inner shadow
    darker = tuple(int(c * 0.7) for c in AUDITY_BEZEL)
    draw.rectangle([1, 1, size-2, size-2], outline=darker + (255,), width=1)
    
    # Center recess
    center_color = tuple(int(c * 0.8) for c in AUDITY_BEZEL)
    draw.rectangle([12, 12, size-12, size-12], fill=center_color + (255,))
    
    return img

def create_button(width=80, height=32, pressed=False):
    """Create button texture."""
    if pressed:
        base_color = tuple(int(c * 0.8) for c in AUDITY_PANEL)
    else:
        base_color = AUDITY_PANEL
    
    img = Image.new('RGBA', (width, height), base_color + (255,))
    draw = ImageDraw.Draw(img)
    
    if not pressed:
        # Top/left highlight
        highlight = tuple(int(c * 1.4) for c in base_color)
        draw.line([(0, 0), (width-1, 0)], fill=highlight + (255,), width=2)
        draw.line([(0, 0), (0, height-1)], fill=highlight + (255,), width=2)
        
        # Bottom/right shadow
        shadow = tuple(int(c * 0.6) for c in base_color)
        draw.line([(1, height-1), (width-1, height-1)], fill=shadow + (255,), width=2)
        draw.line([(width-1, 1), (width-1, height-1)], fill=shadow + (255,), width=2)
    else:
        # Inset shadow for pressed state
        shadow = tuple(int(c * 0.5) for c in base_color)
        draw.line([(0, 0), (width-1, 0)], fill=shadow + (255,), width=2)
        draw.line([(0, 0), (0, height-1)], fill=shadow + (255,), width=2)
    
    return img

def create_knob_cap(size=64):
    """Create knob cap texture."""
    img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    center = size // 2
    radius = center - 2
    
    # Outer ring (bezel)
    draw.ellipse([2, 2, size-2, size-2], fill=AUDITY_BEZEL + (255,))
    
    # Inner knob with gradient effect
    knob_size = int(radius * 0.85)
    knob_color = tuple(int(c * 1.2) for c in AUDITY_BEZEL)
    draw.ellipse([center-knob_size, center-knob_size, 
                  center+knob_size, center+knob_size], 
                 fill=knob_color + (255,))
    
    # Highlight
    highlight_size = int(knob_size * 0.7)
    highlight_color = tuple(int(c * 1.5) for c in knob_color)
    draw.arc([center-highlight_size, center-highlight_size-4,
              center+highlight_size, center+highlight_size-4],
             30, 150, fill=highlight_color + (255,), width=2)
    
    # Indicator line
    draw.line([(center, center), (center, center-knob_size+5)], 
              fill=(255, 255, 255, 255), width=3)
    
    return img

def create_led(size=8, on=True):
    """Create LED texture."""
    img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    if on:
        # Bright red LED
        led_color = (255, 69, 69)
        glow_color = (255, 69, 69, 128)
        
        # Glow effect
        draw.ellipse([0, 0, size, size], fill=glow_color)
        # Core
        core_size = int(size * 0.6)
        offset = (size - core_size) // 2
        draw.ellipse([offset, offset, offset+core_size, offset+core_size], 
                     fill=led_color + (255,))
    else:
        # Dark LED
        led_color = (74, 16, 16)
        draw.ellipse([1, 1, size-1, size-1], fill=led_color + (255,))
        # Rim
        draw.ellipse([1, 1, size-1, size-1], outline=(42, 8, 8, 255), width=1)
    
    return img

def create_lcd_strip(width=512, height=32):
    """Create LCD strip background."""
    img = Image.new('RGBA', (width, height), (10, 12, 11, 255))  # Dark background
    draw = ImageDraw.Draw(img)
    
    # Subtle LCD glow area
    glow_color = tuple(int(c * 0.1) for c in AUDITY_LCD)
    draw.rectangle([4, 4, width-4, height-4], fill=glow_color + (255,))
    
    return img

def create_meter_segment(width=6, height=14, on=True):
    """Create meter segment."""
    img = Image.new('RGBA', (width, height), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    if on:
        # Green meter segment
        color = (0, 255, 65)
        draw.rectangle([1, 1, width-1, height-1], fill=color + (255,))
        # Highlight
        highlight = tuple(int(c * 1.3) for c in color)
        draw.line([(1, 1), (width-1, 1)], fill=highlight + (255,))
    else:
        # Dark segment
        color = (16, 64, 16)
        draw.rectangle([1, 1, width-1, height-1], fill=color + (255,))
        draw.rectangle([1, 1, width-1, height-1], outline=(8, 32, 8, 255))
    
    return img

# Generate all textures
print("Generating SpectralCanvas Pro E-mu textures...")

# Panel powdercoat (tileable)
panel_1x = create_panel_powdercoat(256)
panel_2x = create_panel_powdercoat(512)
panel_1x.save("Assets/UI/panel_powdercoat.png")
panel_2x.save("Assets/UI/panel_powdercoat@2x.png")

# Bezel 9-slice
bezel_1x = create_bezel_9slice(48)
bezel_2x = create_bezel_9slice(96)
bezel_1x.save("Assets/UI/bezel_9slice.png")
bezel_2x.save("Assets/UI/bezel_9slice@2x.png")

# Buttons
button_up_1x = create_button(80, 32, False)
button_up_2x = create_button(160, 64, False)
button_down_1x = create_button(80, 32, True)
button_down_2x = create_button(160, 64, True)
button_up_1x.save("Assets/UI/button_up.png")
button_up_2x.save("Assets/UI/button_up@2x.png")
button_down_1x.save("Assets/UI/button_down.png")
button_down_2x.save("Assets/UI/button_down@2x.png")

# Knob cap
knob_1x = create_knob_cap(64)
knob_2x = create_knob_cap(128)
knob_1x.save("Assets/UI/knob_cap.png")
knob_2x.save("Assets/UI/knob_cap@2x.png")

# LEDs
led_on_1x = create_led(8, True)
led_on_2x = create_led(16, True)
led_off_1x = create_led(8, False)
led_off_2x = create_led(16, False)
led_on_1x.save("Assets/UI/led_on.png")
led_on_2x.save("Assets/UI/led_on@2x.png")
led_off_1x.save("Assets/UI/led_off.png")
led_off_2x.save("Assets/UI/led_off@2x.png")

# LCD strip
lcd_1x = create_lcd_strip(512, 32)
lcd_2x = create_lcd_strip(1024, 64)
lcd_1x.save("Assets/UI/lcd_strip.png")
lcd_2x.save("Assets/UI/lcd_strip@2x.png")

# Meter segments
meter_on_1x = create_meter_segment(6, 14, True)
meter_on_2x = create_meter_segment(12, 28, True)
meter_off_1x = create_meter_segment(6, 14, False)
meter_off_2x = create_meter_segment(12, 28, False)
meter_on_1x.save("Assets/UI/meter_on.png")
meter_on_2x.save("Assets/UI/meter_on@2x.png")
meter_off_1x.save("Assets/UI/meter_off.png")
meter_off_2x.save("Assets/UI/meter_off@2x.png")

print("Texture generation complete!")
print("Generated files:")
for filename in sorted(os.listdir("Assets/UI")):
    if filename.endswith('.png'):
        filepath = os.path.join("Assets/UI", filename)
        size = os.path.getsize(filepath)
        print(f"  {filename} ({size} bytes)")