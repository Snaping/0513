@echo off
chcp 65001 >nul
echo ========================================
echo    AI Editor Pro - 智能代码编辑器
echo ========================================
echo.
echo [提示] 正在启动编辑器...
echo.
echo [快捷键]
echo   Ctrl + N       新建文件
echo   Ctrl + O       打开文件
echo   Ctrl + Shift + A  打开 AI 助手
echo.
echo [功能]
echo   - Monaco Editor (VSCode 同款编辑器)
echo   - 多标签页编辑
echo   - AI 助手面板
echo   - AI 插件管理
echo   - 文件打开和保存
echo.

npm start

if errorlevel 1 (
    echo.
    echo [错误] 启动失败，请确保已正确安装依赖。
    echo 如果是首次运行，请先运行: npm install
    pause
)
