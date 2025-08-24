# Deployment

## Artifacts

- VST3 plugin (`build/*/SpectralCanvas.vst3`)
- Standalone app (if enabled by JUCE target)

## Steps (Windows)

1. Build Release:
```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target SpectralCanvas
```
2. Validate with pluginval (optional).
3. Package binaries with required runtime libs.

## Notes

- Avoid bundling `_archive/` or `build/` interim files.
- Use semantic versioning; update `project(...)` in `CMakeLists.txt`.

