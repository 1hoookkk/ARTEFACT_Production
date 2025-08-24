#!/usr/bin/env python3
"""
E-mu Audity 2000 Style UI Texture Generator
Generates procedural texture assets for SpectralCanvas E-mu theme
"""

import os
import math
import random
from PIL import Image, ImageDraw, ImageFilter, ImageOps
from typing import Tuple, List

# E-mu Audity 2000 Theme Colors
PANEL_COLOR = (0x34, 0x5E, 0x78)      # Main panel color
BEZEL_COLOR = (0x3D, 0x43, 0x49)      # Dark bezel/frame color  
LCD_COLOR = (0x8D, 0xFF, 0x6A)        # Lime green LCD
BUTTON_LIGHT = (0x48, 0x72, 0x94)     # Lighter button shade
BUTTON_DARK = (0x25, 0x47, 0x5C)      # Darker button shade
LED_ON = (0xFF, 0x40, 0x40)           # Red LED on
LED_OFF = (0x40, 0x20, 0x20)          # Dark LED off
METER_ON = (0x00, 0xFF, 0x80)         # Green meter on
METER_OFF = (0x20, 0x40, 0x30)        # Dark meter off

class EMUTextureGenerator:
    def __init__(self, output_dir: str = "Source/UI/Assets"):
        self.output_dir = output_dir
        # Ensure output directory exists
        os.makedirs(self.output_dir, exist_ok=True)
        
    def add_noise(self, image: Image.Image, intensity: float = 0.1) -> Image.Image:
        """Add subtle noise texture for realism"""
        noise = Image.new('RGBA', image.size, (0, 0, 0, 0))
        pixels = noise.load()
        
        for y in range(image.height):
            for x in range(image.width):
                noise_val = int((random.random() - 0.5) * intensity * 255)
                pixels[x, y] = (noise_val, noise_val, noise_val, abs(noise_val))
        
        return Image.alpha_composite(image.convert('RGBA'), noise)
    
    def create_gradient(self, size: Tuple[int, int], start_color: Tuple[int, int, int], 
                       end_color: Tuple[int, int, int], direction: str = 'vertical') -> Image.Image:
        """Create a linear gradient"""
        image = Image.new('RGB', size)
        draw = ImageDraw.Draw(image)
        
        if direction == 'vertical':
            for y in range(size[1]):
                ratio = y / size[1]
                r = int(start_color[0] * (1 - ratio) + end_color[0] * ratio)
                g = int(start_color[1] * (1 - ratio) + end_color[1] * ratio)
                b = int(start_color[2] * (1 - ratio) + end_color[2] * ratio)
                draw.line([(0, y), (size[0], y)], fill=(r, g, b))
        else:  # horizontal
            for x in range(size[0]):
                ratio = x / size[0]
                r = int(start_color[0] * (1 - ratio) + end_color[0] * ratio)
                g = int(start_color[1] * (1 - ratio) + end_color[1] * ratio)
                b = int(start_color[2] * (1 - ratio) + end_color[2] * ratio)
                draw.line([(x, 0), (x, size[1])], fill=(r, g, b))
        
        return image
    
    def generate_panel_powdercoat(self) -> Image.Image:
        """Generate tileable powder-coated metal panel texture (256x256)"""
        size = (256, 256)
        
        # Base color with slight variation
        base_r, base_g, base_b = PANEL_COLOR
        image = Image.new('RGB', size, PANEL_COLOR)
        pixels = image.load()
        
        # Add subtle color variation and speckles
        for y in range(size[1]):
            for x in range(size[0]):
                # Subtle color variation
                variation = random.gauss(0, 8)
                r = max(0, min(255, base_r + int(variation)))
                g = max(0, min(255, base_g + int(variation)))
                b = max(0, min(255, base_b + int(variation)))
                
                # Add occasional brighter speckles (powder coat texture)
                if random.random() < 0.003:
                    speckle_brightness = random.randint(15, 35)
                    r = min(255, r + speckle_brightness)
                    g = min(255, g + speckle_brightness) 
                    b = min(255, b + speckle_brightness)
                
                # Add darker micro-scratches
                if random.random() < 0.001:
                    scratch_darkness = random.randint(10, 20)
                    r = max(0, r - scratch_darkness)
                    g = max(0, g - scratch_darkness)
                    b = max(0, b - scratch_darkness)
                    
                pixels[x, y] = (r, g, b)
        
        # Apply very subtle blur to soften harsh edges
        image = image.filter(ImageFilter.GaussianBlur(radius=0.3))
        return image
    
    def generate_bezel_9slice(self) -> Image.Image:
        """Generate dark bezel for 9-slice (48x48, 12px insets)"""
        size = (48, 48)
        inset = 12
        
        # Create base dark bezel
        image = Image.new('RGB', size, BEZEL_COLOR)
        draw = ImageDraw.Draw(image)
        
        # Outer highlight edge (top-left)
        lighter = tuple(min(255, c + 20) for c in BEZEL_COLOR)
        draw.rectangle([0, 0, size[0]-1, 0], fill=lighter)  # Top
        draw.rectangle([0, 0, 0, size[1]-1], fill=lighter)  # Left
        
        # Inner shadow edge (bottom-right) 
        darker = tuple(max(0, c - 30) for c in BEZEL_COLOR)
        draw.rectangle([0, size[1]-1, size[0]-1, size[1]-1], fill=darker)  # Bottom
        draw.rectangle([size[0]-1, 0, size[0]-1, size[1]-1], fill=darker)  # Right
        
        # Inner recess area
        inner_color = tuple(max(0, c - 15) for c in BEZEL_COLOR)
        draw.rectangle([inset, inset, size[0]-inset-1, size[1]-inset-1], fill=inner_color)
        
        # Inner highlight
        inner_light = tuple(min(255, c + 10) for c in inner_color)
        draw.rectangle([inset, inset, size[0]-inset-1, inset], fill=inner_light)  # Top
        draw.rectangle([inset, inset, inset, size[1]-inset-1], fill=inner_light)  # Left
        
        return image
    
    def generate_button_up(self) -> Image.Image:
        """Generate beveled button in up state (80x32)"""
        size = (80, 32)
        
        # Base gradient from light to dark
        light = BUTTON_LIGHT
        dark = BUTTON_DARK
        image = self.create_gradient(size, light, dark, 'vertical')
        draw = ImageDraw.Draw(image)
        
        # Top highlight
        highlight = tuple(min(255, c + 25) for c in light)
        draw.rectangle([1, 1, size[0]-2, 3], fill=highlight)
        
        # Bottom shadow
        shadow = tuple(max(0, c - 25) for c in dark)
        draw.rectangle([1, size[1]-3, size[0]-2, size[1]-2], fill=shadow)
        
        # Side highlights and shadows
        draw.rectangle([1, 1, 3, size[1]-2], fill=highlight)  # Left highlight
        draw.rectangle([size[0]-3, 1, size[0]-2, size[1]-2], fill=shadow)  # Right shadow
        
        return image
    
    def generate_button_down(self) -> Image.Image:
        """Generate pressed button state (80x32)"""
        size = (80, 32)
        
        # Darker base for pressed state
        dark = tuple(max(0, c - 20) for c in BUTTON_DARK)
        light = BUTTON_DARK
        image = self.create_gradient(size, dark, light, 'vertical')
        draw = ImageDraw.Draw(image)
        
        # Pressed effect - invert highlights/shadows
        shadow = tuple(max(0, c - 30) for c in dark)
        draw.rectangle([1, 1, size[0]-2, 3], fill=shadow)  # Top shadow
        draw.rectangle([1, 1, 3, size[1]-2], fill=shadow)  # Left shadow
        
        # Subtle bottom/right highlight
        highlight = tuple(min(255, c + 15) for c in light)
        draw.rectangle([3, size[1]-2, size[0]-1, size[1]-1], fill=highlight)  # Bottom
        draw.rectangle([size[0]-2, 3, size[0]-1, size[1]-1], fill=highlight)  # Right
        
        return image
    
    def generate_knob_cap(self) -> Image.Image:
        """Generate matte plastic knob with indicator (64x64)"""
        size = (64, 64)
        center = (size[0] // 2, size[1] // 2)
        radius = 28
        
        image = Image.new('RGBA', size, (0, 0, 0, 0))
        draw = ImageDraw.Draw(image)
        
        # Main knob body with gradient
        knob_light = tuple(min(255, c + 30) for c in PANEL_COLOR)
        knob_dark = tuple(max(0, c - 20) for c in PANEL_COLOR)
        
        # Draw circular gradient
        for r in range(radius, 0, -1):
            ratio = r / radius
            color_r = int(knob_light[0] * ratio + knob_dark[0] * (1 - ratio))
            color_g = int(knob_light[1] * ratio + knob_dark[1] * (1 - ratio))
            color_b = int(knob_light[2] * ratio + knob_dark[2] * (1 - ratio))
            
            draw.ellipse([center[0] - r, center[1] - r, center[0] + r, center[1] + r],
                        fill=(color_r, color_g, color_b))
        
        # Indicator line (pointing up)
        indicator_start = (center[0], center[1] - radius + 8)
        indicator_end = (center[0], center[1] - radius + 18)
        draw.line([indicator_start, indicator_end], fill=(255, 255, 255), width=2)
        
        # Subtle highlight and shadow
        highlight = tuple(min(255, c + 40) for c in knob_light)
        shadow = tuple(max(0, c - 40) for c in knob_dark)
        
        # Top-left highlight arc
        for i in range(3):
            r = radius - i
            draw.arc([center[0] - r, center[1] - r, center[0] + r, center[1] + r],
                    start=225, end=45, fill=highlight, width=1)
        
        # Bottom-right shadow arc  
        for i in range(3):
            r = radius - i
            draw.arc([center[0] - r, center[1] - r, center[0] + r, center[1] + r],
                    start=45, end=225, fill=shadow, width=1)
        
        return image
    
    def generate_led_on(self) -> Image.Image:
        """Generate bright LED dot (8x8)"""
        size = (8, 8)
        center = (4, 4)
        
        image = Image.new('RGBA', size, (0, 0, 0, 0))
        draw = ImageDraw.Draw(image)
        
        # Bright LED core
        draw.ellipse([1, 1, 6, 6], fill=LED_ON)
        
        # Bright center highlight
        highlight = tuple(min(255, c + 80) for c in LED_ON)
        draw.ellipse([2, 2, 5, 5], fill=highlight)
        
        # Very bright center
        draw.ellipse([3, 3, 4, 4], fill=(255, 255, 255))
        
        return image
    
    def generate_led_off(self) -> Image.Image:
        """Generate dark LED dot (8x8)"""
        size = (8, 8)
        
        image = Image.new('RGBA', size, (0, 0, 0, 0))
        draw = ImageDraw.Draw(image)
        
        # Dark LED
        draw.ellipse([1, 1, 6, 6], fill=LED_OFF)
        
        # Subtle highlight to show it's still a physical element
        highlight = tuple(min(255, c + 20) for c in LED_OFF)
        draw.ellipse([2, 2, 4, 4], fill=highlight)
        
        return image
    
    def generate_lcd_strip(self) -> Image.Image:
        """Generate lime green LCD background (512x32)"""
        size = (512, 32)
        
        # Base LCD color
        image = Image.new('RGB', size, LCD_COLOR)
        draw = ImageDraw.Draw(image)
        
        # Subtle horizontal lines for LCD texture
        darker_lcd = tuple(max(0, c - 15) for c in LCD_COLOR)
        for y in range(0, size[1], 2):
            draw.line([(0, y), (size[0], y)], fill=darker_lcd)
        
        # Very subtle vertical segments
        segment_color = tuple(max(0, c - 8) for c in LCD_COLOR)
        for x in range(0, size[0], 8):
            draw.line([(x, 0), (x, size[1])], fill=segment_color)
        
        # Darker border
        border_color = tuple(max(0, c - 40) for c in LCD_COLOR)
        draw.rectangle([0, 0, size[0]-1, 0], fill=border_color)  # Top
        draw.rectangle([0, size[1]-1, size[0]-1, size[1]-1], fill=border_color)  # Bottom
        
        return image
    
    def generate_meter_on(self) -> Image.Image:
        """Generate LED meter segment lit (6x14)"""
        size = (6, 14)
        
        image = Image.new('RGB', size, METER_ON)
        draw = ImageDraw.Draw(image)
        
        # Bright center
        highlight = tuple(min(255, c + 60) for c in METER_ON)
        draw.rectangle([1, 1, 4, 12], fill=highlight)
        
        # Very bright core
        core = tuple(min(255, c + 100) for c in METER_ON)
        draw.rectangle([2, 2, 3, 11], fill=core)
        
        return image
    
    def generate_meter_off(self) -> Image.Image:
        """Generate LED meter segment dark (6x14)"""
        size = (6, 14)
        
        image = Image.new('RGB', size, METER_OFF)
        draw = ImageDraw.Draw(image)
        
        # Subtle highlight to show physical structure
        highlight = tuple(min(255, c + 15) for c in METER_OFF)
        draw.rectangle([1, 1, 4, 12], fill=highlight)
        
        return image
    
    def generate_all_textures(self):
        """Generate all texture assets"""
        print("Generating E-mu Audity 2000 style texture assets...")
        
        textures = [
            ("panel_powdercoat.png", self.generate_panel_powdercoat),
            ("bezel_9slice.png", self.generate_bezel_9slice),
            ("button_up.png", self.generate_button_up),
            ("button_down.png", self.generate_button_down),
            ("knob_cap.png", self.generate_knob_cap),
            ("led_on.png", self.generate_led_on),
            ("led_off.png", self.generate_led_off),
            ("lcd_strip.png", self.generate_lcd_strip),
            ("meter_on.png", self.generate_meter_on),
            ("meter_off.png", self.generate_meter_off),
        ]
        
        for filename, generator_func in textures:
            print(f"  Generating {filename}...")
            image = generator_func()
            filepath = os.path.join(self.output_dir, filename)
            image.save(filepath)
            print(f"    Saved: {filepath}")
        
        print(f"\nAll textures generated successfully in: {self.output_dir}")
        
        # Generate summary
        self.generate_summary()
    
    def generate_summary(self):
        """Generate a summary of created assets"""
        summary_path = os.path.join(self.output_dir, "texture_manifest.txt")
        
        summary_content = f"""E-mu Audity 2000 Style Texture Assets
Generated on: {os.path.basename(__file__)}

Theme Colors:
- Panel: #{PANEL_COLOR[0]:02X}{PANEL_COLOR[1]:02X}{PANEL_COLOR[2]:02X}
- Bezel: #{BEZEL_COLOR[0]:02X}{BEZEL_COLOR[1]:02X}{BEZEL_COLOR[2]:02X}  
- LCD: #{LCD_COLOR[0]:02X}{LCD_COLOR[1]:02X}{LCD_COLOR[2]:02X}

Generated Assets:
1. panel_powdercoat.png (256x256) - Tileable speckled metal panel texture
2. bezel_9slice.png (48x48) - Dark bezel with 12px insets for 9-slice
3. button_up.png (80x32) - Beveled button in up state
4. button_down.png (80x32) - Pressed button state  
5. knob_cap.png (64x64) - Matte plastic knob with indicator
6. led_on.png (8x8) - Bright red LED dot
7. led_off.png (8x8) - Dark LED dot
8. lcd_strip.png (512x32) - Lime green LCD background with texture
9. meter_on.png (6x14) - LED meter segment lit (green)
10. meter_off.png (6x14) - LED meter segment dark

Usage Notes:
- panel_powdercoat.png is tileable for large background areas
- bezel_9slice.png uses 12px insets for proper 9-slice scaling
- Button states can be swapped based on interaction
- Knob indicator points upward (12 o'clock position)
- LED and meter assets support on/off states for visual feedback
"""
        
        with open(summary_path, 'w') as f:
            f.write(summary_content)
        
        print(f"Asset manifest saved: {summary_path}")

def main():
    """Main execution function"""
    # Set random seed for reproducible textures
    random.seed(12345)
    
    # Generate in Assets/UI directory
    generator = EMUTextureGenerator("Source/UI/Assets")
    generator.generate_all_textures()
    
    print("\n[SUCCESS] E-mu Audity 2000 texture generation complete!")
    print("Assets are ready for integration into SpectralCanvas UI.")
    print(f"Generated 10 texture assets in: {os.path.abspath('Source/UI/Assets')}")

if __name__ == "__main__":
    main()