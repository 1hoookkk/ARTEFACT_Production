# Spectral Mapping (Y → Frequency)

This document outlines how canvas Y-coordinates map to frequency bins for spectral synthesis.

## Implementation

- Core: `Source/Core/ColorToSpectralMapper.(h|cpp)`
- Helpers: `Source/Core/FrequencyLUT.h`

## Mapping Strategy

- Log-frequency grid across canvas height.
- Adjustable base and range parameters (min/max Hz).
- Optional snapping/quantization via `HarmonicQuantizer.h`.

## Considerations

- Maintain numeric stability across sample rates.
- Ensure consistent mapping when resizing canvas.
- Validate against golden references in tests.

## Testing

- Add tests that verify bin indices for canonical Y positions.
- Validate frequency-to-bin inverse mapping when available.

