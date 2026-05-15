@echo off
echo ========================================
echo Create3D Dependency Downloader
echo ========================================
echo.

set VENDOR_DIR=%~dp0vendor
set TEMP_DIR=%~dp0temp_dl

if not exist "%VENDOR_DIR%" mkdir "%VENDOR_DIR%"
if not exist "%TEMP_DIR%" mkdir "%TEMP_DIR%"

echo Checking for curl...
where curl >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] curl not found. Please install Git for Windows or use WSL.
    echo.
    echo Alternative: Manually download the following files:
    echo   1. GLFW 3.4:     https://github.com/glfw/glfw/archive/refs/tags/3.4.zip
    echo   2. GLAD v0.1.36: https://github.com/Dav1dde/glad/archive/refs/tags/v0.1.36.zip
    echo   3. ImGui 1.91.0: https://github.com/ocornut/imgui/archive/refs/tags/v1.91.0.zip
    echo   4. GLM 1.0.1:    https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip
    echo.
    echo Extract each to:
    echo   %VENDOR_DIR%\glfw
    echo   %VENDOR_DIR%\glad
    echo   %VENDOR_DIR%\imgui
    echo   %VENDOR_DIR%\glm
    echo.
    pause
    exit /b 1
)

echo.
echo Downloading dependencies...
echo.

echo [1/4] Downloading GLFW 3.4...
curl -L --fail "https://github.com/glfw/glfw/archive/refs/tags/3.4.zip" -o "%TEMP_DIR%\glfw.zip"
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to download GLFW
    goto :cleanup_error
)

echo [2/4] Downloading GLAD v0.1.36...
curl -L --fail "https://github.com/Dav1dde/glad/archive/refs/tags/v0.1.36.zip" -o "%TEMP_DIR%\glad.zip"
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to download GLAD
    goto :cleanup_error
)

echo [3/4] Downloading ImGui 1.91.0...
curl -L --fail "https://github.com/ocornut/imgui/archive/refs/tags/v1.91.0.zip" -o "%TEMP_DIR%\imgui.zip"
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to download ImGui
    goto :cleanup_error
)

echo [4/4] Downloading GLM 1.0.1...
curl -L --fail "https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip" -o "%TEMP_DIR%\glm.zip"
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to download GLM
    goto :cleanup_error
)

echo.
echo Extracting dependencies...
echo.

where powershell >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo Extracting GLFW...
    powershell -Command "Expand-Archive -Force -Path '%TEMP_DIR%\glfw.zip' -DestinationPath '%TEMP_DIR%'"
    move /Y "%TEMP_DIR%\glfw-3.4" "%VENDOR_DIR%\glfw" >nul
    
    echo Extracting GLAD...
    powershell -Command "Expand-Archive -Force -Path '%TEMP_DIR%\glad.zip' -DestinationPath '%TEMP_DIR%'"
    move /Y "%TEMP_DIR%\glad-0.1.36" "%VENDOR_DIR%\glad" >nul
    
    echo Extracting ImGui...
    powershell -Command "Expand-Archive -Force -Path '%TEMP_DIR%\imgui.zip' -DestinationPath '%TEMP_DIR%'"
    move /Y "%TEMP_DIR%\imgui-1.91.0" "%VENDOR_DIR%\imgui" >nul
    
    echo Extracting GLM...
    powershell -Command "Expand-Archive -Force -Path '%TEMP_DIR%\glm.zip' -DestinationPath '%TEMP_DIR%'"
    move /Y "%TEMP_DIR%\glm-1.0.1" "%VENDOR_DIR%\glm" >nul
) else (
    echo [ERROR] PowerShell not available for extraction
    echo Please manually extract the zip files from: %TEMP_DIR%
    goto :end
)

echo.
echo ========================================
echo All dependencies downloaded successfully!
echo ========================================
echo.
echo Now run: build.bat
echo.
goto :cleanup_success

:cleanup_error
echo.
echo ========================================
echo ERROR: Some dependencies failed to download
echo ========================================
echo.
echo Please check your internet connection or use a VPN/proxy.
echo Alternatively, download manually as shown above.
echo.

:cleanup_success
if exist "%TEMP_DIR%" rmdir /S /Q "%TEMP_DIR%"

:end
pause
