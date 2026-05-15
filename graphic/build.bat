@echo off
setlocal enabledelayedexpansion

echo ========================================
echo CAD Graphics Engine - Build Script
echo ========================================
echo.

where cmake >nul 2>&1
if errorlevel 1 (
    echo [ERROR] CMake is not found in PATH
    echo Please install CMake from https://cmake.org/download/
    echo And make sure to add it to PATH during installation
    pause
    exit /b 1
)

echo [OK] CMake found:
cmake --version
echo.

set BUILD_DIR=build
set SOURCE_DIR=%~dp0
set USE_GLFW=0
set USE_GDI=1

if "%1"=="glfw" (
    set USE_GLFW=1
    set USE_GDI=0
    echo [INFO] Using GLFW backend (requires GLFW3)
) else (
    echo [INFO] Using Windows GDI backend (no external dependencies needed)
)

if exist "%BUILD_DIR%" (
    echo.
    echo [INFO] Cleaning old build directory...
    rmdir /s /q "%BUILD_DIR%"
)

mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%"

echo.
echo [INFO] Configuring CMake...
echo.

if %USE_GLFW%==1 (
    cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_GLFW=ON -DUSE_GDI=OFF
) else (
    cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_GLFW=OFF -DUSE_GDI=ON
)

if errorlevel 1 (
    echo.
    echo [ERROR] CMake configuration failed!
    echo.
    echo For GDI backend:
    echo   build.bat
    echo.
    echo For GLFW backend (requires GLFW3):
    echo   build.bat glfw
    echo.
    pause
    exit /b 1
)

echo.
echo [INFO] Building project...
echo.

cmake --build . --config Release

if errorlevel 1 (
    echo.
    echo [ERROR] Build failed!
    pause
    exit /b 1
)

echo.
echo ========================================
echo [SUCCESS] Build completed!
echo ========================================
echo.
echo Executable: %cd%\Release\CADGraphicsEngine.exe
echo.

if exist "%cd%\Release\CADGraphicsEngine.exe" (
    echo [INFO] To run the application:
    echo   cd /d "%cd%\Release"
    echo   CADGraphicsEngine.exe
    echo.
    set /p RUN=Run application now? [Y/N]: 
    if /i "!RUN!"=="Y" (
        echo [INFO] Starting application...
        start "" "%cd%\Release\CADGraphicsEngine.exe"
    )
)

endlocal
pause
