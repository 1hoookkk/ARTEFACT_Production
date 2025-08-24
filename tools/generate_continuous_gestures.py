#!/usr/bin/env python3
"""
Continuous Gesture Generator for SpectralCanvas Pro Demo
Creates smooth, continuous paint gestures for testing hi-hat→pad transformation
"""

import numpy as np
import argparse
import sys
from pathlib import Path

def generate_smooth_trajectory(duration_sec, num_points, y_start=0.9, y_end=0.3, movement_type='descending'):
    """Generate smooth Y position trajectory"""
    
    t = np.linspace(0, duration_sec, num_points)
    
    if movement_type == 'descending':
        # Smooth descent with slight curve
        y = y_start + (y_end - y_start) * (t / duration_sec) ** 1.5
        
    elif movement_type == 'wave':
        # Sinusoidal movement
        y_mid = (y_start + y_end) / 2
        y_range = (y_start - y_end) / 2
        y = y_mid + y_range * np.sin(2 * np.pi * t / duration_sec * 2)
        
    elif movement_type == 'spiral':
        # Spiral pattern (frequency increases)
        base_descent = y_start + (y_end - y_start) * (t / duration_sec)
        spiral_freq = t / duration_sec * 4  # Accelerating spiral
        spiral_amp = 0.1 * (1 - t / duration_sec)  # Decreasing amplitude
        y = base_descent + spiral_amp * np.sin(2 * np.pi * spiral_freq * t)
        
    else:  # linear
        y = y_start + (y_end - y_start) * (t / duration_sec)
    
    return t, np.clip(y, 0.0, 1.0)

def generate_pressure_envelope(times, envelope_type='attack_sustain_release'):
    """Generate pressure envelope over time"""
    
    duration = times[-1] if len(times) > 0 else 1.0
    
    if envelope_type == 'attack_sustain_release':
        # Classic ADSR-style envelope
        attack_time = duration * 0.1
        sustain_time = duration * 0.7
        release_time = duration * 0.2
        
        pressure = np.zeros_like(times)
        for i, t in enumerate(times):
            if t < attack_time:
                # Attack phase
                pressure[i] = 0.3 + 0.6 * (t / attack_time)
            elif t < attack_time + sustain_time:
                # Sustain phase with slight decay
                sustain_progress = (t - attack_time) / sustain_time
                pressure[i] = 0.9 - 0.2 * sustain_progress
            else:
                # Release phase
                release_progress = (t - attack_time - sustain_time) / release_time
                pressure[i] = 0.7 * (1 - release_progress ** 2)
                
    elif envelope_type == 'crescendo':
        # Gradual increase
        pressure = 0.2 + 0.7 * (times / duration) ** 0.7
        
    elif envelope_type == 'burst':
        # Quick burst then decay
        pressure = 0.9 * np.exp(-2 * times / duration) + 0.1
        
    elif envelope_type == 'oscillating':
        # Rhythmic pressure changes
        base_pressure = 0.4 + 0.3 * (1 - times / duration)
        oscillation = 0.2 * np.sin(2 * np.pi * times / duration * 8)
        pressure = base_pressure + oscillation
        
    else:  # constant
        pressure = np.full_like(times, 0.6)
    
    return np.clip(pressure, 0.0, 1.0)

def generate_continuous_gestures(duration_sec, gesture_rate_hz=30, 
                                y_start=0.9, y_end=0.3, 
                                movement_type='descending',
                                pressure_type='attack_sustain_release'):
    """Generate a continuous stream of paint gestures"""
    
    # Calculate number of gesture points
    num_points = int(duration_sec * gesture_rate_hz)
    if num_points < 2:
        num_points = 2
    
    # Generate smooth trajectory
    times, y_positions = generate_smooth_trajectory(
        duration_sec, num_points, y_start, y_end, movement_type)
    
    # Generate pressure envelope
    pressures = generate_pressure_envelope(times, pressure_type)
    
    # Create gesture events
    gestures = []
    for t, y, p in zip(times, y_positions, pressures):
        gestures.append({
            'time': t,
            'y': y,
            'pressure': p
        })
    
    return gestures

def write_gestures_file(gestures, output_file):
    """Write gestures to SpectralCanvas format"""
    
    with open(output_file, 'w') as f:
        f.write("# SpectralCanvas Pro - Continuous Gesture Sequence\\n")
        f.write("# Generated for hi-hat→pad transformation testing\\n")
        f.write("# Format: <time_seconds> <yPos_0to1> <pressure_0to1>\\n")
        f.write("#\\n")
        f.write(f"# Total gestures: {len(gestures)}\\n")
        f.write(f"# Duration: {gestures[-1]['time']:.2f}s\\n")
        f.write("#\\n")
        f.write("\\n")
        
        for g in gestures:
            f.write(f"{g['time']:.4f} {g['y']:.4f} {g['pressure']:.4f}\\n")

def main():
    parser = argparse.ArgumentParser(description='Generate continuous gestures for SpectralCanvas Pro')
    parser.add_argument('output_file', help='Output gestures file')
    parser.add_argument('duration', type=float, help='Duration in seconds')
    parser.add_argument('rate', type=int, nargs='?', default=30, 
                       help='Gesture rate in Hz (default: 30)')
    
    parser.add_argument('--y-start', type=float, default=0.9,
                       help='Starting Y position (default: 0.9)')
    parser.add_argument('--y-end', type=float, default=0.3,
                       help='Ending Y position (default: 0.3)')
    parser.add_argument('--movement', choices=['descending', 'wave', 'spiral', 'linear'],
                       default='descending', help='Movement pattern')
    parser.add_argument('--pressure', choices=['attack_sustain_release', 'crescendo', 'burst', 'oscillating', 'constant'],
                       default='attack_sustain_release', help='Pressure envelope')
    parser.add_argument('--preview', action='store_true',
                       help='Print first 10 gestures for preview')
    
    args = parser.parse_args()
    
    if args.duration <= 0:
        print("ERROR: Duration must be positive")
        sys.exit(1)
    
    if args.rate <= 0 or args.rate > 1000:
        print("ERROR: Rate must be between 1-1000 Hz")
        sys.exit(1)
    
    print(f"Generating continuous gestures:")
    print(f"  Duration: {args.duration:.2f}s")
    print(f"  Rate: {args.rate} Hz")
    print(f"  Movement: {args.movement} ({args.y_start:.2f} → {args.y_end:.2f})")
    print(f"  Pressure: {args.pressure}")
    
    # Generate gestures
    gestures = generate_continuous_gestures(
        args.duration, args.rate,
        args.y_start, args.y_end,
        args.movement, args.pressure
    )
    
    print(f"Generated {len(gestures)} gesture points")
    
    if args.preview:
        print("\\nPreview (first 10 gestures):")
        for i, g in enumerate(gestures[:10]):
            print(f"  {g['time']:.4f}s  y={g['y']:.3f}  p={g['pressure']:.3f}")
        if len(gestures) > 10:
            print("  ...")
    
    # Write to file
    output_path = Path(args.output_file)
    try:
        write_gestures_file(gestures, output_path)
        print(f"\\nWrote gestures to: {output_path}")
    except Exception as e:
        print(f"ERROR writing file: {e}")
        sys.exit(1)
    
    print("Ready for render_test_input!")

if __name__ == '__main__':
    main()