@echo off
echo ========================================
echo Create3D - Build Script for VS2022
echo ========================================
echo.

set PROJECT_DIR=%~dp0
set BUILD_DIR=%PROJECT_DIR%build_vs2022

echo Project Directory: %PROJECT_DIR%
echo Build Directory: %BUILD_DIR%
echo.

cd /d "%PROJECT_DIR%"

if not exist "%PROJECT_DIR%vendor\glfw" (
    echo [WARNING] Dependencies not found in vendor directory!
    echo.
    echo Please run download_deps.bat first, or manually download:
    echo   1. GLFW 3.4 - https://github.com/glfw/glfw/archive/refs/tags/3.4.zip
    echo   2. GLAD v0.1.36 - https://github.com/Dav1dde/glad/archive/refs/tags/v0.1.36.zip
    echo   3. ImGui 1.91.0 - https://github.com/ocornut/imgui/archive/refs/tags/v1.91.0.zip
    echo   4. GLM 1.0.1 - https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip
    echo.
    echo Extract to:
    echo   %PROJECT_DIR%vendor\glfw
    echo   %PROJECT_DIR%vendor\glad
    echo   %PROJECT_DIR%vendor\imgui
    echo   %PROJECT_DIR%vendor\glm
    echo.
    echo NOTE: CMake will try to download from GitHub if vendor dir is empty
    echo.
)

echo [1/3] Creating build directory...
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

echo.
echo [2/3] Configuring CMake project...
cd /d "%BUILD_DIR%"
cmake "%PROJECT_DIR%" -G "Visual Studio 17 2022" -A x64

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] CMake configuration failed!
    echo.
    pause
    exit /b 1
)

echo.
echo [3/3] Building Release configuration...
cmake --build . --config Release

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Build failed!
    echo.
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo Executable: %BUILD_DIR%\Release\Create3D.exe
echo.
echo To run the program:
echo   %BUILD_DIR%\Release\Create3D.exe
echo.
pause
