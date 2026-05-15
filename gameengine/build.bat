@echo off
echo ========================================
echo Building Fighter Engine Project
echo ========================================
echo.

REM Try to find MSBuild using vswhere
set "VSPATH="
for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`) do (
    set VSPATH=%%i
)

if not exist "%VSPATH%\MSBuild\Current\Bin\MSBuild.exe" (
    if exist "%VSPATH%\MSBuild\15.0\Bin\MSBuild.exe" (
        set "MSBUILD=%VSPATH%\MSBuild\15.0\Bin\MSBuild.exe"
    )
) else (
    set "MSBUILD=%VSPATH%\MSBuild\Current\Bin\MSBuild.exe"
)

if not exist "%MSBUILD%" (
    echo ERROR: MSBuild not found!
    echo Please install Visual Studio 2022 with C++ development tools.
    pause
    exit /b 1
)

echo MSBuild found at: %MSBUILD%
echo.
echo Building project...
echo.

"%MSBUILD%" FighterEngine.sln /p:Configuration=Release /p:Platform=x64 /v:minimal

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo Build successful!
    echo ========================================
    echo.
    echo Run the game: x64\Release\FighterEngine.exe
    echo.
) else (
    echo.
    echo ========================================
    echo Build failed!
    echo ========================================
    echo.
)

pause
