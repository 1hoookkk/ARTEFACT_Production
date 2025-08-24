#!/usr/bin/env python3
"""
SpectralCanvas Pro Sprite Processing Script
Processes high-resolution source sprites into production-ready assets for JUCE plugin
"""

import os
import sys
from pathlib import Path

try:
    from PIL import Image, ImageOps
    PIL_AVAILABLE = True
except ImportError:
    PIL_AVAILABLE = False
    print("PIL/Pillow not available, will attempt basic processing")

def ensure_dir(path):
    """Create directory if it doesn't exist"""
    Path(path).mkdir(parents=True, exist_ok=True)

def get_sprite_info(input_path):
    """Get sprite information and categorize by name"""
    name = Path(input_path).stem.lower()
    
    # Categorize sprites by function
    if 'knob' in name:
        return 'knob', [32, 48, 64]  # Standard knob sizes
    elif 'led' in name and 'segment' not in name:
        return 'led', [12, 16, 20]   # LED indicator sizes
    elif 'button' in name:
        return 'button', [24, 32, 40]  # Button sizes
    elif 'toggle' in name:
        return 'toggle', [20, 24, 28]  # Toggle switch sizes
    elif 'meter' in name or 'vu' in name:
        return 'meter', [16, 20, 24]   # VU meter segment sizes
    elif 'lcd' in name or 'segment' in name:
        return 'lcd', [16, 20, 24]     # LCD/segment sizes
    else:
        return 'misc', [32, 48, 64]    # Default sizes

def process_sprite_pil(input_path, output_dir, sizes):
    """Process sprite using PIL/Pillow"""
    try:
        with Image.open(input_path) as img:
            # Convert to RGBA if not already
            if img.mode != 'RGBA':
                img = img.convert('RGBA')
            
            base_name = Path(input_path).stem
            category, _ = get_sprite_info(input_path)
            
            # Create category subdirectory
            category_dir = Path(output_dir) / category
            ensure_dir(category_dir)
            
            # Process each size
            results = []
            for size in sizes:
                # High-quality resize with antialiasing
                resized = img.resize((size, size), Image.Resampling.LANCZOS)
                
                # Generate clean filename
                clean_name = f"{category}_{size}px.png"
                if 'knob' in base_name.lower():
                    clean_name = f"knob_{size}px.png"
                elif 'led' in base_name.lower() and 'red' in base_name.lower():
                    clean_name = f"led_red_{size}px.png"
                elif 'led' in base_name.lower() and 'green' in base_name.lower():
                    clean_name = f"led_green_{size}px.png"
                elif 'button' in base_name.lower():
                    clean_name = f"button_{size}px.png"
                elif 'toggle' in base_name.lower():
                    clean_name = f"toggle_{size}px.png"
                elif 'meter' in base_name.lower():
                    clean_name = f"meter_segment_{size}px.png"
                elif 'lcd' in base_name.lower():
                    clean_name = f"lcd_segment_{size}px.png"
                
                output_path = category_dir / clean_name
                
                # Save with PNG optimization
                resized.save(output_path, 'PNG', optimize=True)
                results.append(str(output_path))
                print(f"+ Created {clean_name} ({size}x{size})")
            
            return results
    except Exception as e:
        print(f"- Error processing {input_path}: {e}")
        return []

def process_panel_pil(input_path, output_dir):
    """Process panel textures for 9-slice rendering"""
    try:
        with Image.open(input_path) as img:
            if img.mode != 'RGBA':
                img = img.convert('RGBA')
            
            base_name = Path(input_path).stem.lower()
            
            # Determine panel type and optimal sizes
            if 'teal' in base_name:
                name = "panel_teal"
                sizes = [64, 128, 256]  # 9-slice panel sizes
            elif 'graphite' in base_name or 'rail' in base_name:
                name = "panel_graphite"
                sizes = [64, 128, 256]
            elif 'nameplate' in base_name or 'aluminum' in base_name:
                name = "nameplate_aluminum"
                sizes = [32, 64, 128]
            else:
                name = "panel_generic"
                sizes = [64, 128, 256]
            
            panel_dir = Path(output_dir) / "panels"
            ensure_dir(panel_dir)
            
            results = []
            for size in sizes:
                # Resize maintaining aspect ratio
                resized = img.resize((size, size), Image.Resampling.LANCZOS)
                
                output_path = panel_dir / f"{name}_{size}px.png"
                resized.save(output_path, 'PNG', optimize=True)
                results.append(str(output_path))
                print(f"+ Created {name}_{size}px.png")
            
            return results
    except Exception as e:
        print(f"- Error processing panel {input_path}: {e}")
        return []

def create_sprite_index(output_dir):
    """Create an index file listing all processed sprites"""
    index_content = []
    index_content.append("// SpectralCanvas Pro Sprite Asset Index")
    index_content.append("// Auto-generated by process_sprites.py")
    index_content.append("")
    
    assets_dir = Path(output_dir)
    
    # Index sprites by category
    for category_dir in assets_dir.iterdir():
        if category_dir.is_dir():
            index_content.append(f"// {category_dir.name.upper()} SPRITES")
            for sprite_file in sorted(category_dir.glob("*.png")):
                relative_path = sprite_file.relative_to(assets_dir)
                index_content.append(f"// {relative_path}")
            index_content.append("")
    
    index_path = assets_dir / "sprite_index.txt"
    with open(index_path, 'w') as f:
        f.write('\n'.join(index_content))
    
    print(f"+ Created sprite index: {index_path}")

def main():
    # Paths
    source_dir = Path("C:/ARTEFACT_Production/Source/assets_src")
    output_dir = Path("C:/ARTEFACT_Production/Source/assets")
    
    if not PIL_AVAILABLE:
        print("ERROR: PIL/Pillow is required for sprite processing")
        print("Install with: pip install Pillow")
        return 1
    
    if not source_dir.exists():
        print(f"ERROR: Source directory not found: {source_dir}")
        return 1
    
    print("SpectralCanvas Pro Sprite Processor")
    print("===================================")
    
    # Create output structure
    ensure_dir(output_dir)
    
    processed_count = 0
    
    # Process individual sprites
    sprites_dir = source_dir / "sprites"
    if sprites_dir.exists():
        print(f"\nProcessing sprites from {sprites_dir}")
        for sprite_file in sprites_dir.glob("*.png"):
            category, sizes = get_sprite_info(sprite_file)
            results = process_sprite_pil(sprite_file, output_dir, sizes)
            processed_count += len(results)
    
    # Process panel textures
    panels_dir = source_dir / "panels"
    if panels_dir.exists():
        print(f"\nProcessing panels from {panels_dir}")
        for panel_file in panels_dir.glob("*.png"):
            results = process_panel_pil(panel_file, output_dir)
            processed_count += len(results)
    
    # Create sprite index
    create_sprite_index(output_dir)
    
    print(f"\nProcessing complete!")
    print(f"   Processed {processed_count} sprite assets")
    print(f"   Output directory: {output_dir}")
    print(f"   Ready for JUCE BinaryData integration")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())