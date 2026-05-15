@echo off
setlocal enabledelayedexpansion

echo ========================================
echo CAD Graphics Engine - Build Script (Visual Studio)
echo ========================================
echo.

set VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe

if not exist "%VSWHERE%" (
    echo [ERROR] vswhere.exe not found. Please install Visual Studio.
    pause
    exit /b 1
)

for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -requires Microsoft.Component.MSBuild -property installationPath`) do (
    set VSPATH=%%i
)

if not defined VSPATH (
    echo [ERROR] Visual Studio installation not found.
    pause
    exit /b 1
)

echo [OK] Found Visual Studio at: %VSPATH%
echo.

set DEVCMD=%VSPATH%\Common7\Tools\VsDevCmd.bat
if not exist "%DEVCMD%" (
    echo [ERROR] Developer Command Prompt not found.
    pause
    exit /b 1
)

echo [INFO] Setting up Visual Studio environment...
echo.

set BUILD_DIR=%~dp0build
set SOURCE_DIR=%~dp0

if exist "%BUILD_DIR%" (
    echo [INFO] Cleaning old build directory...
    rmdir /s /q "%BUILD_DIR%"
)

mkdir "%BUILD_DIR%"

echo [INFO] Configuring CMake...
echo.

call "%DEVCMD%" -arch=x64 -host_arch=x64 -no_logo >nul 2>&1

cd /d "%BUILD_DIR%"
cmake .. -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release -DUSE_GLFW=OFF -DUSE_GDI=ON

if errorlevel 1 (
    echo.
    echo [ERROR] CMake configuration failed!
    pause
    exit /b 1
)

echo.
echo [INFO] Building project...
echo.

cmake --build . --config Release -- /m

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
echo Executable: %BUILD_DIR%\Release\CADGraphicsEngine.exe
echo.

if exist "%BUILD_DIR%\Release\CADGraphicsEngine.exe" (
    echo [INFO] Press Y to run the application, N to exit.
    choice /c YN /m "Run application"
    if errorlevel 2 goto :end
    if errorlevel 1 (
        echo [INFO] Starting application...
        start "" "%BUILD_DIR%\Release\CADGraphicsEngine.exe"
    )
)

:end
endlocal
pause
