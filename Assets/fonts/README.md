Embedded fonts (optional)

Place a MetaSynth-style pixel/mono font here to embed it into the app so the look is identical across systems.

Recommended:
- PixelOperator.ttf (Public Domain / SIL)
- VT323.ttf (Google Fonts)

Usage:
- Drop one or more .ttf/.otf files into this folder.
- Reconfigure/build:
  - cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
  - msbuild build\SpectralCanvas_Standalone.vcxproj /p:Configuration=RelWithDebInfo;Platform=x64 /m /nologo

Notes:
- Files in this folder will be embedded via JUCE BinaryData and automatically used by the UI.
- If no fonts are present, the app falls back to installed system fonts.

