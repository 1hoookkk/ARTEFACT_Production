@echo off
setlocal

echo "--- Configuring and building tests... ---"
cmake -B build -S . -G "Visual Studio 17 2022"
if %errorlevel% neq 0 (
    echo "ERROR: CMake configuration failed."
    exit /b 1
)

cmake --build build --target SpectralCanvas_Tests --config Debug
if %errorlevel% neq 0 (
    echo "ERROR: Build failed."
    exit /b 1
)