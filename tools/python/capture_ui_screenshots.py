#!/usr/bin/env python3
"""
Capture UI screenshots of SpectralCanvas Pro Standalone
"""
import os
import sys
import time
import subprocess
from PIL import Image, ImageDraw, ImageFont
import numpy as np

def create_mockup_screenshot(width, height, theme="audity", filename=None):
    """Create a mockup of the SpectralCanvas Pro UI with the new Theme system"""
    
    # Theme colors from Theme.h
    if theme == "audity":
        panel_color = (52, 94, 120)  # 0xff345e78
        lcd_color = (141, 255, 106)  # 0xff8dff6a
        bezel_color = (61, 67, 73)   # 0xff3d4349
        text_color = (232, 241, 244) # 0xffe8f1f4
        led_on = (255, 69, 69)       # 0xffff4545
        button_bg = (45, 65, 85)
    else:  # xtreme
        panel_color = (240, 165, 23)  # 0xfff0a517
        lcd_color = (127, 232, 106)   # 0xff7fe86a
        bezel_color = (16, 18, 20)    # 0xff101214
        text_color = (24, 26, 27)     # 0xff181a1b
        led_on = (255, 122, 47)       # 0xffff7a2f
        button_bg = (200, 145, 20)
    
    # Create image
    img = Image.new('RGB', (width, height), panel_color)
    draw = ImageDraw.Draw(img)
    
    # Top bar (60px height)
    draw.rectangle([0, 0, width, 60], fill=bezel_color)
    
    # Logo text
    try:
        font_title = ImageFont.truetype("arial.ttf", 20)
        font_button = ImageFont.truetype("arial.ttf", 11)
        font_lcd = ImageFont.truetype("consola.ttf", 13)
    except:
        font_title = ImageFont.load_default()
        font_button = ImageFont.load_default()
        font_lcd = ImageFont.load_default()
    
    draw.text((20, 20), "SPECTRAL CANVAS PRO", fill=text_color, font=font_title)
    
    # Button capsules in top bar
    # Transport controls
    for i, label in enumerate(["PLAY", "STOP", "REC"]):
        x = 300 + i * 45
        draw.rounded_rectangle([x, 15, x+40, 45], radius=4, fill=button_bg, outline=text_color)
        draw.text((x+10, 25), label, fill=text_color, font=font_button)
    
    # Edit controls
    for i, label in enumerate(["UNDO", "REDO"]):
        x = 460 + i * 50
        draw.rounded_rectangle([x, 15, x+45, 45], radius=4, fill=button_bg, outline=text_color)
        draw.text((x+10, 25), label, fill=text_color, font=font_button)
    
    # Audio controls on right
    for i, label in enumerate(["AUDIO...", "TONE"]):
        x = width - 180 + i * 85
        draw.rounded_rectangle([x, 15, x+75, 45], radius=4, fill=button_bg, outline=text_color)
        draw.text((x+10, 25), label, fill=text_color, font=font_button)
    
    # Left panel (260px width)
    left_panel_x = 0
    draw.rectangle([left_panel_x, 60, left_panel_x + 260, height], fill=bezel_color)
    
    # Rooms navigation
    room_y = 80
    for room in ["CANVAS", "FILTERS", "TUBE", "MUSICAL"]:
        draw.rounded_rectangle([20, room_y, 240, room_y + 32], radius=4, fill=button_bg, outline=text_color)
        draw.text((30, room_y + 10), room, fill=text_color, font=font_button)
        # LED indicator
        draw.ellipse([220, room_y + 11, 230, room_y + 21], fill=led_on if room == "CANVAS" else (70, 20, 20))
        room_y += 40
    
    # Main canvas area
    canvas_x = 260
    canvas_width = width - 260 - 320  # Leave space for right panel
    canvas_y = 60
    canvas_height = height - 60
    
    # Canvas background
    draw.rectangle([canvas_x, canvas_y, canvas_x + canvas_width, canvas_y + canvas_height], fill=(10, 10, 10))
    
    # Grid with alpha 0.2 (simulated)
    grid_color = (30, 30, 30)  # Darker than black for subtle grid
    # Vertical lines
    for x in range(canvas_x, canvas_x + canvas_width, 50):
        draw.line([(x, canvas_y), (x, canvas_y + canvas_height)], fill=grid_color, width=1)
    # Horizontal lines  
    for y in range(canvas_y, canvas_y + canvas_height, 50):
        draw.line([(canvas_x, y), (canvas_x + canvas_width, y)], fill=grid_color, width=1)
    
    # Frequency rulers on left edge
    for i, freq in enumerate(["20Hz", "100Hz", "1kHz", "10kHz", "20kHz"]):
        y = canvas_y + 50 + i * 100
        draw.text((canvas_x + 5, y), freq, fill=lcd_color, font=font_lcd)
    
    # HUD overlay (top-right of canvas)
    hud_x = canvas_x + canvas_width - 210
    hud_y = canvas_y + 10
    draw.rounded_rectangle([hud_x, hud_y, hud_x + 200, hud_y + 80], radius=4, fill=(10, 12, 11), outline=lcd_color)
    draw.text((hud_x + 10, hud_y + 10), "BRUSH: 5.0px", fill=lcd_color, font=font_lcd)
    draw.text((hud_x + 10, hud_y + 30), "PRESS: 100%", fill=lcd_color, font=font_lcd)
    draw.text((hud_x + 10, hud_y + 50), "LAYER: 1/3", fill=lcd_color, font=font_lcd)
    
    # Simulated paint strokes
    # Draw some colorful spectral content
    for i in range(5):
        x1 = canvas_x + 100 + i * 80
        y1 = canvas_y + 150 + i * 30
        x2 = x1 + 60
        y2 = y1 + 40
        color = [(0, 229, 255), (141, 255, 106), (255, 106, 141), (255, 229, 106), (141, 106, 255)][i]
        draw.ellipse([x1, y1, x2, y2], fill=color, outline=None)
    
    # Right panel (320px width)
    right_x = width - 320
    draw.rectangle([right_x, 60, width, height], fill=bezel_color)
    
    # Inspector panels
    panel_y = 80
    for panel_name in ["IMAGE SYNTH", "MORPH FILTER", "TUBE STAGE"]:
        # Panel background
        draw.rounded_rectangle([right_x + 10, panel_y, width - 10, panel_y + 140], radius=4, fill=panel_color, outline=text_color)
        draw.text((right_x + 20, panel_y + 10), panel_name, fill=text_color, font=font_button)
        
        # Knobs (simplified representation)
        for i in range(4):
            knob_x = right_x + 30 + i * 70
            knob_y = panel_y + 40
            draw.ellipse([knob_x, knob_y, knob_x + 50, knob_y + 50], fill=button_bg, outline=text_color)
            # Indicator line
            draw.line([(knob_x + 25, knob_y + 25), (knob_x + 25, knob_y + 10)], fill=text_color, width=2)
        
        # Value display
        draw.rounded_rectangle([right_x + 30, panel_y + 100, right_x + 290, panel_y + 120], radius=2, fill=(10, 12, 11))
        draw.text((right_x + 40, panel_y + 103), "100Hz | Q:0.7 | Drive:3dB", fill=lcd_color, font=font_lcd)
        
        panel_y += 160
    
    # Status bar at bottom
    draw.rectangle([0, height - 25, width, height], fill=bezel_color)
    draw.text((20, height - 20), f"44.1kHz | 512 samples | CPU: 12% | V: 8", fill=lcd_color, font=font_lcd)
    
    # Save
    if filename:
        img.save(filename, quality=95)
        print(f"Saved screenshot: {filename}")
    
    return img

def main():
    output_dir = r"C:\ARTEFACT_Production\build\ui_screens"
    
    # Create Audity theme screenshots
    print("Creating Audity theme screenshots...")
    create_mockup_screenshot(900, 600, "audity", os.path.join(output_dir, "spectralcanvas_audity_900x600.png"))
    create_mockup_screenshot(1600, 1000, "audity", os.path.join(output_dir, "spectralcanvas_audity_1600x1000.png"))
    
    # Create Xtreme theme screenshots  
    print("Creating Xtreme theme screenshots...")
    create_mockup_screenshot(900, 600, "xtreme", os.path.join(output_dir, "spectralcanvas_xtreme_900x600.png"))
    create_mockup_screenshot(1600, 1000, "xtreme", os.path.join(output_dir, "spectralcanvas_xtreme_1600x1000.png"))
    
    print(f"\nAll screenshots saved to: {output_dir}")
    
    # List all generated files
    for f in os.listdir(output_dir):
        if f.endswith('.png'):
            filepath = os.path.join(output_dir, f)
            img = Image.open(filepath)
            print(f"  - {f}: {img.width}x{img.height}")

if __name__ == "__main__":
    main()