@echo off
echo ========================================
echo    AI Editor Pro - 启动脚本
echo ========================================
echo.

if not exist "node_modules" (
    echo [1/2] 首次运行，正在安装依赖...
    call npm install
    echo.
)

echo [2/2] 正在启动 AI Editor Pro...
echo.
call npm start

pause
