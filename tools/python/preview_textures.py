#!/usr/bin/env python3
"""
Preview generated E-mu Audity 2000 textures
Simple script to view the generated texture assets
"""

import os
from PIL import Image

def preview_textures():
    """Display information about generated textures"""
    assets_dir = "Source/UI/Assets"
    
    if not os.path.exists(assets_dir):
        print(f"Assets directory not found: {assets_dir}")
        return
    
    print("E-mu Audity 2000 Texture Assets Preview")
    print("=" * 50)
    
    texture_files = [
        "panel_powdercoat.png",
        "bezel_9slice.png", 
        "button_up.png",
        "button_down.png",
        "knob_cap.png",
        "led_on.png",
        "led_off.png", 
        "lcd_strip.png",
        "meter_on.png",
        "meter_off.png"
    ]
    
    for filename in texture_files:
        filepath = os.path.join(assets_dir, filename)
        if os.path.exists(filepath):
            try:
                img = Image.open(filepath)
                file_size = os.path.getsize(filepath)
                print(f"{filename:20} | {img.size[0]:3}x{img.size[1]:<3} | {img.mode:4} | {file_size:,} bytes")
            except Exception as e:
                print(f"{filename:20} | ERROR: {e}")
        else:
            print(f"{filename:20} | NOT FOUND")
    
    print("\nTo view textures:")
    print("- Open the PNG files in any image viewer")
    print("- Import into your JUCE project as ImageComponents")
    print("- Use for UI theming in SpectralCanvas")

if __name__ == "__main__":
    preview_textures()