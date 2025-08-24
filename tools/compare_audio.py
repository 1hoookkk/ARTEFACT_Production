#!/usr/bin/env python3
"""
Audio Analysis Tool for SpectralCanvas Pro Demo Validation
Compares rendered audio against expected characteristics for hi-hat→pad transformation
"""

import numpy as np
import scipy.signal
import sys
import argparse
from pathlib import Path

try:
    import soundfile as sf
except ImportError:
    print("ERROR: soundfile not installed. Run: pip install soundfile")
    sys.exit(1)

def analyze_spectral_content(audio, sr, scale="C"):
    """Analyze spectral content for harmonic quantization validation"""
    
    # C Major frequencies for validation
    c_major_ratios = [1.0, 9/8, 5/4, 4/3, 3/2, 5/3, 15/8]  # Major scale ratios
    fundamental = 220.0  # A3 as base
    expected_freqs = [fundamental * ratio for ratio in c_major_ratios]
    
    # Compute FFT
    fft_size = min(8192, len(audio))
    if len(audio) < fft_size:
        audio_padded = np.pad(audio, (0, fft_size - len(audio)))
    else:
        audio_padded = audio[:fft_size]
    
    spectrum = np.fft.rfft(audio_padded)
    freqs = np.fft.rfftfreq(fft_size, 1/sr)
    magnitude = np.abs(spectrum)
    
    # Find peaks
    peaks, _ = scipy.signal.find_peaks(magnitude, height=np.max(magnitude) * 0.1)
    peak_freqs = freqs[peaks]
    peak_mags = magnitude[peaks]
    
    # Check for harmonic content near expected frequencies
    harmonic_score = 0.0
    tolerance_hz = 10.0  # Hz tolerance for frequency matching
    
    for expected_freq in expected_freqs:
        # Find closest peak
        if len(peak_freqs) > 0:
            distances = np.abs(peak_freqs - expected_freq)
            closest_idx = np.argmin(distances)
            if distances[closest_idx] < tolerance_hz:
                # Weight by magnitude
                harmonic_score += peak_mags[closest_idx] / np.max(magnitude)
    
    return {
        'harmonic_score': harmonic_score / len(expected_freqs),
        'peak_freqs': peak_freqs[:10],  # Top 10 peaks
        'peak_mags': peak_mags[:10],
        'total_peaks': len(peaks)
    }

def analyze_transient_preservation(audio, sr):
    """Analyze if transients are preserved (for percussion→pad validation)"""
    
    # Short-time energy analysis
    hop_length = 512
    frame_length = 1024
    
    # Compute frame-wise energy
    n_frames = (len(audio) - frame_length) // hop_length + 1
    energy = np.zeros(n_frames)
    
    for i in range(n_frames):
        start = i * hop_length
        end = start + frame_length
        frame = audio[start:end]
        energy[i] = np.sum(frame ** 2)
    
    # Find energy peaks (potential transients)
    if len(energy) > 0:
        energy_peaks, _ = scipy.signal.find_peaks(energy, height=np.max(energy) * 0.3)
        transient_count = len(energy_peaks)
        
        # Compute attack characteristics
        if transient_count > 0:
            avg_attack_time = np.mean(np.diff(energy_peaks)) * hop_length / sr
        else:
            avg_attack_time = 0.0
    else:
        transient_count = 0
        avg_attack_time = 0.0
    
    return {
        'transient_count': transient_count,
        'avg_attack_time': avg_attack_time,
        'energy_variance': np.var(energy) if len(energy) > 0 else 0.0
    }

def validate_hihat_to_pad_transformation(audio_file, scale="C"):
    """Main validation function for hi-hat→pad transformation"""
    
    try:
        audio, sr = sf.read(audio_file)
        if len(audio.shape) > 1:
            audio = np.mean(audio, axis=1)  # Convert to mono
    except Exception as e:
        return False, f"Failed to read audio file: {e}"
    
    # Basic audio validation
    if len(audio) == 0:
        return False, "Audio file is empty"
    
    rms = np.sqrt(np.mean(audio ** 2))
    if rms < 1e-6:
        return False, f"Audio is too quiet (RMS: {rms:.2e})"
    
    # Analyze spectral content
    spectral_analysis = analyze_spectral_content(audio, sr, scale)
    
    # Analyze transient preservation
    transient_analysis = analyze_transient_preservation(audio, sr)
    
    # Validation criteria
    results = {
        'rms': rms,
        'duration': len(audio) / sr,
        'spectral': spectral_analysis,
        'transient': transient_analysis
    }
    
    # Pass/fail criteria
    checks = []
    
    # Must have audible content
    if rms > 1e-5:
        checks.append(("Audio audible", True, f"RMS: {rms:.4f}"))
    else:
        checks.append(("Audio audible", False, f"Too quiet: {rms:.2e}"))
    
    # Must have harmonic content (pad characteristic)
    if spectral_analysis['harmonic_score'] > 0.1:
        checks.append(("Harmonic content", True, f"Score: {spectral_analysis['harmonic_score']:.3f}"))
    else:
        checks.append(("Harmonic content", False, f"Low score: {spectral_analysis['harmonic_score']:.3f}"))
    
    # Must preserve some transient character
    if transient_analysis['transient_count'] > 0:
        checks.append(("Transient preservation", True, f"Count: {transient_analysis['transient_count']}"))
    else:
        checks.append(("Transient preservation", False, "No transients detected"))
    
    # Overall pass/fail
    passed_checks = sum(1 for _, passed, _ in checks if passed)
    all_passed = passed_checks == len(checks)
    
    return all_passed, {
        'overall': 'PASS' if all_passed else 'FAIL',
        'checks': checks,
        'results': results
    }

def main():
    parser = argparse.ArgumentParser(description='Analyze SpectralCanvas Pro demo audio')
    parser.add_argument('audio_file', help='Path to rendered audio file')
    parser.add_argument('--scale', default='C', help='Expected scale (default: C)')
    parser.add_argument('--verbose', '-v', action='store_true', help='Verbose output')
    
    args = parser.parse_args()
    
    if not Path(args.audio_file).exists():
        print(f"ERROR: Audio file not found: {args.audio_file}")
        sys.exit(1)
    
    print(f"Analyzing: {args.audio_file}")
    print(f"Expected scale: {args.scale} major")
    print("-" * 50)
    
    passed, results = validate_hihat_to_pad_transformation(args.audio_file, args.scale)
    
    if isinstance(results, str):
        print(f"ERROR: {results}")
        sys.exit(1)
    
    # Print results
    print(f"Overall Result: {results['overall']}")
    print()
    
    for check_name, check_passed, details in results['checks']:
        status = "✓ PASS" if check_passed else "✗ FAIL"
        print(f"{status} {check_name}: {details}")
    
    if args.verbose:
        print()
        print("Detailed Analysis:")
        r = results['results']
        print(f"  Duration: {r['duration']:.2f}s")
        print(f"  RMS Level: {r['rms']:.4f}")
        print(f"  Spectral Peaks: {r['spectral']['total_peaks']}")
        print(f"  Harmonic Score: {r['spectral']['harmonic_score']:.3f}")
        print(f"  Transients: {r['transient']['transient_count']}")
        print(f"  Attack Time: {r['transient']['avg_attack_time']:.3f}s")
        
        if len(r['spectral']['peak_freqs']) > 0:
            print("  Top Peak Frequencies:")
            for i, freq in enumerate(r['spectral']['peak_freqs'][:5]):
                print(f"    {freq:.1f} Hz")
    
    sys.exit(0 if passed else 1)

if __name__ == '__main__':
    main()