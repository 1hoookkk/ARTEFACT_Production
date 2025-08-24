# Build Guide

## Prerequisites

- CMake 3.24+
- MSVC 2022 (Windows) or a recent Clang/GCC (other platforms)
- JUCE fetched by CMake (no manual setup required)

## Recommended Dev Build (with tests)

```
cmake -S . -B build -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build --config RelWithDebInfo --target ALL_BUILD
```

## Release Build

```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## Debug Build

```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build build --config Debug
```

## Windows Helpers (optional)

- `scripts/build-and-notify.ps1 -Config RelWithDebInfo`

