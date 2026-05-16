@echo off
echo 正在编译图片编辑器...
g++ -std=c++17 -municode -mwindows -o ImageEditor.exe main.cpp ImageEditor.cpp -lgdiplus -lcomctl32 -lole32
if %ERRORLEVEL% EQU 0 (
    echo 编译成功！
    echo 可执行文件: ImageEditor.exe
    echo.
    echo 是否运行程序？(Y/N)
    set /p run=
    if /i "%run%"=="Y" (
        start ImageEditor.exe
    )
) else (
    echo 编译失败！
)
pause
